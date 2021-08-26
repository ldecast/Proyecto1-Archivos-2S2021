#ifndef FUNC_ADMIN_USERS_GROUPS
#define FUNC_ADMIN_USERS_GROUPS

#include <iostream>
#include <string.h>
#include "../../model/structures.h"
#include "../../model/users_groups.h"
#include "../../model/filesystem.h"
#include "../file_system/func.h"

using std::string;

// Users search_User(MOUNTED _mounted, Users _user_to_login)
// {
//     Users user_tmp;
//     Superbloque super_bloque = getSuperBloque(_mounted);
//     FILE *file = fopen(_mounted.path.c_str(), "rb+");

//     InodosTable users_inode;
//     fseek(file, super_bloque.s_inode_start, SEEK_SET); // Mover el puntero al inicio de la tabla de inodos
//     fseek(file, sizeof(InodosTable), SEEK_CUR);        // Mover el puntero al segundo inodo que corresponde al archivo de users.txt
//     fread(&users_inode, sizeof(InodosTable), 1, file); // Leer el inodo

//     ArchivosBlock users_file;
//     fseek(file, super_bloque.s_block_start, SEEK_SET);  // Mover el puntero al inicio de la tabla de bloques
//     fseek(file, 64, SEEK_CUR);                          // Mover el puntero al segundo bloque que corresponde al archivo de users.txt
//     fread(&users_file, sizeof(ArchivosBlock), 1, file); // Leer el bloque

//     /* LEER LÍNEA POR LÍNEA EL ARCHIVO USERS.TXT */
//     std::istringstream f(users_file.b_content);
//     string line;
//     while (getline(f, line))
//     {
//         // std::cout << line << std::endl;
//         int count = 0;
//         for (int i = 0; (i = line.find(',', i)) != std::string::npos; i++)
//             count++;
//         switch (count)
//         {
//         case 4:
//             user_tmp.UID = std::stoi(line.substr(0, line.find_first_of(',')));
//             line = line.substr(line.find_first_of(',') + 1);

//             user_tmp.tipo = line.substr(0, line.find_first_of(','))[0];
//             line = line.substr(line.find_first_of(',') + 1);

//             user_tmp.grupo = line.substr(0, line.find_first_of(','));
//             line = line.substr(line.find_first_of(',') + 1);

//             user_tmp.nombre = line.substr(0, line.find_first_of(','));
//             line = line.substr(line.find_first_of(',') + 1);

//             user_tmp.contrasena = line.substr(0, line.find_first_of('\n'));

//             if (user_tmp.nombre == _user_to_login.nombre && user_tmp.contrasena == _user_to_login.contrasena)
//             {
//                 user_tmp.logged_in = true;
//                 user_tmp.mounted = _mounted;
//                 fclose(file);
//                 file = NULL;
//                 return user_tmp;
//             }
//             break;
//         default:
//             break;
//         }
//     }
//     fclose(file);
//     file = NULL;
//     return user_tmp;
// }

// Groups search_Group(Groups _group_to_create)
// {
//     Groups group_tmp;
//     Superbloque super_bloque = getSuperBloque(_user_logged.mounted);
//     FILE *file = fopen(_user_logged.mounted.path.c_str(), "rb");

//     InodosTable users_inode;
//     fseek(file, super_bloque.s_inode_start, SEEK_SET); // Mover el puntero al inicio de la tabla de inodos
//     fseek(file, sizeof(InodosTable), SEEK_CUR);        // Mover el puntero al segundo inodo que corresponde al archivo de users.txt
//     fread(&users_inode, sizeof(InodosTable), 1, file); // Leer el inodo

//     ArchivosBlock users_file;
//     fseek(file, super_bloque.s_block_start, SEEK_SET);  // Mover el puntero al inicio de la tabla de bloques
//     fseek(file, 64, SEEK_CUR);                          // Mover el puntero al segundo bloque que corresponde al archivo de users.txt
//     fread(&users_file, sizeof(ArchivosBlock), 1, file); // Leer el bloque

//     /* LEER LÍNEA POR LÍNEA EL ARCHIVO USERS.TXT */
//     std::istringstream f(users_file.b_content);
//     string line;
//     int gid = 1;
//     while (getline(f, line))
//     {
//         // std::cout << line << std::endl;
//         int count = 0;
//         for (int i = 0; (i = line.find(',', i)) != std::string::npos; i++)
//             count++;
//         switch (count)
//         {
//         case 2:
//             gid++;
//             group_tmp.GID = std::stoi(line.substr(0, line.find_first_of(',')));
//             line = line.substr(line.find_first_of(',') + 1);

//             group_tmp.tipo = line.substr(0, line.find_first_of(','))[0];
//             line = line.substr(line.find_first_of(',') + 1);

//             group_tmp.nombre = line.substr(0, line.find_first_of('\n'));

//             if (group_tmp.nombre == _group_to_create.nombre)
//             {
//                 group_tmp.nombre = "ERROR";
//                 return group_tmp;
//             }
//             break;
//         default:
//             break;
//         }
//     }
//     string tmp = users_file.b_content + std::to_string(gid) + ",G," + _group_to_create.nombre + "\n";
//     strcpy(users_file.b_content, tmp.c_str());

//     return group_tmp;
// }

#endif