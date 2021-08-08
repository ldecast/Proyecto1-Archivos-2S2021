#include <iostream>
#include <string>
#include "../model/builder.h"
#include "./disks/mkdisk.cpp"

int bloque() //Falta que lleve el usuario, grupo, etc
{
    if (x.keyword == "__MKDISK")
        return makeDisk(x.size, x.fit, x.unit, x.path);

    return 0;
}