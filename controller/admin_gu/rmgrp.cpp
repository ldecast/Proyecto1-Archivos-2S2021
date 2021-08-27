#include <iostream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../handler.h"
#include "func.h"

using std::string;

int rmgrp(string _name)
{
    if (_name == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    if (_user_logged.logged_in == false || _user_logged.nombre != "root")
        return coutError("Error: solamente se puede ejecutar el comando mkgrp con el usuario root.", NULL);

    Groups group_to_remove;
    group_to_remove.nombre = _name;

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
    // std::cout << users_file.b_content << std::endl;
    std::istringstream f(users_file.b_content);
    string line, tmp = "";
    int gid = 1;
    while (getline(f, line))
    {
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

            if (group_tmp.nombre == group_to_remove.nombre)
            {
                // if (group_tmp.GID == 0)
                //     std::cout << "\033[1;31mEl grupo: '" + _name + "' ya se encuentra eliminado.\033[0m\n";
                if (group_tmp.nombre == "root")
                    return coutError("Error: No se puede eliminar el grupo root.", file);
                tmp = std::to_string(group_tmp.GID) + "," + group_tmp.tipo + "," + group_tmp.nombre + "\n";
            }
            break;
        default:
            break;
        }
    }
    if (tmp == "")
        return coutError("Error: No existe ningún grupo con el nombre: '" + _name + "'.", file);

    string aux = string(users_file.b_content);
    aux.replace(aux.find(tmp), tmp.find_first_of(','), "0");
    strcpy(users_file.b_content, aux.c_str());
    users_inode.i_size = sizeof(aux.c_str());

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