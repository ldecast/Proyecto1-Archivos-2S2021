#include <iostream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../handler.h"
#include "func.h"

using std::string;

int rmusr(string _name)
{
    if (_name == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    if (_user_logged.logged_in == false || _user_logged.nombre != "root")
        return coutError("Error: solamente se puede ejecutar el comando rmgrp con el usuario root.", NULL);

    if (_name == "root")
        return coutError("Error: No se puede eliminar el usuario root.", NULL);

    Users user_to_remove;
    user_to_remove.nombre = _name;
    Users user_tmp;

    FILE *file = fopen((_user_logged.mounted.path).c_str(), "rb");
    /* Lectura del superbloque */
    Superbloque super_bloque;
    fseek(file, startByteSuperBloque(_user_logged.mounted), SEEK_SET);
    fread(&super_bloque, sizeof(Superbloque), 1, file);

    /* Lectura del inodo de usuarios */
    InodosTable users_inode;
    fseek(file, super_bloque.s_inode_start, SEEK_SET); // Mover el puntero al inicio de la tabla de inodos
    fseek(file, sizeof(InodosTable), SEEK_CUR);        // Mover el puntero al segundo inodo que corresponde al archivo de users.txt
    fread(&users_inode, sizeof(InodosTable), 1, file); // Leer el inodo
    fclose(file);
    file = NULL;

    /* Obtener todo el archivo concatenado */
    string content_file = GetAllFile(users_inode, super_bloque.s_block_start, _user_logged.mounted.path);
    file = fopen((_user_logged.mounted.path).c_str(), "rb+");

    /* LEER LÍNEA POR LÍNEA EL ARCHIVO USERS.TXT */
    // std::cout << "\033[1;32m" + string(content_file) + "\033[0m\n";
    std::istringstream f(content_file);
    int uid = 1;
    string line, tmp = "";
    while (getline(f, line))
    {
        int count = 0;
        for (int i = 0; (i = line.find(',', i)) != std::string::npos; i++)
            count++;
        switch (count)
        {
        case 4:
            uid++;
            user_tmp.UID = std::stoi(line.substr(0, line.find_first_of(',')));
            line = line.substr(line.find_first_of(',') + 1);

            user_tmp.tipo = line.substr(0, line.find_first_of(','))[0];
            line = line.substr(line.find_first_of(',') + 1);

            user_tmp.grupo = line.substr(0, line.find_first_of(','));
            line = line.substr(line.find_first_of(',') + 1);

            user_tmp.nombre = line.substr(0, line.find_first_of(','));
            line = line.substr(line.find_first_of(',') + 1);

            user_tmp.contrasena = line.substr(0, line.find_first_of('\n'));
            if (user_tmp.nombre == user_to_remove.nombre && user_tmp.UID != 0)
            {
                tmp = std::to_string(user_tmp.UID) + ",U," + user_tmp.grupo + "," + user_tmp.nombre + "," + user_tmp.contrasena + "\n";
            }
            break;
        default:
            break;
        }
    }
    if (tmp == "")
        return coutError("Error: No existe ningún usuario activo con el nombre: '" + _name + "'.", file);

    content_file.replace(content_file.find(tmp), tmp.find_first_of(','), "0");

    users_inode.i_size = content_file.length();
    /* REESCRITURA */
    for (int i = 0; i < 15 && content_file.length() > 0; i++) //falta agregar indirectos
    {
        ArchivosBlock fblock;
        if (content_file.length() > 64)
        {
            strcpy(fblock.b_content, (content_file.substr(0, 64).c_str()));
            content_file = content_file.substr(64);
        }
        else
        {
            strcpy(fblock.b_content, content_file.c_str());
            content_file = "";
        }
        if (users_inode.i_block[i] != -1)
        {
            fseek(file, super_bloque.s_block_start, SEEK_SET);
            fseek(file, users_inode.i_block[i] * 64, SEEK_CUR);
            fwrite(&fblock, 64, 1, file);
        }
    }
    users_inode.i_mtime = getCurrentTime();
    fseek(file, super_bloque.s_inode_start, SEEK_SET);
    fseek(file, sizeof(InodosTable), SEEK_CUR);
    fwrite(&users_inode, sizeof(InodosTable), 1, file);

    fclose(file);
    file = NULL;
    return 1;
}