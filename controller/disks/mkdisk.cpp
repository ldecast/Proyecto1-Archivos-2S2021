#include <iostream>
#include <string.h>
#include "../structures.h"
#include "../handler.h"

int CrearDisco(int _size, char _fit, char _unit, std::string _path)
{
    char sc[_path.size() + 1];
    strcpy(sc, _path.c_str());
    FILE *file = NULL;
    file = fopen(sc, "r");
    if (file != NULL)
    {
        std::cout << "Error: El disco ya existe." << std::endl;
        return 0;
    }

    //Tamaño para distintas unidades que se piden
    int tam;
    if (_unit == 'k') //kb
        tam = _size * 1000;
    else if (_unit == 'm') //mb
        tam = _size * 1000 * 1000;

    file = fopen(sc, "wb");
    fwrite("\0", 1, 1, file);
    /*se pone el puntero en el tamaño deseado del archivo y esto automaticamente 
	hace que el archivo tenga ese size*/
    fseek(file, tam, SEEK_SET); // objeto file, size de cuanto se quiere mover, al inicio
    fwrite("\0", 1, 1, file);

    //////MBR//////
    MBR mbr;
    mbr.mbr_tamano = tam;
    mbr.mbr_fecha_creacion = time(0);
    mbr.mbr_disk_signature = rand() % 1000;
    mbr.disk_fit = _fit;
    for (int i = 0; i < 4; i++)
    {
        mbr.mbr_partition[i].part_status = '0';
        mbr.mbr_partition[i].part_type = 'P';
        mbr.mbr_partition[i].part_fit = _fit;
        mbr.mbr_partition[i].part_start = 0;
        mbr.mbr_partition[i].part_size = tam;
        strcpy(mbr.mbr_partition[i].part_name, "");
    }

    //Escritura del MBR
    fseek(file, 0, SEEK_SET);
    fwrite(&mbr, sizeof(MBR), 1, file);
    fclose(file);

    std::cout << "--------------------DISCO CREADO--------------------" << std::endl;
    std::cout << "Fecha de creación: " << asctime(gmtime(&mbr.mbr_fecha_creacion));
    std::cout << "Signature: " << mbr.mbr_disk_signature << std::endl;
    std::cout << "Tamaño: " << mbr.mbr_tamano << std::endl;
    std::cout << "Fit: " << mbr.disk_fit << std::endl;
    return 1;
}

int makeDisk(std::string _size, std::string _fit, std::string _unit, std::string _path)
{
    try
    {
        if (_size == "" || _path == "")
        {
            std::string err;
            err.append(std::string("Error: faltan parámetros obligatorios: ") + ((_size == "") ? "'-size'" : "'-path'"));
            std::cout << err << std::endl;
            return 0;
        }
        int ns = intSize(_size);
        char nf = getFit(_fit);
        char nu = getUnit(_unit);
        std::string np = getPath(_path);
        std::cout << "CREANDO DISCO EN LA RUTA: \"" + np + "\"" << std::endl;
        return CrearDisco(ns, nf, nu, np);
    }
    catch (const std::exception &e)
    {
        std::cout << "ERROR!" << std::endl;
        std::cerr << e.what() << '\n';
        return 0;
    }
}
