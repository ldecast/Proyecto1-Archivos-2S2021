#include <iostream>
#include <string.h>
#include "../../model/structures.h"
#include "../handler.h"
#include "func.h"
#include "../partitions/fit.cpp"
#include "../partitions/add.cpp"
#include "../partitions/delete.cpp"

void CrearPrimaria(int _size, int _nstart, FILE *_file)
{
    fseek(_file, _nstart, SEEK_SET);
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
    fseek(_file, _nstart, SEEK_SET);
    fwrite(&ebr, sizeof(EBR), 1, _file);
    // fwrite("\0", _size, 1, _file);
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
    // std::cout << "ebr_inicial.part_name: ";
    // std::cout << ebr_inicial.part_name << std::endl;

    if (existeNombreEBR(ebr_inicial, _name, _file))
        return coutError("El nombre a asignar ya existe como partición lógica.", _file);

    if (_size + getLogicsSize(ebr_inicial, 0, _file) > extendida.part_size)
        return coutError("El -size requerido de la partición lógica sobrepasa el tamaño de la partición extendida", _file);

    if (ebr_inicial.part_size == 0) // Modifica el EBR inicial si no hay una partición lógica creada
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
        fseek(_file, extendida.part_start, SEEK_SET);
        EBR ebr_final = getLogicByFit(ebr_inicial, _fit, _file, _size); // ebr_to_update
        if (ebr_final.part_status == '!')
            return coutError("No se pudo crear la partición lógica.", _file);
        EBR ebr_new;
        ebr_new.part_fit = _fit;
        strcpy(ebr_new.part_name, _name.c_str());
        ebr_new.part_next = -1;
        ebr_new.part_size = _size;
        ebr_new.part_start = ebr_final.part_start + ebr_final.part_size; // + sizeof(EBR)
        ebr_new.part_status = '1';

        ebr_final.part_next = ebr_new.part_start; // +1?
        // std::cout << "ebr_new.part_name: " + std::string(ebr_new.part_name) + " ebr_new.part_start: " + std::to_string(ebr_new.part_start) << std::endl;
        fseek(_file, ebr_final.part_start, SEEK_SET);
        fwrite(&ebr_final, sizeof(EBR), 1, _file);

        fseek(_file, ebr_new.part_start, SEEK_SET);
        fwrite(&ebr_new, sizeof(EBR), 1, _file);
        fwrite("\0", _size, 1, _file);
    }
    fclose(_file);
    _file = NULL;
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
    int nstart = sizeof(MBR);
    if (_type == 'L')
    {
        return CrearLogica(mbr, tam, pFile, _fit, _name);
    }
    else
    {
        int options[] = {-1, -1, -1, -1};
        for (int i = 0; i < 4; i++)
        {
            /* nstart += mbr.mbr_partition[i].part_start + mbr.mbr_partition[i].part_size; */
            nstart += mbr.mbr_partition[i].part_size;
            if (mbr.mbr_partition[i].part_status == '0')
            {
                if (!Validations(mbr, i, nstart, tam)) //return coutError("Ya no se encuentra espacio disponible para crear la partición.", pFile);
                    continue;
                if (_type == 'E' && existeExtendida(mbr) > -1)
                    return coutError("No puede existir más de una partición extendida dentro del disco.", pFile);
                if (existeNombreMBR(mbr, _name))
                    return coutError("El nombre a asignar ya existe como partición.", pFile);

                if (i < 3)
                {
                    if (mbr.mbr_partition[i + 1].part_start != 0)
                        options[i] = mbr.mbr_partition[i + 1].part_start - mbr.mbr_partition[i].part_start - _size;
                    else
                        options[i] = 0;
                }
                else if (i == 3)
                {
                    if (mbr.mbr_partition[i].part_start != 0)
                        options[i] = mbr.mbr_tamano - mbr.mbr_partition[i].part_start - _size;
                    else
                        options[i] = 0;
                }
            }
            // std::cout << mbr.mbr_partition[i].part_size << std::endl;
        }

        int i = getPartitionByFit(options, _fit); //ahora toca con el EBR
        if (i == -1)
            return coutError("No se encontró ningún espacio apto para crear la partición.", pFile);

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
            CrearPrimaria(tam, nstart, pFile);
        }
        else if (_type == 'E')
        {
            CrearExtendida(tam, nstart, _fit, pFile);
        }
        fclose(pFile);
        pFile = NULL;
        return 1;
    }
    // return coutError("No se encontró ningún espacio apto para crear la partición.", pFile);
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
    {
        char ans;
        std::cout << "¿Desea eliminar la partición '" + _name + "' ubicada en el disco: '" + npath + "?' [Y/n] ";
        std::cin >> ans;
        if (ans == 'Y' || ans == 'y')
            return BorrarParticion(charDelete(_delete), npath, _name);
        else
            return 1;
    }

    char ntype = charType(_type, 'P');
    char nfit = getFit(_fit, 'W');
    int nsize = (_size != "") ? intSize(_size) : intSize("-1");
    return CrearParticion(nsize, nunit, npath, ntype, nfit, _name);
}