#ifndef BUILDER
#define BUILDER
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

using std::string;

struct command
{
    string keyword;
    string size;
    string fit;
    string unit;
    string path;
    string type;
    string delet;
    string name;
    string add;
    string id;
    string fs;

    string user;
    string pwd;
    string grp;

    string ugo;
    string r;
    string cont;
    string _stdin;

    string ruta;
    string root;
};

struct parametro
{
    string tipo;
    string valor;
};

struct command newCommand(string _type, std::vector<parametro> _params)
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

        else if (_params[i].tipo == "__USER")
            x.user = _params[i].valor;

        else if (_params[i].tipo == "__PWD")
            x.pwd = _params[i].valor;

        else if (_params[i].tipo == "__GRP")
            x.grp = _params[i].valor;

        else if (_params[i].tipo == "__RUTA")
            x.ruta = _params[i].valor;

        else if (_params[i].tipo == "__ROOT")
            x.root = _params[i].valor;

        else if (_params[i].tipo == "__UGO")
            x.ugo = _params[i].valor;

        else if (_params[i].tipo == "__R")
            x.r = _params[i].valor;

        else if (_params[i].tipo == "__CONT")
            x.cont = _params[i].valor;

        else if (_params[i].tipo == "__STDIN")
            x._stdin = _params[i].valor;

        else
            std::cout << "Parámetro no válido: " + _params[i].valor << std::endl;
    }
    return x;
}

#endif