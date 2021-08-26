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

    Users user_to_login;
    user_to_login.nombre = _user;
    user_to_login.contrasena = _pwd;

    Users user_tmp = search_User(mounted, user_to_login);
    if (user_tmp.logged_in)
        _user_logged = user_tmp;
    else
        return coutError("No se encuentra ningún usuario con la contraseña brindada. Intente de nuevo.", NULL);

    /* std::cout << _user_logged.UID << std::endl;
    std::cout << _user_logged.tipo << std::endl;
    std::cout << _user_logged.grupo << std::endl;
    std::cout << _user_logged.nombre << std::endl;
    std::cout << _user_logged.contrasena << std::endl;
    std::cout << _user_logged.logged_in << std::endl; */

    return 1;
}