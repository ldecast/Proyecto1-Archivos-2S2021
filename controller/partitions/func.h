#ifndef FUNC_PARTITIONS
#define FUNC_PARTITIONS

#include <iostream>
#include <string.h>
#include <fstream>
#include "../../model/structures.h"

Disk_id buildID(std::string _id)
{
    Disk_id disk_id;
    disk_id._carnet = _id.substr(0, 2);
    disk_id._number_id = std::stoi(_id.substr(2, 1));
    disk_id._letter_id = _id[_id.length() - 1];

    return disk_id;
}

int existMountedID(Disk_id _disk_id)
{
    for (int i = 0; i < _particiones_montadas.size(); i++)
    {
        MOUNTED mounted = _particiones_montadas[i];
        if (mounted.id._carnet == _disk_id._carnet && mounted.id._number_id == _disk_id._number_id && mounted.id._letter_id == _disk_id._letter_id)
            return i;
    }
    return -1;
}

/* MOUNTED getMountedByID(Disk_id _disk_id)
{
    for (int i = 0; i < _particiones_montadas.size(); i++)
    {
        MOUNTED mounted = _particiones_montadas[i];
        if (mounted.id._carnet == _disk_id._carnet && mounted.id._number_id == _disk_id._number_id && mounted.id._letter_id == _disk_id._letter_id)
            return mounted;
    }
    return {};
} */

#endif