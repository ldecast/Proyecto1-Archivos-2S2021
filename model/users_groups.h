#ifndef STRUCTURES_USERS_GROUPS
#define STRUCTURES_USERS_GROUPS

#include <iostream>
#include <stdio.h>

struct Groups
{
    int GID = 1;
    char tipo = 'G';
    string nombre = "root";
};

struct Users
{
    int UID = 1;
    int GID = 1;
    char tipo = 'U';
    string grupo = "root";
    string nombre = "root";
    string contrasena = "123";
    bool logged_in = false;
    MOUNTED mounted;
    int permisos = 777;
};

std::string getData(Groups _group, Users _user)
{
    std::string data = std::to_string(_group.GID) + "," + _group.tipo + "," + _group.nombre + "\n" + std::to_string(_user.UID) + "," + _user.tipo + "," + _user.grupo + "," + _user.nombre + "," + _user.contrasena + "\n";
    return data;
}

Users _user_logged;

#endif