#ifndef STRUCTURES
#define STRUCTURES

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include <time.h>

using namespace std;

string root = "/home/ldecast/Escritorio"; //url raiz para guardar

struct partition
{
    char part_status;
    char part_type; // primaria, extendida, logica
    char part_fit;  // las opciones por default
    int part_start;
    int part_size;
    char part_name[16];
};

struct MBR
{
    int mbr_tamano;
    time_t mbr_fecha_creacion;
    int mbr_disk_signature;
    char disk_fit;
    partition mbr_partition[4];
};

#endif