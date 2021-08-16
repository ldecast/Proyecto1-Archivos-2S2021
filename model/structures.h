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

struct EBR
{
    char part_status;
    char part_fit; //b,f,w
    int part_start;
    int part_size;
    int part_next; //próximo EBR || -1
    char part_name[16];
};

struct Disk_id
{
    std::string _carnet = "38";
    int _number_id = 1;
    char _letter_id = 'A';
};

struct MOUNTED
{
    std::string path;
    Disk_id id;
    char type;
    partition particion;
    EBR logica;
};

std::vector<MOUNTED> _particiones_montadas;

#endif