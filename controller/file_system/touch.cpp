#include <iostream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../admin_gu/func.h"
#include "../handler.h"
#include "func.h"

using std::string;
int touch(string _path, string _r, string _size, string _cont, string _stdin);

int CrearArchivo(string _path, string _name, bool _r, int _size, string _cont, bool _stdin)
{
    // std::cout << _path << _name << std::endl;
    /* CREACIÓN DE UN ARCHIVO */
    // CarpetasBlock folder_to_alocate; // Bloque carpeta donde se alojará el archivo
    // Content file_content;            // Nuevo bloque de archivo
    InodosTable new_inode;        // Nuevo inodo
    ArchivosBlock file_to_create; // Nuevo bloque de contenido

    FILE *file = fopen((_user_logged.mounted.path).c_str(), "rb+");
    int start_byte_sb = startByteSuperBloque(_user_logged.mounted);
    /* Lectura del superbloque */
    Superbloque super_bloque;
    fseek(file, start_byte_sb, SEEK_SET);
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

    /* Lectura del archivo _cont si existe */
    string content = "";
    if (_cont != "")
    {
        std::ifstream file(_cont);
        string line;
        while (std::getline(file, line))
        {
            content += line;
        }
    }
    /* Llenado del contenido con dígitos 0-9 */
    else
    {
        int j = 0;
        for (size_t i = 0; content.length() < _size; i++)
        {
            if (j > 9)
                j = 0;
            content += std::to_string(j);
            j++;
        }
    }

    /* Llenar el nuevo inodo de archivo */
    new_inode.i_block[0] = free_block;
    new_inode.i_size = content.length();
    new_inode.i_type = '1';
    new_inode.i_gid = _user_logged.GID;
    new_inode.i_uid = _user_logged.UID;
    new_inode.i_perm = 664;
    new_inode.i_ctime = getCurrentTime();
    new_inode.i_mtime = new_inode.i_ctime;
    new_inode.i_atime = new_inode.i_ctime;

    /* Lectura de la última carpeta padre */
    FolderReference fr;
    std::vector<string> folders = SplitPath(_path);
    for (int i = 0; i < folders.size(); i++)
    {
        string tmp = _path.substr(0, _path.find(folders[i]));
        fr = getFatherReference(fr, folders[i], file, super_bloque.s_inode_start, super_bloque.s_block_start);
        if (fr.inode == -1)
        {
            std::cout << "Not found: " + folders[i] + "\n";
            if (!_r)
                return coutError("Error: la ruta no existe y no se ha indicado el comando -r.", file);
            fclose(file);
            file = NULL;
            for (int j = i; j < folders.size(); j++)
            {
                // std::cout << _path.substr(0, _path.find(folders[j])) + folders[j] << std::endl; //solo leer y luego esribir
                int r = touch(_path.substr(0, _path.find(folders[j])) + folders[j], "", std::to_string(_size), _cont, _stdin ? "t" : "");
                if (!r)
                    return coutError("Ha ocurrido un error", NULL);
            }
            return touch(_path + "/" + _name, "", std::to_string(_size), _cont, _stdin ? "t" : "");
        }
    }

    /* Lectura del inodo de carpeta padre */
    InodosTable inode_father;
    fseek(file, super_bloque.s_inode_start, SEEK_SET);
    fseek(file, fr.inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_father, sizeof(InodosTable), 1, file);
    if (!HasPermission(_user_logged, inode_father, 2))
        return coutError("El usuario no posee los permisos de escritura sobre la carpeta padre.", NULL);
    if (fileExists(inode_father, _name, file, super_bloque.s_block_start))
    {
        char ans;
        std::cout << "¿Desea sobreescribir el archivo ubicado en: '" + _path + "/" + _name + "?' [Y/n]" << std::endl;
        std::cin >> ans;
        if (ans == 'N' || ans == 'n')
        {
            fclose(file);
            file = NULL;
            return 1;
        }
    }
    /* Lectura del bloque de carpeta padre */
    CarpetasBlock folder_father;
    bool cupo = false;
    fseek(file, super_bloque.s_block_start, SEEK_SET);
    fseek(file, fr.block * 64, SEEK_CUR);
    fread(&folder_father, 64, 1, file);
    /* Escritura del nuevo bloque archivo que hace referencia al inodo carpeta */
    for (int i = 0; i < 4; i++)
    { // std::cout << folder_father.b_content[i].b_name << std::endl;
        if (folder_father.b_content[i].b_inodo == -1)
        {
            folder_father.b_content[i].b_inodo = free_inode;
            strcpy(folder_father.b_content[i].b_name, _name.c_str());
            fseek(file, super_bloque.s_block_start, SEEK_SET);
            fseek(file, fr.block * 64, SEEK_CUR);
            fwrite(&folder_father, 64, 1, file);
            cupo = true;
            break;
        }
    }
    if (!cupo) // usar el siguiente i_block disponible del inodo_father
    {
        for (int i = 0; i < 12; i++) //agregar indirectos
        {
            if (inode_father.i_block[i] != -1 && !cupo) // revisar si existe espacio disponible en un bloque carpeta
            {
                CarpetasBlock tmp_block;
                fseek(file, super_bloque.s_block_start, SEEK_SET);
                fseek(file, inode_father.i_block[i] * 64, SEEK_CUR);
                fread(&tmp_block, 64, 1, file);
                for (int j = 0; j < 4; j++)
                {
                    if (tmp_block.b_content[j].b_inodo == -1)
                    {
                        tmp_block.b_content[j].b_inodo = free_inode;
                        strcpy(tmp_block.b_content[j].b_name, _name.c_str());
                        fseek(file, super_bloque.s_block_start, SEEK_SET);
                        fseek(file, inode_father.i_block[i] * 64, SEEK_CUR);
                        fwrite(&tmp_block, 64, 1, file);
                        cupo = true;
                        inode_father.i_atime = getCurrentTime();
                        break;
                    }
                }
            }
            else if (inode_father.i_block[i] == -1 && !cupo) // crear un nuevo bloque carpeta
            {
                CarpetasBlock new_block;
                new_block.b_content[0].b_inodo = free_inode;
                strcpy(new_block.b_content[0].b_name, _name.c_str());
                fseek(file, super_bloque.s_block_start, SEEK_SET);
                fseek(file, free_block * 64, SEEK_CUR);
                fwrite(&new_block, 64, 1, file);

                inode_father.i_block[i] = free_block;
                inode_father.i_mtime = getCurrentTime();

                bm_blocks[free_block] = '1';
                super_bloque.s_first_blo = searchFreeIndex(bm_blocks, 3 * super_bloque.s_inodes_count);
                super_bloque.s_free_blocks_count--;
                free_block = super_bloque.s_first_blo;
                cupo = true;
                break;
            }
        }
    }
    fseek(file, super_bloque.s_inode_start, SEEK_SET);
    fseek(file, fr.inode * sizeof(InodosTable), SEEK_CUR);
    fwrite(&inode_father, sizeof(InodosTable), 1, file);

    if (!cupo)
        std::cout << "\033[1;31mNo se encontró espacio para crear el archivo.\033[0m\n";

    /* Llenar con la información del archivo */
    string extra = "-";
    if (content.length() > 64)
    {
        extra = content.substr(64);
        content = content.substr(0, 64);
    }
    strcpy(file_to_create.b_content, content.c_str());
    bm_inodes[free_inode] = '1';
    bm_blocks[free_block] = '1';

    super_bloque.s_first_ino = searchFreeIndex(bm_inodes, super_bloque.s_inodes_count);
    super_bloque.s_first_blo = searchFreeIndex(bm_blocks, 3 * super_bloque.s_inodes_count);
    super_bloque.s_free_inodes_count--;
    super_bloque.s_free_blocks_count--;

    /* ESCRITURA */
    fseek(file, start_byte_sb, SEEK_SET);
    fwrite(&super_bloque, sizeof(Superbloque), 1, file);

    fseek(file, super_bloque.s_bm_inode_start, SEEK_SET);
    fwrite(&bm_inodes, super_bloque.s_inodes_count, 1, file);

    fseek(file, super_bloque.s_bm_block_start, SEEK_SET);
    fwrite(&bm_blocks, 3 * super_bloque.s_inodes_count, 1, file);

    fseek(file, super_bloque.s_inode_start, SEEK_SET); // Mover el puntero al inicio de la tabla de inodos
    fseek(file, free_inode * sizeof(InodosTable), SEEK_CUR);
    fwrite(&new_inode, sizeof(InodosTable), 1, file);

    fseek(file, super_bloque.s_block_start, SEEK_SET); // Mover el puntero al inicio de la tabla de bloques
    fseek(file, free_block * 64, SEEK_CUR);
    fwrite(&file_to_create, 64, 1, file);

    fclose(file);
    file = NULL;
    if (extra != "-") // Si excede los 64 char, crear más bloques de contenido y asignarlo al inodo
    {
        writeBlocks(new_inode, extra, free_inode);
    }
    std::cout << "se creó: " + _path + "/" + _name + "\n";
    return 1;
}

int touch(string _path, string _r, string _size, string _cont, string _stdin)
{
    if (_path == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);
    if (!_user_logged.logged_in)
        return coutError("Error: No se encuentra ninguna sesión activa.", NULL);
    if (_cont != "")
    {
        if (!isDir(_cont))
            return coutError("Error: no se puede acceder a la ubicación: '" + _cont + "'", NULL);
    }
    string npath = _path.substr(0, _path.find_last_of('/'));
    string filename = _path.substr(_path.find_last_of('/') + 1);
    int nsize = (_size == "" || _cont != "") ? 0 : std::stoi(_size);
    return CrearArchivo(npath, filename, _r != "", nsize, _cont, _stdin != "");
}
