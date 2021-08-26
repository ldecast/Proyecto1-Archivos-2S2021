#ifndef FUNC_FILESYSTEM
#define FUNC_FILESYSTEM

#include <iostream>
#include <stdio.h>
#include <math.h> /* floor */
#include "../../model/structures.h"
#include "../partitions/func.h"
#include "../handler.h"

Superbloque getSuperBloque(MOUNTED _mounted)
{
    Superbloque super_bloque;
    FILE *file = fopen(_mounted.path.c_str(), "rb");

    int sb_start;
    switch (_mounted.type)
    {
    case 'P':
        sb_start = _mounted.particion.part_start;
        break;
    case 'L':
        sb_start = _mounted.logica.part_start;
        break;
    default:
        break;
    }

    fseek(file, sb_start, SEEK_SET);                    // Mover el puntero al inicio del superbloque
    fread(&super_bloque, sizeof(Superbloque), 1, file); // Leer el superbloque

    fclose(file);
    file = NULL;
    return super_bloque;
}

char charFormat(std::string _format)
{
    char nformat = 'N';
    transform(_format.begin(), _format.end(), _format.begin(), ::tolower);
    if (_format == "fast")
        nformat = 'R'; //rápida
    else if (_format == "full" || _format == "")
        nformat = 'C'; //completa
    else
        std::cout << "Error: parámetro -type no válido: " + std::to_string(nformat) << std::endl;
    return nformat;
}

int _2_or_3fs(std::string _fs)
{
    int nfs = -1;
    transform(_fs.begin(), _fs.end(), _fs.begin(), ::tolower);
    if (_fs == "3fs")
        nfs = 3; //ext3
    else if (_fs == "2fs" || _fs == "")
        nfs = 2; //ext2
    return nfs;
}

int _number_inodos(int _part_size)
{
    return (int)floor((_part_size - sizeof(Superbloque) / (1 + 3 + sizeof(InodosTable) + 3 * 64)));
}

#endif