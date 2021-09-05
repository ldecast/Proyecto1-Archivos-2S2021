#include <iostream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../handler.h"
#include "func.h"

using std::string;
int UpdatePerm(FolderReference _fr, int _ugo, FILE *_file, int _start_inodes, int _start_blocks, bool _r, int _aux);
int RewriteInode(FolderReference _fr, FILE *_file, int _ugo, int _start_inodes, int _aux);

int CambiarPermisos(string _path, string _name, int _ugo, bool _r)
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
        string tmp = _path.substr(0, _path.find(folders[i]));
        fr = getFatherReference(fr, folders[i], file, super_bloque.s_inode_start, super_bloque.s_block_start);
        if (fr.inode == -1)
        {
            std::cout << "Not found: " + folders[i] + "\n";
            return coutError("Error: la ruta del archivo o carpeta no existe.", file);
        }
    }
    /* Lectura del inodo de carpeta padre */
    InodosTable inode_father;
    fseek(file, super_bloque.s_inode_start, SEEK_SET);
    fseek(file, fr.inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_father, sizeof(InodosTable), 1, file);
    CarpetasBlock file_block;
    bool x = false;
    for (int i = 0; i < 12 && !x; i++) // Obtener el inodo asociado directo
    {
        if (inode_father.i_block[i] != -1)
        {
            fseek(file, super_bloque.s_block_start, SEEK_SET);
            fseek(file, inode_father.i_block[i] * 64, SEEK_CUR);
            fread(&file_block, 64, 1, file);
            for (int j = 0; j < 4; j++)
            {
                if (string(file_block.b_content[j].b_name) == _name)
                {
                    fr.inode = file_block.b_content[j].b_inodo;
                    x = true;
                    break;
                }
            }
        }
    }
    if (!x)
        return coutError((_name.find('.') != std::string::npos) ? ("El archivo '") : ("La carpeta '") + _name + "' no se encuentra en la ruta: '/" + _path + "'.", file);

    int err = UpdatePerm(fr, _ugo, file, super_bloque.s_inode_start, super_bloque.s_block_start, _r, 0);
    fclose(file);
    file = NULL;
    if (err > 0)
    {
        std::cout << "\033[1;31mNo se pudieron actualizar los permisos de " + std::to_string(err) + " carpetas o archivos.\033[0m\n";
        return 0;
    }
    else if (err == -1)
        return 0;
    else
        return 1;
}

int UpdatePerm(FolderReference _fr, int _ugo, FILE *_file, int _start_inodes, int _start_blocks, bool _r, int _aux)
{
    if (RewriteInode(_fr, _file, _ugo, _start_inodes, _aux) > 0)
        return -1;
    // Actualizar lo que está dentro de la carpeta
    InodosTable inode_current;
    fseek(_file, _start_inodes, SEEK_SET);
    fseek(_file, _fr.inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_current, sizeof(InodosTable), 1, _file);
    if (_r && inode_current.i_type == '0')
    {
        CarpetasBlock file_block;
        for (int i = 0; i < 12; i++) // falta indirectos
        {
            if (inode_current.i_block[i] != -1)
            { /* Leer el bloque y redireccionar al inodo y ver si es el mismo UID */
                fseek(_file, _start_blocks, SEEK_SET);
                fseek(_file, inode_current.i_block[i] * 64, SEEK_CUR);
                fread(&file_block, 64, 1, _file);
                for (int j = 0; j < 4; j++)
                {
                    if (file_block.b_content[j].b_inodo != -1 && file_block.b_content[j].b_inodo != _fr.inode)
                    {
                        _fr.inode = file_block.b_content[j].b_inodo;
                        _aux = RewriteInode(_fr, _file, _ugo, _start_inodes, _aux);
                    }
                }
            }
        }
    }
    return _aux;
}

int RewriteInode(FolderReference _fr, FILE *_file, int _ugo, int _start_inodes, int _aux)
{
    /* Lectura del inodo de carpeta padre */
    InodosTable inode_current;
    fseek(_file, _start_inodes, SEEK_SET);
    fseek(_file, _fr.inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_current, sizeof(InodosTable), 1, _file);
    if (_user_logged.UID != inode_current.i_uid)
    {
        inode_current.i_atime = getCurrentTime();
        if (_aux == 0)
        {
            std::cout << "\033[1;31mEl usuario no es propietario del archivo o carpeta.\033[0m\n";
            return 1;
        }
        _aux++;
    }
    else
    {
        inode_current.i_perm = _ugo;
        inode_current.i_mtime = getCurrentTime();
    }
    fseek(_file, _start_inodes, SEEK_SET);
    fseek(_file, _fr.inode * sizeof(InodosTable), SEEK_CUR);
    fwrite(&inode_current, sizeof(InodosTable), 1, _file);
    return _aux;
}

int chmod(string _path, string _ugo, string _r)
{
    if (_path == "" || _ugo == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    if (!_user_logged.logged_in)
        return coutError("Error: No se encuentra ninguna sesión activa.", NULL);

    if (_ugo.length() != 3)
        return coutError("Error: parámetro 'ugo' no válido, deben ser tres números enteros.", NULL);

    for (int i = 0; i < 3; i++)
    {
        int bit = std::stoi(_ugo.substr(i, 1));
        if (bit < 0 || bit > 7)
            return coutError("El bit de permiso debe ser un número entre 0 y 7.", NULL);
    }

    string npath = _path.substr(0, _path.find_last_of('/'));
    string name = _path.substr(_path.find_last_of('/') + 1);
    return CambiarPermisos(npath, name, std::stoi(_ugo), _r != "");
}
