#include <iostream>
#include <string>
#include "../model/builder.h"
#include "./disks/mkdisk.cpp"
#include "./disks/rmdisk.cpp"
#include "./disks/fdisk.cpp"

int bloque(struct command x) //Falta que lleve el usuario, grupo, etc
{
    if (x.keyword == "__MKDISK")
        return makeDisk(x.size, x.fit, x.unit, x.path);

    if (x.keyword == "__RMDISK")
        return removeDisk(x.path);

    if (x.keyword == "__FDISK")
        return formatDisk(x.size, x.unit, x.path, x.type, x.fit, x.delet, x.name, x.add);

    return 0;
}