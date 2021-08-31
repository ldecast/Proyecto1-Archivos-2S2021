#include <iostream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../../model/users_groups.h"
#include "../handler.h"
#include "func.h"

using std::string;

int CrearArchivo(string _path, bool _r, int _size, string _cont, bool _stdin)
{

    return 1;
}

int touch(string _path, string _r, string _size, string _cont, string _stdin)
{
    if (_path == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);
    int preSize = (_size == "" || _cont != "") ? -1 : std::stoi(_size);
    return CrearArchivo(_path, _r != "", getSize('b', preSize), _cont, _stdin != "");
    /* std::cout << _path << std::endl;
    std::cout << _r << std::endl;
    std::cout << _size << std::endl;
    std::cout << _cont << std::endl;
    std::cout << _stdin << std::endl;
    std::cout << "\n"; */
}
