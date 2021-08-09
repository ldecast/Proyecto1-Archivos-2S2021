#include <iostream>
#include <string.h>
#include "../structures.h"
#include "../handler.h"

int BorrarDisco(std::string _path)
{
    char src[_path.size() + 1];
    strcpy(src, _path.c_str());
    if (remove(src) != 0)
        return coutError("Error: no se pudo eliminar el disco.");
    return 1;
}

int removeDisk(std::string _path)
{
    std::string np = getPath(_path);
    if (!isDir(np))
        return coutError("Error: el disco no existe.");
    char ans;
    std::cout << "Confirme eliminar el disco ubicado en: '" + np + "' [Y/N]:" << std::endl;
    std::cin >> ans;
    if (ans == 'Y' || ans == 'y')
        return BorrarDisco(np);
    else
        return 1;
}