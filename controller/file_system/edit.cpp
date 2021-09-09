#include <iostream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../admin_gu/func.h"
#include "../handler.h"
#include "func.h"
#include "edit_file.h"

using std::string;

int edit(string _path, string _cont, string _stdin)
{
    if (_path == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);
    if (!_user_logged.logged_in)
        return coutError("Error: No se encuentra ninguna sesión activa.", NULL);
    if (!isDir(_cont))
        return coutError("Error: no se puede acceder a la ubicación: '" + _cont + "'", NULL);
    string npath = _path.substr(0, _path.find_last_of('/'));
    string filename = _path.substr(_path.find_last_of('/') + 1);
    string content = "";
    if (_stdin != "")
    {
        std::cout << "Ingrese el nuevo contenido del archivo:" << std::endl;
        std::getline(std::cin, content);
    }
    else if (_cont != "")
    {
        std::ifstream f(_cont);
        string line;
        while (std::getline(f, line))
        {
            content += (line + "\n");
        }
    }
    else
        return coutError("Error: debe existir una entrada para poder editar el archivo.", NULL);

    return EditarArchivo(npath, filename, content, _stdin != "");
}
