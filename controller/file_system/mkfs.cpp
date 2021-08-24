#include <iostream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../handler.h"
#include "func.h"

using std::string;

int CrearSistemaArchivos(MOUNTED _mounted, char _type, int _fs)
{
    int part_size;
    int part_start;
    switch (_mounted.type)
    {
    case 'P':
        part_size = _mounted.particion.part_size;
        part_start = _mounted.particion.part_start;
        break;
    case 'L':
        part_size = _mounted.logica.part_size;
        part_start = _mounted.logica.part_start;
        break;
    default:
        return coutError("Solamente se puede montar un sistema de archivos en una partición primaria o lógica.", NULL);
    }

    int bm_start_inodes;
    switch (_fs)
    {
    case 2:
        bm_start_inodes = part_start + sizeof(Superbloque);
        break;
    case 3:
        bm_start_inodes = part_start + sizeof(Superbloque) + 100 * 64;
        break;
    default:
        return coutError("Error: parámetro -fs no válido: " + std::to_string(_fs), NULL);
    }

    /* CREACIÓN DEL SUPERBLOQUE */
    Superbloque super_bloque;
    InodosTable inodo;
    int n = _number_inodos(part_size);
    auto curr_time = std::chrono::system_clock::now();
    super_bloque.s_filesystem_type = _fs;
    super_bloque.s_inodes_count = n;
    super_bloque.s_blocks_count = 3 * n;
    super_bloque.s_free_inodes_count = super_bloque.s_inodes_count - 2;
    super_bloque.s_free_blocks_count = super_bloque.s_blocks_count - 2;
    super_bloque.s_mtime = std::chrono::system_clock::to_time_t(curr_time);
    super_bloque.s_mnt_count = 1;
    super_bloque.s_magic = 61267;
    super_bloque.s_inode_size = sizeof(InodosTable);
    super_bloque.s_block_size = 64;
    super_bloque.s_first_ino = 2;
    super_bloque.s_first_blo = 2;
    super_bloque.s_bm_inode_start = bm_start_inodes;
    super_bloque.s_bm_block_start = super_bloque.s_bm_inode_start + n;
    super_bloque.s_inode_start = super_bloque.s_bm_block_start + 3 * n;
    super_bloque.s_block_start = super_bloque.s_inode_start + n * sizeof(InodosTable);

    /* CREACIÓN DEL BITMAP DE INODOS */
    char bitmap_inodes[n];
    for (int i = 2; i < n; i++)
        bitmap_inodes[i] = '0';
    bitmap_inodes[0] = '1';
    bitmap_inodes[1] = '1';

    /* CREACIÓN DEL BITMAP DE BLOQUES */
    char bitmap_blocks[3 * n];
    for (int i = 2; i < 3 * n; i++)
        bitmap_blocks[i] = '0';
    bitmap_blocks[0] = '1';
    bitmap_blocks[1] = '1';

    /* CREACIÓN DE JOURNALING */
    Journaling journaling;

    /* ESCRITURA DEL SISTEMA EXT2 */
    FILE *_file = fopen(_mounted.path.c_str(), "rb+");
    fseek(_file, part_start, SEEK_SET); // Mover el puntero al inicio de la partición (primaria o lógica)

    fwrite(&super_bloque, sizeof(Superbloque), 1, _file); // 1. Superbloque

    if (super_bloque.s_filesystem_type == 3)
    { // fwrite("\0", 1, 1, _file);
        for (int i = 0; i < 32; i++)
            fwrite(&journaling, sizeof(Journaling), 1, _file); // 2. Journaling
    }

    fwrite(&bitmap_inodes, n, 1, _file); // 3. Bitmap de inodos

    fwrite(&bitmap_blocks, 3 * n, 1, _file); // 4. Bitmap de bloques

    for (int i = 0; i < n; i++)
        fwrite(&inodo, sizeof(InodosTable), 1, _file); // 5. Inodos

    for (int i = 0; i < 3 * n; i++)
        fwrite("\0", 64, 1, _file); // 6. Bloques (Hay distintos tipos de bloque, todos de 64 bytes)

    fclose(_file);
    _file = NULL;
    return 1;
}

int mkfs(string _id, string _type, string _fs)
{
    if (_id == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    Disk_id disk_id = buildID(_id);

    int index = existMountedID(disk_id);
    if (index == -1)
        exitFailure("No se encuentra ninguna partición montada con el id '" + _id + "'.");
    MOUNTED mounted = _particiones_montadas[index];

    char type = charFormat(_type);
    int fs = _2_or_3fs(_fs);

    return CrearSistemaArchivos(mounted, type, fs);
}
