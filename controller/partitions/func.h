#ifndef FUNC_PARTITIONS
#define FUNC_PARTITIONS

#include <iostream>
#include <string.h>
#include <fstream>
#include "../../model/structures.h"

Disk_id buildID(std::string _id)
{
    Disk_id disk_id;
    disk_id._number_id = std::stoi(_id.substr(2, 1));
    disk_id._letter_id = _id[_id.length() - 1];

    return disk_id;
}

bool existMountedID(Disk_id _disk_id)
{
    for (int i = 0; i < _particiones_montadas.size(); i++)
    {
        MOUNTED montada = _particiones_montadas[i];
        if (montada.id._number_id == _disk_id._number_id && montada.id._letter_id == _disk_id._letter_id)
            return true;
    }
    return false;
}

MOUNTED getMountedByID(Disk_id _disk_id)
{
    for (int i = 0; i < _particiones_montadas.size(); i++)
    {
        MOUNTED montada = _particiones_montadas[i];
        if (montada.id._number_id == _disk_id._number_id && montada.id._letter_id == _disk_id._letter_id)
            return montada;
    }
    return {};
}

#endif