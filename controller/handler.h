#ifndef HANDLER
#define HANDLER

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;
std::string root = "/home/ldecast/Escritorio"; // url raíz

int coutError(std::string err, FILE *_file)
{
    if (_file != NULL)
    {
        fclose(_file);
        _file = NULL;
    }
    std::cout << "\033[1;31m" + err + "\033[0m\n";
    return 0;
}

void exitFailure(std::string err)
{
    std::cout << "\033[1;31m" + err + "\033[0m\n";
    exit(EXIT_FAILURE);
}

int intSize(std::string _size)
{
    int ns = std::stoi(_size);
    if (ns <= 0)
        exitFailure("Error: el valor de '-size' debe ser mayor que cero.");
    return ns;
}

std::string getPath(std::string _path)
{
    std::string np = _path;
    if (_path[0] == '\"')
        np = _path.substr(1, _path.size() - 2);
    if (np[0] != '/')
        np = "/" + np;
    np = root + np;
    return np;
}

std::string getDir(std::string np)
{
    std::string dir = np.substr(0, np.find_last_of('/'));
    return dir;
}

bool isDir(std::string dir)
{
    if (!fs::is_directory(dir) && !fs::exists(dir))
        return false;
    else
        return true;
}

std::string buildPath(std::string _path)
{
    std::string np = getPath(_path);
    std::string dir = getDir(np);
    if (!isDir(dir))                 // Check if path exists
        fs::create_directories(dir); // create src folder
    return np;
}

char getFit(std::string _fit, char _default)
{
    char nf;
    transform(_fit.begin(), _fit.end(), _fit.begin(), ::toupper);
    if (_fit == "BF") //best
        nf = 'B';
    else if (_fit == "FF") //first
        nf = 'F';
    else if (_fit == "WF") //worst
        nf = 'W';
    else if (_fit == "")
        nf = _default;
    else //err
        exitFailure("Error: parámetro -f no válido: " + _fit);
    return nf;
}

char getUnit(std::string _unit, char _default)
{
    char nu;
    transform(_unit.begin(), _unit.end(), _unit.begin(), ::tolower);
    if (_unit == "b")
        nu = 'b';
    else if (_unit == "k")
        nu = 'k';
    else if (_unit == "m")
        nu = 'm';
    else if (_unit == "")
        nu = _default;
    else //err
        exitFailure("Error: parámetro -u no válido: " + _unit);
    return nu;
}

int getSize(char _unit, int _size)
{
    if (_unit == 'b') //bytes
        return _size;
    else if (_unit == 'k') //kb
        return _size * 1000;
    else if (_unit == 'm') //mb
        return _size * 1000 * 1000;
    return -1;
}

char charType(std::string _type, char _default)
{
    char ntype;
    transform(_type.begin(), _type.end(), _type.begin(), ::toupper);
    if (_type == "E")
        ntype = 'E';
    else if (_type == "L")
        ntype = 'L';
    else if (_type == "P")
        ntype = 'P';
    else if (_type == "")
        ntype = _default;
    else //err
        exitFailure("Error: parámetro -type no válido: " + _type);
    return ntype;
}

char charDelete(std::string _delete)
{
    char ndelete;
    transform(_delete.begin(), _delete.end(), _delete.begin(), ::tolower);
    if (_delete == "fast")
        ndelete = 'R'; //rápida
    else if (_delete == "full")
        ndelete = 'C'; //completa
    else if (_delete == "")
        ndelete = 'N'; //no aplica
    else
        exitFailure("Error: parámetro -delete no válido: " + _delete);
    return ndelete;
}

#endif