#include <iostream>
#include <string.h>
#include "../../model/structures.h"
#include "../handler.h"
#include "../disks/func.h"

int BorrarParticion(char _delete, std::string _path, std::string _name)
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

    if (type == 'P' || type == 'E')
    {
        int i = getPartitionIndex(mbr, _name, pFile);
        mbr.mbr_partition[i].part_fit = mbr.disk_fit;
        strcpy(mbr.mbr_partition[i].part_name, "");
        // mbr.mbr_partition[i].part_size = 0;
        mbr.mbr_partition[i].part_status = '0';
        mbr.mbr_partition[i].part_type = 'P';
        fseek(pFile, 0, SEEK_SET);
        fwrite(&mbr, sizeof(MBR), 1, pFile);
        if (_delete == 'C')
        {
            fseek(pFile, mbr.mbr_partition[i].part_start + sizeof(partition), SEEK_SET);
            fwrite("\0", mbr.mbr_partition[i].part_size - sizeof(partition), 1, pFile);
        }
    }
    else if (type == 'L') //borrar referencia del EBR anterior
    {
        partition extendida = mbr.mbr_partition[existeExtendida(mbr)];
        EBR _ebr_to_delete, _ebr_initial, _ebr_previous;
        fseek(pFile, extendida.part_start, SEEK_SET);
        fread(&_ebr_initial, sizeof(EBR), 1, pFile);
        _ebr_to_delete = getLogicPartition(_ebr_initial, _name, pFile);
        // print(std::to_string(extendida.part_start));
        // print("?" + std::string(_ebr.part_name));
        _ebr_previous = getEBRprevious(_ebr_initial, _ebr_to_delete.part_start, pFile); // me devolvio el mismo???
        // y si es el inicial o no tiene...{}
        std::cout << "_ebr_previous.part_next: " + std::to_string(_ebr_previous.part_next) << std::endl; //pq 0?
        _ebr_previous.part_next = _ebr_to_delete.part_next;
        std::cout << "_ebr_previous.part_size: " + std::to_string(_ebr_previous.part_size) << std::endl;
        std::cout << "_ebr_previous.part_name: " + std::string(_ebr_previous.part_name) << std::endl;
        std::cout << "updated_ebr_previous.part_next: " + std::to_string(_ebr_previous.part_next) << std::endl;
        fseek(pFile, _ebr_to_delete.part_start, SEEK_SET);
        fwrite("\0", sizeof(EBR), 1, pFile);
        std::cout << "_ebr_to_delete.part_size: " + std::to_string(_ebr_to_delete.part_size) << std::endl;
        std::cout << "_ebr_to_delete.part_name: " + std::string(_ebr_to_delete.part_name) << std::endl;
        fseek(pFile, _ebr_previous.part_start, SEEK_SET);
        fwrite(&_ebr_previous, sizeof(EBR), 1, pFile);
        if (_delete == 'C')
        {
            fseek(pFile, _ebr_to_delete.part_start + sizeof(EBR), SEEK_SET);
            fwrite("\0", _ebr_to_delete.part_size - sizeof(EBR), 1, pFile);
        }
    }
    fclose(pFile);
    pFile = NULL;
    return 1;
}