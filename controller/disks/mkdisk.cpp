#include <iostream>
#include <string.h>
#include "../structures.h"

using namespace std;

string getPath(string _path)
{
    string tmp;
    if (_path[0] == '\"')
        tmp = root + _path.substr(1, _path.size() - 2);
    else
        tmp = root + _path;
    return tmp;
}

int CrearDisco(int _size, string _fit, string _unit, string _path)
{
    string s = getPath(_path); //url raiz + url de la entrada
    cout << "CREANDO DISCO EN LA RUTA: " + s << endl;

    char sc[s.size() + 1];
    strcpy(sc, s.c_str());
    FILE *file = NULL;
    file = fopen(sc, "r"); // r=read si el disco ya existía
    if (file != NULL)
    {
        cout << "Error: El disco ya existe." << endl;
        return 0;
    }

    //Tamaño para distintas unidades que se piden
    int tam;
    if (_unit == "k") //kb
        tam = _size * 1024;
    else if (_unit == "m" || _unit == "") //mb
        tam = _size * 1000 * 1000;
    else
    {
        cout << "Error: parámetro -u no válido: " + _unit << endl;
        return 0;
    }

    file = fopen(sc, "wb");
    fwrite("\0", 1, 1, file);
    /*se pone el puntero en el tamaño deseado del archivo y esto automaticamente 
	hace que el archivo tenga ese size*/
    fseek(file, tam, SEEK_SET); // objeto file, size de cuanto se quiere mover, al inicio
    fwrite("\0", 1, 1, file);

    //////ESCRITURA DEL MBR//////
    MBR mbr;
    mbr.mbr_tamano = tam;
    mbr.mbr_fecha_creacion = time(0);
    mbr.mbr_disk_signature = rand() % 1000;
    mbr.disk_fit = (_unit == "k") ? 'k' : 'm';
    for (int i = 0; i < 4; i++)
    {
        mbr.mbr_partition[i].part_status = '0';
        mbr.mbr_partition[i].part_type = 'P';
        mbr.mbr_partition[i].part_fit = 'F';
        mbr.mbr_partition[i].part_start = 0;
        mbr.mbr_partition[i].part_size = tam;
        strcpy(mbr.mbr_partition[i].part_name, "");
    }

    //Escritura del MBR
    fseek(file, 0, SEEK_SET);
    fwrite(&mbr, sizeof(MBR), 1, file);
    fclose(file);

    cout << "-------------DISCO CREADO--------------------" << endl;
    cout << "Disco\nFecha de creación: " << asctime(gmtime(&mbr.mbr_fecha_creacion)) << endl;
    cout << "Signature: " << mbr.mbr_disk_signature << endl;
    cout << "Tamaño: " << mbr.mbr_tamano << endl;
    cout << "Fit: " << mbr.disk_fit << endl;
    return 1;
}

int makeDisk(string _size, string _fit, string _unit, string _path)
{
    try
    {
        if (_size == "" || _path == "")
        {
            cout << "Error: faltan parámetros obligatorios." << endl;
            return 0;
        }
        return CrearDisco(stoi(_size), _fit, _unit, _path);
    }
    catch (const exception &e)
    {
        cout << "ERROR!";
        cerr << e.what() << '\n';
        return 0;
    }
}
