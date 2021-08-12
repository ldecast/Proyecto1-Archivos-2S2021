#include <iostream>
#include <string.h>
#include "../structures.h"
#include "../handler.h"
#include "func.h"

int validations(MBR _mbr, int _index, int _ini, int _size)
{
    if ((_ini + _size) > _mbr.mbr_tamano)
        return 0;
    if (_index < 3 && _mbr.mbr_partition[_index + 1].part_size > 0 && ((_ini + _size) >= _mbr.mbr_partition[_index + 1].part_start))
        return 0;
    return 1;
}

void CrearPrimaria(int _size, FILE *_file)
{
    fwrite("\0", _size, 1, _file);
}

void CrearExtendida(int _size, int _nstart, char _fit, FILE *_file)
{ // Configurando el EBR inicial con los datos de una partición lógica vacía
    EBR ebr;
    ebr.part_fit = _fit;
    strcpy(ebr.part_name, "");
    ebr.part_next = -1;
    ebr.part_size = 0;
    ebr.part_start = _nstart;
    ebr.part_status = '0';
    // fseek(pFile, 0, SEEK_SET);
    fwrite(&ebr, sizeof(EBR), 1, _file);
    // fseek(pFile, 0, SEEK_SET);
    fwrite("\0", _size, 1, _file);
}

int CrearLogica(MBR _mbr, int _size, FILE *_file, char _fit, std::string _name)
{
    int index = existeExtendida(_mbr);
    if (index == -1)
        return coutError("No existe una partición extendida para crear la partición lógica.", _file);
    partition extendida = _mbr.mbr_partition[index];

    EBR ebr_inicial;
    fseek(_file, extendida.part_start, SEEK_SET);
    fread(&ebr_inicial, sizeof(EBR), 1, _file);

    if (_size + getLogicsSize(ebr_inicial, _file) > extendida.part_size)
        return coutError("El -size requerido de la partición lógica sobrepasa el tamaño de la partición extendida", _file);

    if (existeNombreEBR(ebr_inicial, _name, _file))
        return coutError("El nombre a asignar ya existe como partición lógica.", _file);

    if (ebr_inicial.part_status == '0')
    {
        ebr_inicial.part_fit = _fit;
        strcpy(ebr_inicial.part_name, _name.c_str());
        ebr_inicial.part_size = _size;
        ebr_inicial.part_status = '1';
        fseek(_file, extendida.part_start, SEEK_SET);
        fwrite(&ebr_inicial, sizeof(EBR), 1, _file);
        fwrite("\0", _size, 1, _file);
    }
    else
    {
        // fseek(pFile, 0, SEEK_SET);
        EBR ebr_final = getLastEBR(ebr_inicial, _file);
        EBR ebr_new;
        ebr_new.part_fit = _fit;
        strcpy(ebr_new.part_name, _name.c_str());
        ebr_new.part_next = -1;
        ebr_new.part_size = _size;
        ebr_new.part_start = ebr_final.part_start + sizeof(EBR) + ebr_final.part_size; // Tomo en cuenta el EBR del anterior más el size de la partición lógica
        ebr_new.part_status = '1';

        ebr_final.part_next = ebr_new.part_start; // +1?
        // std::cout << ebr_final.part_name << std::endl;
        fseek(_file, ebr_final.part_start, SEEK_SET);
        fwrite(&ebr_final, sizeof(EBR), 1, _file);

        fseek(_file, ebr_new.part_start, SEEK_SET);
        fwrite(&ebr_new, sizeof(EBR), 1, _file);
        fwrite("\0", _size, 1, _file);
    }
    return 1;
}

int CrearParticion(int _size, char _unit, std::string _path, char _type, char _fit, std::string _name)
{
    char src[_path.size() + 1];
    strcpy(src, _path.c_str());
    MBR mbr;
    FILE *pFile;
    pFile = fopen(src, "rb+");
    fseek(pFile, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, pFile);

    int tam = getSize(_unit, _size);
    int nstart = mbr.mbr_partition[0].part_start;
    if (_type == 'L')
    {
        if (CrearLogica(mbr, tam, pFile, _fit, _name))
        {
            fclose(pFile);
            pFile = NULL;
            return 1;
        }
        return 0;
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            if (mbr.mbr_partition[i].part_status == '0')
            {
                nstart = mbr.mbr_partition[i].part_start;
                if (!validations(mbr, i, nstart, tam)) //return coutError("Ya no se encuentra espacio disponible para crear la partición.");
                    continue;
                if (_type == 'E' && existeExtendida(mbr) > -1)
                    return coutError("No puede existir más de una partición extendida dentro del disco.", pFile);
                if (existeNombreMBR(mbr, _name))
                    return coutError("El nombre a asignar ya existe como partición.", pFile);

                //Modificiación del MBR
                mbr.mbr_partition[i].part_fit = _fit;
                strcpy(mbr.mbr_partition[i].part_name, _name.c_str());
                mbr.mbr_partition[i].part_size = tam;
                mbr.mbr_partition[i].part_start = nstart;
                mbr.mbr_partition[i].part_status = '1';
                mbr.mbr_partition[i].part_type = _type;
                fseek(pFile, 0, SEEK_SET);
                fwrite(&mbr, sizeof(MBR), 1, pFile);

                if (_type == 'P')
                {
                    CrearPrimaria(tam, pFile);
                }
                else if (_type == 'E')
                {
                    CrearExtendida(tam, nstart, _fit, pFile);
                }
                fclose(pFile);
                pFile = NULL;
                return 1;
            }
        }
        return coutError("No se encontró ningún espacio apto para crear la partición.", pFile);
    }
}

int AdministrarEspacio(int _add, char _unit, std::string _path, std::string _name) // revisarlo
{
    char src[_path.size() + 1];
    strcpy(src, _path.c_str());
    MBR mbr;
    FILE *pFile;
    pFile = fopen(src, "rb+");
    fseek(pFile, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, pFile);

    char type = existPartition(mbr, _name, pFile);
    if (type == '0')
        return coutError("No se encuentra ninguna partición con ese nombre asignado.", pFile);

    int size = getSize(_unit, _add);
    if (type == 'P' || type == 'E')
    {
        partition _particion = getPartition(mbr, _name, pFile);
        std::cout << "_particion.part_size: " + std::to_string(_particion.part_size) << std::endl;
        if (getPartitionsSize(mbr) + size > mbr.mbr_tamano) //revisar tamano
            return coutError("El tamaño que se desea agregar excede el tamaño del disco.", pFile);
        if (_particion.part_size + size <= 0)
            return coutError("El tamaño al reducir la partición debe ser mayor que 0.", pFile);
        _particion.part_size = _particion.part_size + size;
        fseek(pFile, _particion.part_start, SEEK_SET);
        fwrite(&_particion, sizeof(partition), 1, pFile);
        fwrite("\0", _particion.part_size, 1, pFile);
        std::cout << "_particion.part_size: " + std::to_string(_particion.part_size) << std::endl;
    }
    else if (type == 'L')
    {
        partition extendida = mbr.mbr_partition[existeExtendida(mbr)];
        EBR _ebr;
        fseek(pFile, extendida.part_start, SEEK_SET);
        fread(&_ebr, sizeof(EBR), 1, pFile);
        _ebr = getLogicPartition(_ebr, _name, pFile);
        std::cout << "_ebr.part_size: " + std::to_string(_ebr.part_size) << std::endl;
        if (getLogicsSize(_ebr, pFile) + size > extendida.part_size)
            return coutError("El tamaño que se desea agregar excede el tamaño de la partición extendida.", pFile);
        if (_ebr.part_size + size <= 0)
            return coutError("El tamaño al reducir la partición debe ser mayor que 0.", pFile);
        _ebr.part_size = _ebr.part_size + size;
        fseek(pFile, _ebr.part_start, SEEK_SET);
        fwrite(&_ebr, sizeof(EBR), 1, pFile);
        fwrite("\0", _ebr.part_size, 1, pFile);
        std::cout << "_ebr.part_size: " + std::to_string(_ebr.part_size) << std::endl;
    }
    fclose(pFile);
    pFile = NULL;
    return 1;
}

int BorrarParticion(char _delete, std::string _path, std::string _name)
{
    char src[_path.size() + 1];
    strcpy(src, _path.c_str());
    MBR mbr;
    FILE *pFile;
    pFile = fopen(src, "rb+");
    fseek(pFile, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, pFile);

    char type = existPartition(mbr, _name, pFile);
    if (type == '0')
        return coutError("No se encuentra ninguna partición con ese nombre asignado.", pFile);

    if (type == 'P' || type == 'E')
    {
        int i = getPartitionIndex(mbr, _name, pFile);
        int _presize = mbr.mbr_partition[i].part_size;
        mbr.mbr_partition[i].part_fit = mbr.disk_fit;
        strcpy(mbr.mbr_partition[i].part_name, "");
        mbr.mbr_partition[i].part_size = 0;
        mbr.mbr_partition[i].part_status = '0';
        mbr.mbr_partition[i].part_type = 'P';
        fseek(pFile, 0, SEEK_SET);
        fwrite(&mbr, sizeof(MBR), 1, pFile);
        if (_delete == 'C')
            fwrite("\0", _presize, 1, pFile);
    }
    else if (type == 'L') //borrar referencia del EBR anterior
    {
        partition extendida = mbr.mbr_partition[existeExtendida(mbr)];
        EBR _ebr;
        fseek(pFile, extendida.part_start, SEEK_SET);
        fread(&_ebr, sizeof(EBR), 1, pFile);
        _ebr = getLogicPartition(_ebr, _name, pFile);
        // print(std::to_string(extendida.part_start));
        // print(std::string(_ebr.part_name));
        EBR _ebr_previous = getEBRprevious(extendida.part_start, _ebr.part_start, pFile);
        _ebr_previous.part_next = _ebr.part_next;
        fseek(pFile, _ebr_previous.part_start, SEEK_SET);
        fwrite(&_ebr_previous, sizeof(EBR), 1, pFile);
        std::cout << "_ebr.part_size: " + std::to_string(_ebr.part_size) << std::endl;
        std::cout << "_ebr.part_name: " + std::string(_ebr.part_name) << std::endl;
        fseek(pFile, _ebr.part_start, SEEK_SET);
        fwrite("\0", sizeof(EBR), 1, pFile);
        if (_delete == 'C')
            fwrite("\0", _ebr.part_size, 1, pFile);
        // std::cout << _ebr.part_size << std::endl;
    }
    fclose(pFile);
    pFile = NULL;
    return 1;
}

int formatDisk(std::string _size, std::string _unit, std::string _path, std::string _type, std::string _fit, std::string _delete, std::string _name, std::string _add)
{
    if (_path == "" || _name == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    std::string npath = getPath(_path);
    if (!isDir(npath))
        return coutError("Error: el disco no existe.", NULL);
    if (_name.length() > 16)
        return coutError("El nombre de la partición no puede exceder a 16 caracteres.", NULL);

    char nunit = getUnit(_unit, 'k');
    if (_add != "")
        return AdministrarEspacio(std::stoi(_add), nunit, npath, _name);

    if (_delete != "")
        return BorrarParticion(charDelete(_delete), npath, _name);

    char ntype = charType(_type, 'P');
    char nfit = getFit(_fit, 'W');
    int nsize = (_size != "") ? intSize(_size) : intSize("-1");
    return CrearParticion(nsize, nunit, npath, ntype, nfit, _name);
}