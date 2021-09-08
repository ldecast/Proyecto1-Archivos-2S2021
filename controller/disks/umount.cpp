#include <iostream>
#include <string.h>
#include "../../model/structures.h"
#include "../../model/filesystem.h"
#include "../handler.h"
#include "../partitions/func.h"
#include "func.h"

using std::string;

int DesmontarParticion(Disk_id _disk_id);

int umount(string _id)
{
    if (_id == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);
    Disk_id _disk_id = buildID(_id);
    return DesmontarParticion(_disk_id);
}

int DesmontarParticion(Disk_id _disk_id)
{
    MOUNTED mounted;
    for (int i = _particiones_montadas.size() - 1; i >= 0; i--)
    {
        mounted = _particiones_montadas[i];
        if (mounted.id._carnet == _disk_id._carnet && mounted.id._number_id == _disk_id._number_id && mounted.id._letter_id == _disk_id._letter_id)
        {
            _particiones_montadas.erase(_particiones_montadas.begin() + i);

            if (mounted.type != 'E')
            {
                int _part_start = (mounted.type == 'L') ? (mounted.logica.part_start) : (mounted.particion.part_start);
                FILE *file = fopen(mounted.path.c_str(), "rb+");
                Superbloque sb;
                fseek(file, _part_start, SEEK_SET);
                fread(&sb, sizeof(Superbloque), 1, file);
                if (sb.s_mnt_count > 0 && mounted.type != 'E')
                {
                    sb.s_umtime = getCurrentTime();
                    fseek(file, _part_start, SEEK_SET);
                    fwrite(&sb, sizeof(Superbloque), 1, file);
                    // std::cout << ctime(&sb.s_umtime) << std::endl;
                    // std::cout << ctime(&sb.s_mtime) << std::endl;
                }
                fclose(file);
                file = NULL;
            }

            // Imprimir montadas
            /* for (int j = 0; j < _particiones_montadas.size(); j++)
            {
                MOUNTED _mounted = _particiones_montadas[j];
                string tmp = _mounted.id._carnet + std::to_string(_mounted.id._number_id) + _mounted.id._letter_id;
                string name = (_mounted.type == 'L') ? _mounted.logica.part_name : _mounted.particion.part_name;
                std::cout << _mounted.path + "|" + name + "|" + tmp << std::endl;
            } */
            return 1;
        }
    }
    return coutError("No se encuentra ninguna partición montada con el id: '" + _disk_id._carnet + std::to_string(_disk_id._number_id) + _disk_id._letter_id + "'.", NULL);
}
