#ifndef BUILDER
#define BUILDER
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
using namespace std;

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
};

struct parametro
{
    string tipo;
    string valor;
};

struct command x;

void newCommand(string _type, vector<parametro> _params)
{
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
            cout << "Parámetro no válido: " + _params[i].valor << endl;
    }
}

#endif