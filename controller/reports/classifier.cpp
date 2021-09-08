#include <iostream>
#include <string.h>
#include <algorithm>
#include "../partitions/func.h"
#include "mbr.cpp"
#include "disk.cpp"
#include "inode.cpp"
#include "block.cpp"
#include "bm_inode.cpp"
#include "bm_block.cpp"
#include "sb.cpp"
#include "file.cpp"
#include "ls.cpp"

int classifier(std::string _name, std::string _path, std::string _id, std::string _ruta, std::string _root)
{
    if (_name == "" || _path == "" || _id == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    transform(_name.begin(), _name.end(), _name.begin(), ::tolower);
    std::string dir_output = buildPath(_path);

    Disk_id disk_id = buildID(_id);

    int index = existMountedID(disk_id);
    if (index == -1)
        return coutError("No se encuentra ninguna partición montada con el id '" + _id + "'.", NULL);

    MOUNTED mounted = _particiones_montadas[index];
    std::string grafo;

    if (_name == "mbr")
        grafo = ReportMBR(mounted);

    else if (_name == "disk")
        grafo = ReportDisk(mounted);

    else if (_name == "inode")
        grafo = ReportInodes(mounted);

    else if (_name == "block")
        grafo = ReportBlocks(mounted);

    else if (_name == "bm_inode")
        grafo = ReportBitMapInodes(mounted);

    else if (_name == "bm_block")
        grafo = ReportBitMapBlocks(mounted);

    else if (_name == "sb")
        grafo = ReportSuperBloque(mounted);

    else if (_name == "file")
        grafo = ReportFile(mounted, _ruta);

    else if (_name == "ls")
        grafo = ReportLS(mounted);

    else
        return coutError("El nombre del reporte a generar no es válido: " + _name, NULL);

    if (grafo == "-")
        return 0;

    writeDot(grafo);
    generateReport(dir_output);
    return 1;
}