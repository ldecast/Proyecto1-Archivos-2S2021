#include <iostream>
#include <string.h>
#include "../partitions/func.h"
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../handler.h"
#include "func.h"

using std::string;

int mkgrp(string _name)
{
    if (_name == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    if (_user_logged.logged_in == false || _user_logged.nombre != "root")
        return coutError("Error: solamente se puede ejecutar el comando mkgrp con el usuario root.", NULL);

    Groups group_to_create;
    group_to_create.nombre = _name;

    Groups group_tmp;
    Superbloque super_bloque = getSuperBloque(_user_logged.mounted);
    FILE *file = fopen(_user_logged.mounted.path.c_str(), "rb+");

    InodosTable users_inode;
    fseek(file, super_bloque.s_inode_start, SEEK_SET); // Mover el puntero al inicio de la tabla de inodos
    fseek(file, sizeof(InodosTable), SEEK_CUR);        // Mover el puntero al segundo inodo que corresponde al archivo de users.txt
    fread(&users_inode, sizeof(InodosTable), 1, file); // Leer el inodo

    ArchivosBlock users_file;
    fseek(file, super_bloque.s_block_start, SEEK_SET);  // Mover el puntero al inicio de la tabla de bloques
    fseek(file, 64, SEEK_CUR);                          // Mover el puntero al segundo bloque que corresponde al archivo de users.txt
    fread(&users_file, sizeof(ArchivosBlock), 1, file); // Leer el bloque

    /* LEER LÍNEA POR LÍNEA EL ARCHIVO USERS.TXT */
    std::istringstream f(users_file.b_content);
    string line;
    int gid = 1;
    while (getline(f, line))
    {
        std::cout << line << std::endl;
        int count = 0;
        for (int i = 0; (i = line.find(',', i)) != std::string::npos; i++)
            count++;
        switch (count)
        {
        case 2:
            gid++;
            group_tmp.GID = std::stoi(line.substr(0, line.find_first_of(',')));
            line = line.substr(line.find_first_of(',') + 1);

            group_tmp.tipo = line.substr(0, line.find_first_of(','))[0];
            line = line.substr(line.find_first_of(',') + 1);

            group_tmp.nombre = line.substr(0, line.find_first_of('\n'));

            if (group_tmp.nombre == group_to_create.nombre)
                return coutError("Error: El nombre del grupo ya existe.", NULL);
            break;
        default:
            break;
        }
    }
    string tmp = users_file.b_content + std::to_string(gid) + ",G," + group_to_create.nombre + "\n";
    strcpy(users_file.b_content, tmp.c_str());
    users_inode.i_size = sizeof(tmp.c_str());

    /* ESCRITURA */
    fseek(file, super_bloque.s_inode_start, SEEK_SET);
    fseek(file, sizeof(InodosTable), SEEK_CUR);
    fwrite(&users_inode, sizeof(InodosTable), 1, file);

    fseek(file, super_bloque.s_block_start, SEEK_SET);
    fseek(file, sizeof(CarpetasBlock), SEEK_CUR);
    fwrite(&users_file, sizeof(ArchivosBlock), 1, file);

    fclose(file);
    file = NULL;
    return 1;
}