#ifndef BUILDER
#define BUILDER
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

struct command
{
    std::string keyword;
    std::string size;
    std::string fit;
    std::string unit;
    std::string path;
    std::string type;
    std::string delet;
    std::string name;
    std::string add;
    std::string id;
    std::string fs;
};

struct parametro
{
    std::string tipo;
    std::string valor;
};

struct command newCommand(std::string _type, std::vector<parametro> _params)
{
    struct command x;
    x.keyword = _type;
    for (size_t i = 0; i < _params.size(); i++)
    {
        if (_params[i].tipo == "__SIZE")
            x.size = _params[i].valor;

        else if (_params[i].tipo == "__FIT")
            x.fit = _params[i].valor;

        else if (_params[i].tipo == "__UNIT")
            x.unit = _params[i].valor;

        else if (_params[i].tipo == "__PATH")
            x.path = _params[i].valor;

        else if (_params[i].tipo == "__TYPE")
            x.type = _params[i].valor;

        else if (_params[i].tipo == "__DELETE")
            x.delet = _params[i].valor;

        else if (_params[i].tipo == "__NAME")
            x.name = _params[i].valor;

        else if (_params[i].tipo == "__ADD")
            x.add = _params[i].valor;

        else if (_params[i].tipo == "__ID")
            x.id = _params[i].valor;

        else if (_params[i].tipo == "__FS")
            x.fs = _params[i].valor;

        else
            std::cout << "Parámetro no válido: " + _params[i].valor << std::endl;
    }
    return x;
}

#endif