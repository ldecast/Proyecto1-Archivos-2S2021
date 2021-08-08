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

int intSize(std::string _size)
{
    int ns = std::stoi(_size);
    if (ns <= 0)
    {
        std::cout << "Error: el valor de '-size' debe ser mayor que cero." << std::endl;
        exit(EXIT_FAILURE);
    }
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
    // Check if path exists
    std::string dir = np.substr(0, np.find_last_of('/'));
    if (!fs::is_directory(dir) || !fs::exists(dir))
        fs::create_directories(dir); // create src folder
    return np;
}

char getFit(std::string _fit)
{
    char nf;
    transform(_fit.begin(), _fit.end(), _fit.begin(), ::toupper);
    if (_fit == "BF")
        nf = 'B';
    else if (_fit == "FF" || _fit == "")
        nf = 'F';
    else if (_fit == "WF")
        nf = 'W';
    else
    {
        std::cout << "Error: valor de '-f' no válido: " + _fit << std::endl;
        exit(EXIT_FAILURE);
    }
    return nf;
}

char getUnit(std::string _unit)
{
    char nu;
    transform(_unit.begin(), _unit.end(), _unit.begin(), ::tolower);
    if (_unit == "k")
        nu = 'k';
    else if (_unit == "m" || _unit == "")
        nu = 'm';
    else //err
    {
        std::cout << "Error: parámetro -u no válido: " + _unit << std::endl;
        exit(EXIT_FAILURE);
    }
    return nu;
}

#endif