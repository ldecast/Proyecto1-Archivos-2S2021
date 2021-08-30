#include <iostream>
#include <string.h>
#include <time.h>
#include "../../model/structures.h"
#include "../handler.h"

int CrearDisco(int _size, char _fit, char _unit, std::string _path)
{
    char sc[_path.size() + 1];
    strcpy(sc, _path.c_str());
    FILE *file = NULL;
    file = fopen(sc, "r");
    if (file != NULL)
        return coutError("Error: El disco ya existe.", file);

    //Tamaño para distintas unidades que se piden
    int tam = getSize(_unit, _size);

    file = fopen(sc, "wb");
    fwrite("\0", 1, 1, file);
    fseek(file, tam, SEEK_SET);
    fwrite("\0", 1, 1, file);

    //////MBR//////
    MBR mbr;
    mbr.mbr_tamano = tam;
    mbr.mbr_fecha_creacion = getCurrentTime();
    mbr.mbr_disk_signature = rand() % 1000;
    mbr.disk_fit = _fit;
    for (int i = 0; i < 4; i++)
    {
        mbr.mbr_partition[i].part_status = '0';
        mbr.mbr_partition[i].part_type = 'P';
        mbr.mbr_partition[i].part_fit = _fit;
        mbr.mbr_partition[i].part_start = 0;
        mbr.mbr_partition[i].part_size = 0;
        strcpy(mbr.mbr_partition[i].part_name, "");
    }

    //Escritura del MBR
    fseek(file, 0, SEEK_SET);
    fwrite(&mbr, sizeof(MBR), 1, file);
    fclose(file);
    file = NULL;
    /* std::cout << "--------------------DISCO CREADO--------------------" << std::endl;
    std::cout << "Fecha de creación: " << ctime(&mbr.mbr_fecha_creacion);
    std::cout << "Signature: " << mbr.mbr_disk_signature << std::endl;
    std::cout << "Tamaño: " << mbr.mbr_tamano << std::endl;
    std::cout << "Fit: " << mbr.disk_fit << std::endl; */
    return 1;
}

int makeDisk(std::string _size, std::string _fit, std::string _unit, std::string _path)
{
    try
    {
        if (_size == "" || _path == "")
            return coutError("Error: faltan parámetros obligatorios.", NULL);
        int ns = intSize(_size);
        char nf = getFit(_fit, 'F');
        char nu = getUnit(_unit, 'm');
        std::string np = buildPath(_path);
        std::cout << "CREANDO DISCO EN LA RUTA: \"" + np + "\"" << std::endl;
        return CrearDisco(ns, nf, nu, np);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return coutError("ERROR!", NULL);
    }
}
