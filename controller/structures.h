#ifndef STRUCTURES
#define STRUCTURES

#include <iostream>
#include <stdio.h>

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