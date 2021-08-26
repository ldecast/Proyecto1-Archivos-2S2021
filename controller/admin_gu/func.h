#ifndef FUNC_ADMIN_USERS_GROUPS
#define FUNC_ADMIN_USERS_GROUPS

#include <iostream>
#include <string.h>
#include "../../model/structures.h"
#include "../../model/users_groups.h"
#include "../../model/filesystem.h"

using std::string;

Users search_User(MOUNTED _mounted, Users _user_to_login)
{
    Users user_tmp;
    Superbloque super_bloque;
    FILE *file = fopen(_mounted.path.c_str(), "rb");

    int sb_start;
    switch (_mounted.type)
    {
    case 'P':
        sb_start = _mounted.particion.part_start;
        break;
    case 'L':
        sb_start = _mounted.logica.part_start;
        break;
    default:
        break;
    }

    fseek(file, sb_start, SEEK_SET);                    // Mover el puntero al inicio del superbloque
    fread(&super_bloque, sizeof(Superbloque), 1, file); // Leer el superbloque

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
    while (getline(f, line))
    {
        // std::cout << line << std::endl;
        int count = 0;
        for (int i = 0; (i = line.find(',', i)) != std::string::npos; i++)
            count++;
        switch (count)
        {
        case 4:
            user_tmp.UID = std::stoi(line.substr(0, line.find_first_of(',')));
            line = line.substr(line.find_first_of(',') + 1);

            user_tmp.tipo = line.substr(0, line.find_first_of(','))[0];
            line = line.substr(line.find_first_of(',') + 1);

            user_tmp.grupo = line.substr(0, line.find_first_of(','));
            line = line.substr(line.find_first_of(',') + 1);

            user_tmp.nombre = line.substr(0, line.find_first_of(','));
            line = line.substr(line.find_first_of(',') + 1);

            user_tmp.contrasena = line.substr(0, line.find_first_of('\n'));

            if (user_tmp.nombre == _user_to_login.nombre && user_tmp.contrasena == _user_to_login.contrasena)
            {
                user_tmp.logged_in = true;
                return user_tmp;
            }
            break;
        default:
            break;
        }
    }
    return user_tmp;
}

#endif