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

#endif