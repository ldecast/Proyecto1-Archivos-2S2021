#include <iostream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../handler.h"
#include "func.h"

using std::string;

int RecuperarSistema(MOUNTED _mounted)
{
    int part_start;
    switch (_mounted.type)
    {
    case 'P':
        part_start = _mounted.particion.part_start;
        break;
    case 'L':
        part_start = _mounted.logica.part_start + sizeof(EBR);
        break;
    default:
        return coutError("Solamente se puede recuperar un sistema de archivos en una partición primaria o lógica.", NULL);
    }

    FILE *file = fopen(_mounted.path.c_str(), "rb+");
    Superbloque sb;
    fseek(file, part_start, SEEK_SET);
    fread(&sb, sizeof(Superbloque), 1, file);
    if (sb.s_mnt_count > 0)
    {
        if (sb.s_filesystem_type == 2)
            return coutError("Solamente se puede hacer uso de Recovery en un sistema de archivos EXT3.", file);
        sb.s_magic = 61267;
        fseek(file, part_start, SEEK_SET);
        fwrite(&sb, sizeof(Superbloque), 1, file);
    }
    else
    {
        return coutError("No se encuentra un sistema de archivos existente.", file);
    }

    fclose(file);
    file = NULL;
    return 1;
}

int recovery(string _id)
{
    if (_id == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    Disk_id disk_id = buildID(_id);

    int index = existMountedID(disk_id);
    if (index == -1)
        return coutError("No se encuentra ninguna partición montada con el id '" + _id + "'.", NULL);
    MOUNTED mounted = _particiones_montadas[index];

    return RecuperarSistema(mounted);
}
