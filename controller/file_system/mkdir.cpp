#include <iostream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../handler.h"
#include "func.h"

using std::string;

struct FolderReference
{
    // InodosTable inode;
    int inode;
    int block;
};

FolderReference getFather(FolderReference _fr, string _folder, FILE *_file, int _start_inodes, int _start_blocks);

int CrearCarpeta(string _path, string _name, bool _p)
{
    /* CREACIÓN DE CARPETA */
    CarpetasBlock folder_to_create; // Nuevo bloque carpeta
    Content folder_content;         // Nuevo bloque contenido
    InodosTable new_inode;          // Nuevo inodo

    FILE *file = fopen((_user_logged.mounted.path).c_str(), "rb+");

    /* Lectura del superbloque */
    Superbloque super_bloque;
    fseek(file, startByteSuperBloque(), SEEK_SET);
    fread(&super_bloque, sizeof(Superbloque), 1, file);

    int free_inode = super_bloque.s_first_ino;
    int free_block = super_bloque.s_first_blo;

    /* Lectura del bitmap de inodos */
    char bm_inodes[super_bloque.s_inodes_count];
    fseek(file, super_bloque.s_bm_inode_start, SEEK_SET);
    fread(&bm_inodes, sizeof(super_bloque.s_inodes_count), 1, file);

    /* Lectura del bitmap de bloques */
    char bm_blocks[3 * super_bloque.s_inodes_count];
    fseek(file, super_bloque.s_bm_block_start, SEEK_SET);
    fread(&bm_blocks, sizeof(3 * super_bloque.s_inodes_count), 1, file);

    /* Lectura del inodo de carpeta raíz */
    InodosTable root_inode;
    fseek(file, super_bloque.s_inode_start, SEEK_SET);
    fread(&root_inode, sizeof(InodosTable), 1, file);
    CarpetasBlock root_folder;
    fseek(file, super_bloque.s_block_start, SEEK_SET);
    fread(&root_folder, 64, 1, file);

    /* Llenar el nuevo inodo carpeta */
    new_inode.i_block[0] = free_block;
    new_inode.i_size = 0;
    new_inode.i_type = '0';
    new_inode.i_gid = _user_logged.GID;
    new_inode.i_uid = _user_logged.UID;
    new_inode.i_ctime = getCurrentTime();
    new_inode.i_mtime = new_inode.i_ctime;
    new_inode.i_atime = new_inode.i_ctime;
    // int free_i_block = GetFreeI_Block(root_inode); // if == -1

    /* Lectura de la última carpeta padre */
    FolderReference fr;
    std::vector<string> folders = SplitPath(_path);
    // if (folders.size() == 0)
    // {
    fr.inode = 0;
    fr.block = 0;
    // }
    if (folders.size() > 0)
        for (int i = 0; i < folders.size(); i++)
        {
            fr = getFather(fr, folders[i], file, super_bloque.s_inode_start, super_bloque.s_block_start);
            if (fr.inode == -1)
            {
                if (!_p)
                    return coutError("Error: la ruta no existe y no se ha indicado el comando -p.", file);
                // else: Crear la carpeta
            }
        }
    /* Lectura del bloque de carpeta padre */
    CarpetasBlock folder_father;
    bool cupo = false;
    fseek(file, super_bloque.s_block_start, SEEK_SET);
    fseek(file, fr.block * 64, SEEK_CUR);
    fread(&folder_father, 64, 1, file);
    for (int i = 0; i < 4; i++)
    {
        // std::cout << folder_father.b_content[i].b_name << std::endl;
        if (folder_father.b_content[i].b_inodo == -1)
        {
            folder_father.b_content[i].b_inodo = free_inode;
            strcpy(folder_father.b_content[i].b_name, _name.c_str());
            fseek(file, super_bloque.s_block_start, SEEK_SET);
            fseek(file, fr.block * 64, SEEK_CUR);
            fwrite(&folder_father, 64, 1, file);
            cupo = true;
        }
    }
    if (!cupo)
    {
        std::cout << "no cupo\n";
        // usar el siguiente i_block disponible del inodo_father
    }

    /* Llenar con la información de la carpeta */
    folder_content.b_inodo = free_inode;
    strcpy(folder_content.b_name, ".");
    folder_to_create.b_content[0] = folder_content;

    folder_content.b_inodo = folder_father.b_content[0].b_inodo;
    strcpy(folder_content.b_name, "..");
    folder_to_create.b_content[1] = folder_content;

    bm_inodes[free_inode] = '1'; //
    bm_blocks[free_block] = '1';

    super_bloque.s_first_ino = searchFreeIndex(bm_inodes, super_bloque.s_inodes_count);
    super_bloque.s_first_blo = searchFreeIndex(bm_blocks, 3 * super_bloque.s_inodes_count);
    super_bloque.s_free_inodes_count--;
    super_bloque.s_free_blocks_count--;

    /* ESCRITURA */
    fseek(file, startByteSuperBloque(), SEEK_SET);
    fwrite(&super_bloque, sizeof(Superbloque), 1, file);

    fseek(file, super_bloque.s_bm_inode_start, SEEK_SET);
    fwrite(&bm_inodes, super_bloque.s_inodes_count, 1, file);

    fseek(file, super_bloque.s_bm_block_start, SEEK_SET);
    fwrite(&bm_blocks, 3 * super_bloque.s_inodes_count, 1, file);

    fseek(file, super_bloque.s_inode_start, SEEK_SET); // Mover el puntero al inicio de la tabla de inodos
    fseek(file, free_inode * sizeof(InodosTable), SEEK_CUR);
    fwrite(&new_inode, sizeof(InodosTable), 1, file);

    /* fseek(file, super_bloque.s_block_start, SEEK_SET);
    fseek(file, byte_father * 64, SEEK_CUR);
    fwrite(&father, 64, 1, file); */

    fseek(file, super_bloque.s_block_start, SEEK_SET); // Mover el puntero al inicio de la tabla de bloques
    fseek(file, free_block * 64, SEEK_CUR);
    fwrite(&folder_to_create, 64, 1, file);

    fclose(file);
    file = NULL;
    return 1;
}

int mkdir(string _path, string _p)
{
    if (_path == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);
    if (!_user_logged.logged_in)
        return coutError("Error: No se encuentra ninguna sesión activa.", NULL);

    string npath = _path.substr(0, _path.find_last_of('/'));
    string name_folder = _path.substr(_path.find_last_of('/') + 1);

    return CrearCarpeta(npath, name_folder, _p != "");
}

FolderReference getFather(FolderReference _fr, string _folder, FILE *_file, int _start_inodes, int _start_blocks)
{ // Retorna el inodo carpeta de la carpeta padre
    InodosTable inode;
    // fseek(_file, _start_inodes, SEEK_SET);
    // fseek(_file, _fr.inode * sizeof(InodosTable), SEEK_CUR);
    // fread(&inode, sizeof(InodosTable), 1, _file);
    CarpetasBlock folder_block;
    fseek(_file, _start_blocks, SEEK_SET);
    fseek(_file, _fr.block * 64, SEEK_CUR);
    fread(&folder_block, 64, 1, _file);

    for (int i = 2; i < 4; i++)
    { // std::cout << folder_block.b_content[k].b_name << std::endl;
        if (string(folder_block.b_content[i].b_name) == _folder)
        {
            _fr.inode = folder_block.b_content[i].b_inodo;
            fseek(_file, _start_inodes, SEEK_SET);
            fseek(_file, folder_block.b_content[i].b_inodo * sizeof(InodosTable), SEEK_CUR);
            fread(&inode, sizeof(InodosTable), 1, _file);
            for (int i = 0; i < 12; i++)
            {
                if (inode.i_block[i] != -1)
                {
                    fseek(_file, _start_blocks, SEEK_SET);
                    fseek(_file, inode.i_block[i] * 64, SEEK_CUR);
                    fread(&folder_block, 64, 1, _file);
                    if (folder_block.b_content[0].b_inodo == _fr.inode)
                    {
                        _fr.block = inode.i_block[i];
                        return _fr;
                    }
                }
            }
            // std::cout << folder_block.b_content[i].b_name << std::endl;
            // _fr.block = folder_block.b_content[i].b_inodo;
            // return getFather(_fr, _folder, _file, _start_inodes, _start_blocks);
        }
    }

    _fr.inode = -1;
    return _fr;
}

// int
