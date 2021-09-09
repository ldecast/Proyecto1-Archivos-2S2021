#include <iostream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../admin_gu/func.h"
#include "../handler.h"
#include "func.h"

using std::string;
int UpdateUID(int _index_inode, int _start_inodes, int _start_blocks, int _uid, int _aux);
int RewriteUIDInode(int _index_inode, int _start_inodes, int _uid, int _aux);

int CambiarPropietario(string _path, string _name, string _username, bool _r)
{
    FILE *file = fopen((_user_logged.mounted.path).c_str(), "rb");

    /* Lectura del superbloque */
    Superbloque super_bloque;
    int start_byte_sb = startByteSuperBloque(_user_logged.mounted);
    fseek(file, start_byte_sb, SEEK_SET);
    fread(&super_bloque, sizeof(Superbloque), 1, file);

    int _uid = getUserByName(_username, 1, super_bloque.s_inode_start, super_bloque.s_block_start, _user_logged.mounted.path).UID;
    if (_uid == -1)
        return coutError("Error: no se encontró ningún usuario activo con el nombre: '" + _name + "'.", file);
    /* Lectura de la última carpeta padre */
    FolderReference fr;
    std::vector<string> folders = SplitPath(_path);
    for (int i = 0; i < folders.size(); i++)
    {
        fr = getFatherReference(fr, folders[i], file, super_bloque.s_inode_start, super_bloque.s_block_start);
        if (fr.inode == -1)
        {
            // std::cout << "Not found: " + folders[i] + "\n";
            return coutError("Error: la ruta del archivo o carpeta no existe.", file);
        }
    }
    /* Lectura del inodo de carpeta padre */
    InodosTable inode_father;
    fseek(file, super_bloque.s_inode_start, SEEK_SET);
    fseek(file, fr.inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_father, sizeof(InodosTable), 1, file);
    CarpetasBlock file_block;
    InodosTable inode_current_tmp;
    bool x = false;
    for (int i = 0; i < 15 && !x; i++) // Obtener el inodo asociado directo
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
                    fseek(file, super_bloque.s_inode_start, SEEK_SET);
                    fseek(file, file_block.b_content[j].b_inodo * sizeof(InodosTable), SEEK_CUR);
                    fread(&inode_current_tmp, sizeof(InodosTable), 1, file);
                    break;
                }
            }
        }
    }
    if (!x)
        return coutError(((_name.find('.') != std::string::npos) ? ("El archivo '") : ("La carpeta '")) + _name + "' no se encuentra en la ruta: '" + _path + "'.", file);
    if (!HasPermission(_user_logged, inode_current_tmp, 4))
        return coutError("El usuario no posee los permisos de lectura sobre la carpeta padre.", file);

    fclose(file);
    file = NULL;
    if (_r)
    {
        int err = UpdateUID(fr.inode, super_bloque.s_inode_start, super_bloque.s_block_start, _uid, 0);
        if (err > 0)
            std::cout << "\033[1;33mNo se pudo actualizar el propietario de " + std::to_string(err) + " subcarpetas y archivos.\033[0m\n";
        return 1;
    }
    return RewriteUIDInode(fr.inode, super_bloque.s_inode_start, _uid, -1);
}

int UpdateUID(int _index_inode, int _start_inodes, int _start_blocks, int _uid, int _aux)
{
    if (RewriteUIDInode(_index_inode, _start_inodes, _uid, _aux) > 0)
        _aux++;
    FILE *_file = fopen((_user_logged.mounted.path).c_str(), "rb");
    InodosTable inode_current;
    fseek(_file, _start_inodes, SEEK_SET);
    fseek(_file, _index_inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_current, sizeof(InodosTable), 1, _file);
    if (inode_current.i_type == '0')
    { // Actualizar lo que está dentro de la carpeta
        CarpetasBlock file_block;
        for (int i = 0; i < 15; i++) // falta indirectos
        {
            if (inode_current.i_block[i] != -1)
            { /* Leer el bloque y redireccionar al inodo y ver si de nuevo es otra carpeta */
                fseek(_file, _start_blocks, SEEK_SET);
                fseek(_file, inode_current.i_block[i] * 64, SEEK_CUR);
                fread(&file_block, 64, 1, _file);
                for (int j = 0; j < 4; j++)
                {
                    if (file_block.b_content[j].b_inodo != -1 && file_block.b_content[j].b_inodo != _index_inode && string(file_block.b_content[j].b_name) != ".." && string(file_block.b_content[j].b_name) != ".")
                    {
                        // std::cout << file_block.b_content[j].b_name << std::endl;
                        _index_inode = file_block.b_content[j].b_inodo;
                        fclose(_file);
                        _aux = UpdateUID(_index_inode, _start_inodes, _start_blocks, _uid, _aux);
                    }
                }
            }
        }
    }
    else if (inode_current.i_type == '1')
    {
        fclose(_file);
        RewriteUIDInode(_index_inode, _start_inodes, _uid, _aux);
    }
    return _aux;
}

int RewriteUIDInode(int _index_inode, int _start_inodes, int _uid, int _aux)
{
    FILE *_file = fopen((_user_logged.mounted.path).c_str(), "rb+");
    /* Lectura del inodo a cambiar permisos */
    InodosTable inode_current;
    fseek(_file, _start_inodes, SEEK_SET);
    fseek(_file, _index_inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_current, sizeof(InodosTable), 1, _file);
    if (_user_logged.UID == inode_current.i_uid || _user_logged.nombre == "root")
    {
        inode_current.i_uid = _uid;
        inode_current.i_mtime = getCurrentTime();
    }
    else
    {
        inode_current.i_atime = getCurrentTime();
        _aux++;
    }
    fseek(_file, _start_inodes, SEEK_SET);
    fseek(_file, _index_inode * sizeof(InodosTable), SEEK_CUR);
    fwrite(&inode_current, sizeof(InodosTable), 1, _file);
    fclose(_file);
    _file = NULL;
    return _aux;
}

int chown(string _path, string _usr, string _r)
{
    if (_path == "" || _usr == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    if (!_user_logged.logged_in)
        return coutError("Error: No se encuentra ninguna sesión activa.", NULL);

    string npath = _path.substr(0, _path.find_last_of('/'));
    string name = _path.substr(_path.find_last_of('/') + 1);
    return CambiarPropietario(npath, name, _usr, _r != "");
}
