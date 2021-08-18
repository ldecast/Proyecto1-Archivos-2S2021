#include <iostream>
#include <string.h>
#include <algorithm>
#include "../partitions/func.h"
#include "mbr.cpp"
// #include "disk.cpp"

int classifier(std::string _name, std::string _path, std::string _id, std::string _ruta, std::string _root)
{
    if (_name == "" || _path == "" || _id == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    transform(_name.begin(), _name.end(), _name.begin(), ::tolower);
    std::string dir_output = buildPath(_path);

    Disk_id disk_id = buildID(_id);

    if (!existMountedID(disk_id))
        return coutError("No se encuentra ninguna partición montada con el id '" + _id + "'.", NULL);

    MOUNTED mounted = getMountedByID(disk_id);

    if (_name == "mbr")
        return ReportMBR(mounted, dir_output);

    // if (_name == "disk")
    //     return ReportDisk();

    return coutError("El nombre del reporte a generar no es válido: " + _name, NULL);
}