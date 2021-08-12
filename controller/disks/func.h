#ifndef FUNC
#define FUNC

#include <iostream>
#include <string.h>
#include "../structures.h"

bool existLogicPartition(EBR _ebr, std::string _name, FILE *_file)
{
    if (_ebr.part_name == _name)
        return true;
    else if (_ebr.part_next != -1)
    {
        fseek(_file, _ebr.part_next, SEEK_SET);
        fread(&_ebr, sizeof(EBR), 1, _file);
        return existLogicPartition(_ebr, _name, _file);
    }
    else
        return false;
}

char existPartition(MBR _mbr, std::string _name, FILE *_file)
{
    for (int i = 0; i < 4; i++)
    {
        if (_mbr.mbr_partition[i].part_name == _name)
            return _mbr.mbr_partition[i].part_type;
        if (_mbr.mbr_partition[i].part_type == 'E')
        {
            EBR ebr_inicial;
            fseek(_file, _mbr.mbr_partition[i].part_start, SEEK_SET);
            fread(&ebr_inicial, sizeof(EBR), 1, _file);
            if (existLogicPartition(ebr_inicial, _name, _file))
                return 'L';
            else
                continue;
        }
    }
    return '0';
}

EBR getLogicPartition(EBR _ebr, std::string _name, FILE *_file)
{
    // std::cout << _ebr.part_name << std::endl;
    if (_ebr.part_name == _name)
        return _ebr;
    if (_ebr.part_next != -1)
    {
        fseek(_file, _ebr.part_next, SEEK_SET);
        fread(&_ebr, sizeof(EBR), 1, _file);
        return getLogicPartition(_ebr, _name, _file);
    }
    return {};
}

EBR getEBRprevious(int _seek, int _num, FILE *_file)
{
    EBR _ebr;
    fseek(_file, _seek, SEEK_SET);
    fread(&_ebr, sizeof(EBR), 1, _file);
    while (_ebr.part_next != -1)
    {
        if (_ebr.part_next == _num)
            return _ebr;
        else
        {
            fseek(_file, _ebr.part_next, SEEK_SET);
            fread(&_ebr, sizeof(EBR), 1, _file);
        }
    }
    return {};
}

partition getPartition(MBR _mbr, std::string _name, FILE *_file)
{
    for (int i = 0; i < 4; i++)
    {
        if (_mbr.mbr_partition[i].part_name == _name)
            return _mbr.mbr_partition[i];
    }
    return {};
}

int getPartitionIndex(MBR _mbr, std::string _name, FILE *_file)
{
    for (int i = 0; i < 4; i++)
    {
        if (_mbr.mbr_partition[i].part_name == _name)
            return i;
    }
    return -1;
}

int particionDisponible(MBR _mbr)
{
    for (int i = 0; i < 4; i++)
    {
        if (_mbr.mbr_partition[i].part_status == '0')
            return i;
    }
    return -1;
}

int existeExtendida(MBR _mbr)
{
    for (int i = 0; i < 4; i++)
    {
        if (_mbr.mbr_partition[i].part_type == 'E')
            return i;
    }
    return -1;
}

EBR getLastEBR(EBR _ebr, FILE *pFile)
{
    if (_ebr.part_next == -1)
        return _ebr;
    else
    {
        fseek(pFile, _ebr.part_next, SEEK_SET);
        fread(&_ebr, sizeof(EBR), 1, pFile);
        return getLastEBR(_ebr, pFile);
    }
    return {};
}

bool existeNombreMBR(MBR _mbr, std::string _name)
{
    for (int i = 0; i < 4; i++)
    {
        if (_mbr.mbr_partition[i].part_name == _name)
            return true;
    }
    return false;
}

bool existeNombreEBR(EBR _ebr, std::string _name, FILE *_file)
{
    fseek(_file, _ebr.part_start, SEEK_SET);
    if (_ebr.part_name == _name)
        return true;
    if (_ebr.part_next != -1)
    {
        fseek(_file, _ebr.part_next, SEEK_SET);
        fread(&_ebr, sizeof(EBR), 1, _file);
        return existeNombreEBR(_ebr, _name, _file);
    }
    return false;
}

int getPartitionsSize(MBR _mbr)
{
    int s = 0;
    for (int i = 0; i < 4; i++)
    {
        s += _mbr.mbr_partition[i].part_size;
    }
    return s;
}

int getLogicsSize(EBR _ebr, FILE *_file)
{
    fseek(_file, _ebr.part_start, SEEK_SET);
    // std::cout << _ebr.part_next << std::endl;
    int s = 0;
    s += _ebr.part_size;
    while (_ebr.part_next != -1)
    {
        fseek(_file, _ebr.part_next, SEEK_SET);
        fread(&_ebr, sizeof(EBR), 1, _file);
        s += _ebr.part_size;
    }
    return s;
}

// void print(std::string _message)
// {
//     std::cout << _message << std::endl;
// }

#endif