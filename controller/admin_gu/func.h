#ifndef FUNC_ADMIN_USERS_GROUPS
#define FUNC_ADMIN_USERS_GROUPS

#include <iostream>
#include <string.h>
#include "../../model/structures.h"
#include "../../model/users_groups.h"
#include "../../model/filesystem.h"
#include "../file_system/func.h"

// int freeBlockInode(InodosTable _inode)
// {
//     for (int i = 0; i < 12; i++)
//     {
//         if (_inode.i_block[i] == -1)
//             return i;
//     }
//     // buscar el inodo
//     FILE *file = fopen(_user_logged.mounted.path.c_str(), "rb");
//     Superbloque sb = getSuperBloque(_user_logged.mounted);
//     fseek(file, sb.s_block_start, SEEK_SET);
//     fseek(file, _inode.i_block[12], SEEK_SET);

//     /* APUNTADOR DE NIVEL 1 */
//     if (_inode.i_block[12] != -1)
//     {
//         fseek(file, _inode.i_block[12] * 64, SEEK_CUR);
//         ApuntadoresBlock nivel_1;
//         fread(&nivel_1, sizeof(ApuntadoresBlock), 1, file);
//     }

//     /* APUNTADOR DE NIVEL 2 */
//     if (_inode.i_block[13] != -1)
//     {
//         fseek(file, _inode.i_block[13] * 64, SEEK_CUR);
//         ApuntadoresBlock nivel_2;
//         fread(&nivel_2, sizeof(ApuntadoresBlock), 1, file);
//     }

//     /* APUNTADOR DE NIVEL 3 */
//     if (_inode.i_block[14] != -1)
//     {
//         fseek(file, _inode.i_block[14] * 64, SEEK_CUR);
//         ApuntadoresBlock nivel_3;
//         fread(&nivel_3, sizeof(ApuntadoresBlock), 1, file);
//     }

//     for (int i = 0; i < 16; i++)
//     {
//         if (_inode.i_block[12] != -1)
//         {
//             if ()
//         }
//     }

//     return -1;
// }

ArchivosBlock searchBlock(MOUNTED _mounted, int _index)
{
    Superbloque super_bloque = getSuperBloque(_mounted);
    FILE *file = fopen(_mounted.path.c_str(), "rb+");
    fseek(file, super_bloque.s_block_start, SEEK_SET);
    fseek(file, _index * sizeof(ArchivosBlock), SEEK_CUR);

    ArchivosBlock file_block;
    fread(&file_block, sizeof(ArchivosBlock), 1, file);

    // std::cout << file_block.b_content << std::endl;

    fclose(file);
    file = NULL;
    return file_block;
}

ArchivosBlock LastFileBlock(FILE *_file, InodosTable _inode, Superbloque _super_bloque)
{
    ArchivosBlock fblock;
    strcpy(fblock.b_content, "-");
    for (int i = 0; i < 15; i++)
    {
        std::cout << _inode.i_block[i] << std::endl;
        if (_inode.i_block[i] != -1)
        {
            fseek(_file, _super_bloque.s_block_start, SEEK_SET);
            fseek(_file, _inode.i_block[i] * 64, SEEK_CUR);
            ArchivosBlock file_block;
            fread(&file_block, sizeof(ArchivosBlock), 1, _file);
            std::cout << "¡¡¡¡¡¡¡¡¡" << file_block.b_content << "!!!!!!!!" << std::endl;
        }

        if (_inode.i_block[i] == -1)
        {
            fseek(_file, _super_bloque.s_block_start, SEEK_SET);
            fseek(_file, _inode.i_block[i - 1] * 64, SEEK_CUR);
            ArchivosBlock file_block;
            fread(&file_block, sizeof(ArchivosBlock), 1, _file);

            // fblock = searchBlock(_mounted, _inode.i_block[i - 1]);
            return file_block;
        }
    }
    return fblock;
}

int numberOfFileBlocks(int _size)
{
    float tmp = static_cast<float>(_size) / 64.0;
    return (int)ceil(tmp);
}

std::vector<std::string> Separate64Chars(std::string _content)
{
    std::vector<std::string> vector_s;
    while (_content.length() > 64)
    {
        std::string tmp = _content.substr(0, 64);
        vector_s.push_back(tmp);
        _content.erase(0, 64);
    }
    if (_content.length() > 0)
        vector_s.push_back(_content);
    return vector_s;
}

void writeBlocks(InodosTable _inode, std::string _content, int _number_inode)
{
    std::vector<std::string> chars = Separate64Chars(_content);
    _inode.i_size = sizeof(_content.c_str());
    for (int i = 0; i < chars.size(); i++) // Por cada iteración crear un bloque de contenido
    {
        int block_written = writeBlock(0, chars[i], -1);
        UpdateInode(_inode, _number_inode, block_written);
    }
}

#endif