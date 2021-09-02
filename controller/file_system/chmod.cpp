#include <iostream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../handler.h"
#include "func.h"

using std::string;

int CambiarPermisos(string _path, int _u, int _g, int _o, bool _r)
{

    return 1;
}

int chmod(string _path, string _ugo, string _r)
{
    if (_path == "" || _ugo == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);

    if (!_user_logged.logged_in)
        return coutError("Error: No se encuentra ninguna sesión activa.", NULL);

    if (_ugo.length() != 3)
        return coutError("Error: parámetro 'ugo' no válido, deben ser tres números enteros.", NULL);

    int u = std::stoi(_ugo.substr(0, 1));
    int g = std::stoi(_ugo.substr(1, 1));
    int o = std::stoi(_ugo.substr(2, 1));

    return CambiarPermisos(_path, u, g, o, _r != "");
}