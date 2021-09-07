#include <iostream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../admin_gu/func.h"
#include "../handler.h"
#include "func.h"

using std::string;
int toRemove(int _index_inode, int _seek_superbloque);
int RewriteInode(int _index_inode, int _seek_superbloque);

int EliminarA_C(string _path, string _name)
{
    FILE *file = fopen((_user_logged.mounted.path).c_str(), "rb+");
    /* Lectura del superbloque */
    Superbloque super_bloque;
    int start_byte_sb = startByteSuperBloque(_user_logged.mounted);
    fseek(file, start_byte_sb, SEEK_SET);
    fread(&super_bloque, sizeof(Superbloque), 1, file);

    /* Lectura de la última carpeta padre */
    FolderReference fr;
    std::vector<string> folders = SplitPath(_path);
    for (int i = 0; i < folders.size(); i++)
    {
        fr = getFatherReference(fr, folders[i], file, super_bloque.s_inode_start, super_bloque.s_block_start);
        if (fr.inode == -1)
        {
            std::cout << "Not found: " + folders[i] + "\n";
            return coutError("Error: la ruta del archivo no se encuentra.", file);
        }
    }
    int _index_inode;
    /* Lectura del inodo de carpeta padre */
    InodosTable inode_father;
    fseek(file, super_bloque.s_inode_start, SEEK_SET);
    fseek(file, fr.inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_father, sizeof(InodosTable), 1, file);
    CarpetasBlock file_block;
    InodosTable inode_current_tmp;
    bool x = false;
    for (int i = 0; i < 12 && !x; i++) // Obtener el inodo asociado directo
    {
        if (inode_father.i_block[i] != -1)
        {
            fseek(file, super_bloque.s_block_start, SEEK_SET);
            fseek(file, inode_father.i_block[i] * 64, SEEK_CUR);
            fread(&file_block, 64, 1, file);
            for (int k = 0; k < 4; k++)
            {
                // std::cout << file_block.b_content[k].b_name << std::endl;
                if (string(file_block.b_content[k].b_name) == _name)
                {
                    // std::cout << file_block.b_content[k].b_inodo << std::endl;
                    _index_inode = file_block.b_content[k].b_inodo;
                    fseek(file, super_bloque.s_inode_start, SEEK_SET);
                    fseek(file, file_block.b_content[k].b_inodo * sizeof(InodosTable), SEEK_CUR);
                    fread(&inode_current_tmp, sizeof(InodosTable), 1, file);

                    if (!HasPermission(_user_logged, inode_current_tmp, 2))
                        return coutError("El usuario no posee el permiso de escritura del archivo o carpeta.", file);
                    x = true;

                    file_block.b_content[k].b_inodo = -1;
                    strcpy(file_block.b_content[k].b_name, "");
                    fseek(file, super_bloque.s_block_start, SEEK_SET);
                    fseek(file, inode_father.i_block[i] * 64, SEEK_CUR);
                    fwrite(&file_block, 64, 1, file);
                    break;
                }
            }
        }
    }
    if (!x)
        return coutError(((_name.find('.') != std::string::npos) ? ("El archivo '") : ("La carpeta '")) + _name + "' no se encuentra en la ruta: '/" + _path + "'.", file);

    fclose(file);
    file = NULL;
    /* Eliminar archivos y subcarpetas */
    return toRemove(_index_inode, start_byte_sb);
}

int toRemove(int _index_inode, int _seek_superbloque)
{
    FILE *_file = fopen((_user_logged.mounted.path).c_str(), "rb");
    /* Lectura del superbloque */
    Superbloque super_bloque;
    fseek(_file, _seek_superbloque, SEEK_SET);
    fread(&super_bloque, sizeof(Superbloque), 1, _file);
    // Actualizar lo que está dentro de la carpeta
    InodosTable inode_current;
    fseek(_file, super_bloque.s_inode_start, SEEK_SET);
    fseek(_file, _index_inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_current, sizeof(InodosTable), 1, _file);
    bool tmp = false;
    if (inode_current.i_type == '0')
    {
        CarpetasBlock file_block;
        for (int i = 0; i < 12; i++) // falta indirectos
        {
            if (inode_current.i_block[i] != -1)
            { /* Leer el bloque y redireccionar al inodo y ver si de nuevo es otra carpeta */
                fseek(_file, super_bloque.s_block_start, SEEK_SET);
                fseek(_file, inode_current.i_block[i] * 64, SEEK_CUR);
                fread(&file_block, 64, 1, _file);
                for (int j = 0; j < 4; j++)
                {
                    if (file_block.b_content[j].b_inodo != -1 && file_block.b_content[j].b_inodo != _index_inode && string(file_block.b_content[j].b_name) != ".." && string(file_block.b_content[j].b_name) != ".")
                    {
                        // std::cout << file_block.b_content[j].b_name << std::endl;
                        _index_inode = file_block.b_content[j].b_inodo;
                        fclose(_file);
                        if (!toRemove(_index_inode, _seek_superbloque))
                            return 0;
                    }
                }
            }
        }
    }
    else if (inode_current.i_type == '1')
    {
        fclose(_file);
    }
    return RewriteInode(_index_inode, _seek_superbloque);
}

int RewriteInode(int _index_inode, int _seek_superbloque)
{
    FILE *_file = fopen((_user_logged.mounted.path).c_str(), "rb+");
    /* Lectura del superbloque */
    Superbloque super_bloque;
    fseek(_file, _seek_superbloque, SEEK_SET);
    fread(&super_bloque, sizeof(Superbloque), 1, _file);

    int free_inode = super_bloque.s_first_ino;
    int free_block = super_bloque.s_first_blo;

    /* Lectura del bitmap de inodos */
    char bm_inodes[super_bloque.s_inodes_count];
    fseek(_file, super_bloque.s_bm_inode_start, SEEK_SET);
    fread(&bm_inodes, sizeof(super_bloque.s_inodes_count), 1, _file);

    /* Lectura del bitmap de bloques */
    char bm_blocks[3 * super_bloque.s_inodes_count];
    fseek(_file, super_bloque.s_bm_block_start, SEEK_SET);
    fread(&bm_blocks, sizeof(3 * super_bloque.s_inodes_count), 1, _file);

    /* Lectura del inodo de carpeta padre */
    InodosTable inode_current;
    fseek(_file, super_bloque.s_inode_start, SEEK_SET);
    fseek(_file, _index_inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_current, sizeof(InodosTable), 1, _file);
    if (!HasPermission(_user_logged, inode_current, 2))
    {
        inode_current.i_atime = getCurrentTime();
        fseek(_file, super_bloque.s_inode_start, SEEK_SET);
        fseek(_file, _index_inode * sizeof(InodosTable), SEEK_CUR);
        fwrite(&inode_current, sizeof(InodosTable), 1, _file);
        return coutError("El usuario no posee el permiso de escritura de todos los archivos o subcarpetas.", _file);
    }
    else
    {
        inode_current.i_size = 0;
        inode_current.i_mtime = getCurrentTime();
        for (int i = 11; i >= 0; i--) // falta indirectos
        {
            if (inode_current.i_block[i] != -1)
            {
                free_block = inode_current.i_block[i];
                bm_blocks[free_block] = '0';
                super_bloque.s_first_blo = free_block;
                super_bloque.s_blocks_count++;
                inode_current.i_block[i] = -1;
            }
        }
        free_inode = _index_inode;
        bm_inodes[free_inode] = '0';
        super_bloque.s_first_ino = free_inode;
        super_bloque.s_free_inodes_count++;
    }
    /* ESCRITURA */
    fseek(_file, _seek_superbloque, SEEK_SET);
    fwrite(&super_bloque, sizeof(Superbloque), 1, _file);

    fseek(_file, super_bloque.s_bm_inode_start, SEEK_SET);
    fwrite(&bm_inodes, super_bloque.s_inodes_count, 1, _file);

    fseek(_file, super_bloque.s_bm_block_start, SEEK_SET);
    fwrite(&bm_blocks, 3 * super_bloque.s_inodes_count, 1, _file);

    fseek(_file, super_bloque.s_inode_start, SEEK_SET);
    fseek(_file, _index_inode * sizeof(InodosTable), SEEK_CUR);
    fwrite(&inode_current, sizeof(InodosTable), 1, _file);

    fclose(_file);
    _file = NULL;
    return 1;
}

int rm(string _path)
{
    if (!_user_logged.logged_in)
        return coutError("Error: No se encuentra ninguna sesión activa.", NULL);
    string npath = _path.substr(0, _path.find_last_of('/'));
    string name = _path.substr(_path.find_last_of('/') + 1);
    return EliminarA_C(npath, name);
}
