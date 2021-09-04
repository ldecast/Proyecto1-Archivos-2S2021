#include <iostream>
#include <string.h>
#include "../partitions/func.h"
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../handler.h"
#include "func.h"

using std::string;

int login(string _user, string _pwd, string _id)
{
    if (_id == "" || _pwd == "" || _id == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    if (_user_logged.logged_in)
        return coutError("Error: debe cerrar la sesión activa.", NULL);

    Disk_id disk_id = buildID(_id);

    int index = existMountedID(disk_id);
    if (index == -1)
        return coutError("No se encuentra ninguna partición montada con el id '" + _id + "'.", NULL);
    MOUNTED mounted = _particiones_montadas[index];

    Users user_to_login, user_tmp;
    user_to_login.nombre = _user;
    user_to_login.contrasena = _pwd;

    FILE *file = fopen(mounted.path.c_str(), "rb");
    /* Lectura del superbloque */
    Superbloque super_bloque;
    fseek(file, startByteSuperBloque(mounted), SEEK_SET);
    fread(&super_bloque, sizeof(Superbloque), 1, file);

    InodosTable users_inode;
    fseek(file, super_bloque.s_inode_start, SEEK_SET); // Mover el puntero al inicio de la tabla de inodos
    fseek(file, sizeof(InodosTable), SEEK_CUR);        // Mover el puntero al segundo inodo que corresponde al archivo de users.txt
    fread(&users_inode, sizeof(InodosTable), 1, file); // Leer el inodo

    ArchivosBlock users_file;
    fseek(file, super_bloque.s_block_start, SEEK_SET);  // Mover el puntero al inicio de la tabla de bloques
    fseek(file, 64, SEEK_CUR);                          // Mover el puntero al segundo bloque que corresponde al archivo de users.txt
    fread(&users_file, sizeof(ArchivosBlock), 1, file); // Leer el bloque
    fclose(file);
    file = NULL;

    /* Obtener todo el archivo concatenado */
    string content_file = GetAllFile(users_inode, super_bloque.s_block_start, mounted.path);
    file = fopen((mounted.path).c_str(), "rb");

    /* LEER LÍNEA POR LÍNEA EL ARCHIVO USERS.TXT */
    std::istringstream f(content_file);
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

            if (user_tmp.nombre == user_to_login.nombre && user_tmp.contrasena == user_to_login.contrasena)
            {
                user_tmp.logged_in = true;
                user_tmp.mounted = mounted;
                _user_logged = user_tmp;
                fclose(file);
                file = NULL;
                _user_logged.GID = getGroupByName(user_tmp.grupo, users_inode, super_bloque.s_block_start, mounted.path).GID;
                /* std::cout << _user_logged.UID << std::endl;
                std::cout << _user_logged.tipo << std::endl;
                std::cout << _user_logged.grupo << std::endl;
                std::cout << _user_logged.nombre << std::endl;
                std::cout << _user_logged.contrasena << std::endl;
                std::cout << _user_logged.logged_in << std::endl; */
                return 1;
            }
            break;
        default:
            break;
        }
    }
    fclose(file);
    file = NULL;
    return coutError("No se encuentra ningún usuario con la contraseña brindada. Intente de nuevo.", NULL);
}