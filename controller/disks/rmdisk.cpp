#include <iostream>
#include <string.h>
#include "../handler.h"

int BorrarDisco(std::string _path)
{
    char src[_path.size() + 1];
    strcpy(src, _path.c_str());
    if (remove(src) != 0)
        return coutError("Error: no se pudo eliminar el disco.", NULL);
    return 1;
}

int removeDisk(std::string _path)
{
    if (_path == "")
        return coutError("Error: faltan parámetros obligatorios.", NULL);
    std::string np = getPath(_path);
    if (!isDir(np))
        return coutError("Error: el disco no existe.", NULL);
    char ans;
    std::cout << "¿Desea eliminar el disco ubicado en: '" + np + "?' [Y/n]" << std::endl;
    std::cin >> ans;
    if (ans == 'Y' || ans == 'y')
        return BorrarDisco(np);
    else
        return 1;
}