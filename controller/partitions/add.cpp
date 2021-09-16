#include <iostream>
#include <string.h>
#include "../../model/structures.h"
#include "../handler.h"
#include "../disks/func.h"

int AdministrarEspacio(int _add, char _unit, std::string _path, std::string _name) // revisarlo
{
    char src[_path.size() + 1];
    strcpy(src, _path.c_str());
    MBR mbr;
    FILE *pFile;
    pFile = fopen(src, "rb+");
    fseek(pFile, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, pFile);

    char type = existPartition(mbr, _name, pFile);
    if (type == '0')
        return coutError("No se encuentra ninguna partición con ese nombre asignado.", pFile);

    int size = getSize(_unit, _add);
    if (type == 'P' || type == 'E')
    {
        int i = getPartitionIndex(mbr, _name, pFile);
        partition _particion = mbr.mbr_partition[i];
        if (!Validations(mbr, i, _particion.part_start, size))
            return coutError("El parámetro '-add' debe dejar espacio positivo y no exceder los límites de la partición.", pFile);

        _particion.part_size = _particion.part_size + size;
        fseek(pFile, _particion.part_start, SEEK_SET);
        fwrite(&_particion, sizeof(partition), 1, pFile);
        // fwrite("\0", _particion.part_size, 1, pFile);
    }
    else if (type == 'L')
    {
        partition extendida = mbr.mbr_partition[existeExtendida(mbr)];
        EBR _ebr_to_update, _ebr_initial;
        fseek(pFile, extendida.part_start, SEEK_SET);
        fread(&_ebr_initial, sizeof(EBR), 1, pFile);
        _ebr_to_update = getLogicPartition(_ebr_initial, _name, pFile);
        
        if (_ebr_to_update.part_start + _ebr_to_update.part_size + size >= _ebr_to_update.part_next || _ebr_to_update.part_start + _ebr_to_update.part_size + size < _ebr_to_update.part_start)
            return coutError("El parámetro '-add' debe sobrar espacio positivo y no exceder los límites de la partición.", pFile);
        
        _ebr_to_update.part_size = _ebr_to_update.part_size + size;
        fseek(pFile, _ebr_to_update.part_start, SEEK_SET);
        fwrite(&_ebr_to_update, sizeof(EBR), 1, pFile);
        // fwrite("\0", _ebr_to_update.part_size, 1, pFile);
    }
    fclose(pFile);
    pFile = NULL;
    return 1;
}