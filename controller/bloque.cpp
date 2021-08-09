#include <iostream>
#include <string>
#include "../model/builder.h"
#include "./disks/mkdisk.cpp"
#include "./disks/rmdisk.cpp"

int bloque() //Falta que lleve el usuario, grupo, etc
{
    if (x.keyword == "__MKDISK")
        return makeDisk(x.size, x.fit, x.unit, x.path);

    if (x.keyword == "__RMDISK")
        return removeDisk(x.path);

    return 0;
}