#include <iostream>
#include <string.h>
#include <algorithm>
#include "mbr.cpp"
// #include "disk.cpp"

int classifier(std::string _name, std::string _path, std::string _id, std::string _ruta, std::string _root)
{
    transform(_name.begin(), _name.end(), _name.begin(), ::tolower);
    std::string n_path = buildPath(_path);
    char n_id[_path.size() + 1];
    strcpy(n_id, _path.c_str());

    if (_name == "mbr")
        return ReportMBR(n_path, n_id);

    // if (_name == "disk")
    //     return ReportDisk();

    return coutError("El nombre del reporte a generar no es válido: " + _name, NULL);
}