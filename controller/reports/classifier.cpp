#include <iostream>
#include <string.h>
#include <algorithm>
#include "mbr.cpp"
// #include "disk.cpp"

int classifier(std::string _name, std::string _path, std::string _id, std::string _ruta, std::string _root)
{
    if (_name == "" || _path == "" || _id == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    transform(_name.begin(), _name.end(), _name.begin(), ::tolower);
    std::string n_path = buildPath(_path);

    Disk_id disk_id;
    disk_id._number_id = std::stoi(_id.substr(2, 1));
    disk_id._letter_id = _id[_id.length() - 1];

    if (_name == "mbr")
        return ReportMBR(n_path, disk_id);

    // if (_name == "disk")
    //     return ReportDisk();

    return coutError("El nombre del reporte a generar no es válido: " + _name, NULL);
}