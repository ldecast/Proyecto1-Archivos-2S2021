#include <iostream>
#include <string.h>
#include "../../model/structures.h"
#include "../handler.h"
#include "func.h"

using std::string;

int MontarParticion(string _path, string _name);
Disk_id assignID(string _path);
int printMOUNTED();

int mount(string _path, string _name)
{
    if (_path == "" || _name == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    string n_path = getPath(_path);
    if (!isDir(n_path))
        return coutError("Error: el disco no existe.", NULL);

    return MontarParticion(n_path, _name);
}

int MontarParticion(string _path, string _name)
{
    FILE *_file = fopen(_path.c_str(), "rb+");

    MBR mbr;
    fseek(_file, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, _file);

    char tipo = existPartition(mbr, _name, _file);

    MOUNTED _mounted;
    _mounted.id = assignID(_path);
    _mounted.type = tipo;
    _mounted.path = _path;

    if (tipo == 'P' || tipo == 'E')
    {
        partition _particion = getPartition(mbr, _name, _file);

        // print("_particion.part_name: " + string(_particion.part_name));
        // print("_particion.part_size: " + std::to_string(_particion.part_size));

        _mounted.particion = _particion;
    }
    else if (tipo == 'L')
    {
        partition _extendida = mbr.mbr_partition[existeExtendida(mbr)];
        EBR _ebr_to_mount, _ebr_initial;
        fseek(_file, _extendida.part_start, SEEK_SET);
        fread(&_ebr_initial, sizeof(EBR), 1, _file);
        _ebr_to_mount = getLogicPartition(_ebr_initial, _name, _file);

        // print("_ebr_to_mount.part_name: " + string(_ebr_to_mount.part_name));
        // print("_ebr_to_mount.part_size: " + std::to_string(_ebr_to_mount.part_size));

        _mounted.logica = _ebr_to_mount;
    }
    else
        return coutError("No se encuentra ninguna partición con ese nombre asignado.", _file);

    _particiones_montadas.push_back(_mounted);
    return printMOUNTED();
}

Disk_id assignID(string _path)
{
    MOUNTED mounted;
    Disk_id tmp;
    for (int i = _particiones_montadas.size() - 1; i >= 0; i--)
    {
        mounted = _particiones_montadas[i];
        if (mounted.path == _path)
        {
            tmp._number_id = mounted.id._number_id;
            tmp._letter_id = char(int(mounted.id._letter_id) + 1);
            return tmp;
        }
    }
    _number_id += 1;
    tmp._number_id = _number_id;
    return tmp;
}

int printMOUNTED()
{
    for (int i = 0; i < _particiones_montadas.size(); i++)
    {
        MOUNTED _mounted = _particiones_montadas[i];
        string tmp = _mounted.id._carnet + std::to_string(_mounted.id._number_id) + _mounted.id._letter_id;
        string name = (_mounted.type == 'L') ? _mounted.logica.part_name : _mounted.particion.part_name;
        std::cout << _mounted.path + "|" + name + "|" + tmp << std::endl;
    }
    return 1;
}