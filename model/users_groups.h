#ifndef STRUCTURES_USERS_GROUPS
#define STRUCTURES_USERS_GROUPS

#include <iostream>
#include <stdio.h>

struct Groups
{
    int GID = 1;
    char tipo = 'G';
    char nombre[10] = "root";
};

struct Users
{
    int UID = 1;
    char tipo = 'U';
    char grupo[10] = "root";
    char nombre[10] = "root";
    char contrasena[10] = "123";
};

std::string getData(Groups _group, Users _user)
{
    std::string data = std::to_string(_group.GID) + "," + _group.tipo + "," + _group.nombre + "\n" + std::to_string(_user.UID) + "," + _user.tipo + "," + _user.grupo + "," + _user.nombre + "," + _user.contrasena + "\n";
    return data;
}

#endif