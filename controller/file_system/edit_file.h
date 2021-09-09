#ifndef FUNC_EDIT_FILE
#define FUNC_EDIT_FILE

#include <iostream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../admin_gu/func.h"
#include "../handler.h"
#include "func.h"

using std::string;

int EditarArchivo(string _path, string _name, string _content, bool _stdin)
{
    /* EDICIÓN DE UN ARCHIVO */
    InodosTable tmp_inode;  // Nuevo inodo
    ArchivosBlock tmp_file; // Nuevo bloque de contenido

    FILE *file = fopen((_user_logged.mounted.path).c_str(), "rb+");
    int start_byte_sb = startByteSuperBloque(_user_logged.mounted);

    /* Lectura del superbloque */
    Superbloque super_bloque;
    fseek(file, start_byte_sb, SEEK_SET);
    fread(&super_bloque, sizeof(Superbloque), 1, file);

    int free_block = super_bloque.s_first_blo;

    /* Lectura del bitmap de bloques */
    char bm_blocks[3 * super_bloque.s_inodes_count];
    fseek(file, super_bloque.s_bm_block_start, SEEK_SET);
    fread(&bm_blocks, 3 * super_bloque.s_inodes_count, 1, file);

    /* Lectura del inodo de carpeta raíz */
    InodosTable root_inode;
    fseek(file, super_bloque.s_inode_start, SEEK_SET);
    fread(&root_inode, sizeof(InodosTable), 1, file);

    // std::cout << _content << std::endl;

    /* Lectura de la última carpeta padre */
    FolderReference fr;
    std::vector<string> folders = SplitPath(_path);
    for (int i = 0; i < folders.size(); i++)
    {
        fr = getFatherReference(fr, folders[i], file, super_bloque.s_inode_start, super_bloque.s_block_start);
        if (fr.inode == -1)
        {
            // std::cout << "Not found: " + folders[i] + "\n";
            return coutError("Error: la ruta no existe y no se ha indicado el comando -r.", file);
        }
    }

    /* Lectura del inodo de carpeta padre */
    InodosTable inode_father;
    fseek(file, super_bloque.s_inode_start, SEEK_SET);
    fseek(file, fr.inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_father, sizeof(InodosTable), 1, file);
    if (!HasPermission(_user_logged, inode_father, 6))
        return coutError("El usuario no posee los permisos de lectura y escritura sobre la carpeta padre.", file);
    if (!fileExists(inode_father, _name, file, super_bloque.s_block_start))
        return coutError("El archivo '" + _name + "' no se encuentra en la ruta: " + _path + ".", file);

    /* Lectura del bloque de carpeta padre */
    CarpetasBlock file_block_tmp;
    int _index_to_edit_inode = -1;
    bool x = false;
    for (int i = 0; i < 15 && !x; i++) // falta indirectos
    {
        if (inode_father.i_block[i] != -1)
        {
            fseek(file, super_bloque.s_block_start, SEEK_SET);
            fseek(file, inode_father.i_block[i] * 64, SEEK_CUR);
            fread(&file_block_tmp, 64, 1, file);
            for (int j = 0; j < 4; j++)
            {
                if (string(file_block_tmp.b_content[j].b_name) == _name)
                {
                    _index_to_edit_inode = file_block_tmp.b_content[j].b_inodo;
                    x = true;
                    break;
                }
            }
        }
    }
    if (!x)
        return coutError("No se halló el inodo del archivo a editar.", file);

    InodosTable inode_current; // Leer el inodo de archivo que sólo posee bloques de contenido
    fseek(file, super_bloque.s_inode_start, SEEK_SET);
    fseek(file, _index_to_edit_inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_current, sizeof(InodosTable), 1, file);

    inode_current.i_size = _content.length();
    inode_current.i_mtime = getCurrentTime();
    /* Llenar con la información del archivo */
    ArchivosBlock tmp_content_block;
    string tmp = "";
    for (int i = 0; i < 15; i++) //falta indirectos
    {
        if (_content.length() > 64)
        {
            tmp = _content.substr(0, 64);
            _content = _content.substr(64);
        }
        else
        {
            tmp = _content;
            _content = "";
        }
        if (inode_current.i_block[i] != -1 && tmp.length() > 0) /* Reutiliza el mismo bloque */
        {
            strcpy(tmp_content_block.b_content, tmp.c_str());
            fseek(file, super_bloque.s_block_start, SEEK_SET);
            fseek(file, inode_current.i_block[i] * 64, SEEK_CUR);
            fwrite(&tmp_content_block, 64, 1, file);
        }
        else if (inode_current.i_block[i] == -1 && tmp.length() > 0) /* Crea otro bloque en caso exceda el tamaño anterior */
        {
            strcpy(tmp_content_block.b_content, tmp.c_str());
            fseek(file, super_bloque.s_block_start, SEEK_SET);
            fseek(file, free_block * 64, SEEK_CUR);
            fwrite(&tmp_content_block, 64, 1, file);
            inode_current.i_block[i] = free_block;
            bm_blocks[free_block] == '1';
            free_block++;
            super_bloque.s_first_blo = free_block;
            super_bloque.s_free_blocks_count--;
        }
        else if (inode_current.i_block[i] != -1 && tmp.length() == 0) /* El archivo era más grande y se liberan los bloques */
        {
            strcpy(tmp_content_block.b_content, "");
            fseek(file, super_bloque.s_block_start, SEEK_SET);
            fseek(file, inode_current.i_block[i] * 64, SEEK_CUR);
            fwrite(&tmp_content_block, 64, 1, file);
            bm_blocks[inode_current.i_block[i]] = '0';
            // free_block = inode_current.i_block[i];
            // super_bloque.s_first_blo = free_block;
            super_bloque.s_free_blocks_count++;
            inode_current.i_block[i] = -1;
        }
    }

    /* ESCRITURA */
    fseek(file, start_byte_sb, SEEK_SET);
    fwrite(&super_bloque, sizeof(Superbloque), 1, file);

    fseek(file, super_bloque.s_bm_block_start, SEEK_SET);
    fwrite(&bm_blocks, 3 * super_bloque.s_inodes_count, 1, file);

    fseek(file, super_bloque.s_inode_start, SEEK_SET); // Mover el puntero al inicio de la tabla de inodos
    fseek(file, _index_to_edit_inode * sizeof(InodosTable), SEEK_CUR);
    fwrite(&inode_current, sizeof(InodosTable), 1, file);

    // coutError("Se editó el archivo en el bloque: " + std::to_string(free_block) + " En el inodo: " + std::to_string(free_inode), NULL);
    fclose(file);
    file = NULL;
    // std::cout << "Se editó el archivo: " + _path + "/" + _name + "\n";
    return 1;
}

#endif