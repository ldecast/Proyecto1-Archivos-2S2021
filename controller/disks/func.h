#ifndef FUNC
#define FUNC

#include <iostream>
#include <string.h>
#include "../../model/structures.h"

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

EBR getEBRprevious(EBR _ebr, int _n, FILE *_file) //revisar
{
    // std::cout << ".." + std::string(_ebr.part_name) << std::endl;
    if (_ebr.part_next == _n)
        return _ebr;
    if (_ebr.part_next != -1)
    {
        fseek(_file, _ebr.part_next, SEEK_SET);
        fread(&_ebr, sizeof(EBR), 1, _file);
        return getEBRprevious(_ebr, _n, _file);
    }
    else
        return _ebr;
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

int getLogicsSize(EBR _ebr, int _s, FILE *_file)
{
    // std::cout << "getLogicsSize -->_ebr.part_next: ";
    // std::cout << _ebr.part_next << std::endl;
    _s += _ebr.part_size;
    if (_ebr.part_next == -1)
        return _s;
    else
    {
        fseek(_file, _ebr.part_next, SEEK_SET);
        fread(&_ebr, sizeof(EBR), 1, _file);
        return getLogicsSize(_ebr, _s, _file);
    }
}

bool Validations(MBR _mbr, int _index, int _ini, int _size)
{
    if (_ini + _size < 0 || _ini + _size > _mbr.mbr_tamano || _ini + _size < _mbr.mbr_partition[_index].part_start)
        return false;
    if (_index < 3 && _mbr.mbr_partition[_index + 1].part_start > 0)
    {
        if (_ini + _size >= _mbr.mbr_partition[_index + 1].part_start)
            return false;
    }
    return true;
}

// void print(std::string _message)
// {
//     std::cout << _message << std::endl;
// }

#endif