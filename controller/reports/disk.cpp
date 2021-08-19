#include <iostream>
#include <fstream>
#include <string.h>
#include "../../model/structures.h"
#include "../handler.h"
#include "../disks/func.h"
#include "func.h"
#include <cmath>

using std::string;

struct _s
{
    string dot = "";
    int free;
};

string getDotPartitions(MBR _mbr, string _path, FILE *_file);
_s getDotLogics(EBR _ebr, int _mbr_tamano, FILE *_file, string _color, _s retorno);

int ReportDisk(MOUNTED _mounted, string _dirOutput)
{
    string grafo = getHeaderMBR();
    FILE *_file = fopen(_mounted.path.c_str(), "rb");

    MBR mbr;
    fseek(_file, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, _file);
    grafo += getDotPartitions(mbr, _mounted.path, _file);

    grafo += "}";

    // std::cout << grafo << std::endl;
    writeDot(grafo);
    generateReport(_dirOutput);

    fclose(_file);
    _file = NULL;
    return 1;
}

string getPorcentaje(int _free, int _mbr_tamano)
{
    // std::cout << "_mbr_tamano: " + std::to_string(_mbr_tamano) << std::endl;
    // std::cout << "_free: " + std::to_string(_free) << std::endl;
    float tmp = static_cast<float>(_free) / static_cast<float>(_mbr_tamano) * 100.0; //????????'
    return std::to_string((int)round(tmp)) + "%";
}

int getSizeBeforeFree(MBR _mbr, int _num)
{
    while (_num >= 0)
    {
        if (_mbr.mbr_partition[_num].part_size != 0)
        {
            return _mbr.mbr_tamano + sizeof(MBR) - (_mbr.mbr_partition[_num].part_start + _mbr.mbr_partition[_num].part_size);
        }
        _num--;
    }
    return 0;
}

string getDotPartitions(MBR _mbr, string _path, FILE *_file)
{
    string dot = "";
    string type;
    int free = _mbr.mbr_tamano;

    string colors[] = {"#1abc9c", "#839192", "#e59866", "#7fb3d5", "#f5b041"};

    dot +=
        string("\"Disk report\" [ label = <") +
        "<TABLE CELLBORDER=\"2\" BGCOLOR=\"BLACK\" BORDER=\"2\" COLOR=\"BLACK\"  CELLSPACING=\"0\">" +
        "\n\n" +
        "<TR>" +
        "<TD BGCOLOR=\"#b7950b\" COLSPAN=\"100\">" +
        "<FONT POINT-SIZE=\"20\">Disk Report of: " + _path.substr(root.length()) + "</FONT>" +
        "</TD>" +
        "</TR>" +
        "<TR>" +
        "\n\n" +
        "<TD HEIGHT=\"150\" WIDTH=\"75\" BGCOLOR=\"#cd6155\">MBR</TD>" +
        "\n\n";

    for (int i = 0; i < 4; i++)
    {
        partition _particion = _mbr.mbr_partition[i];
        if (_particion.part_size != 0)
            free = _particion.part_size;
        if (_particion.part_type == 'P' && _particion.part_status != '0')
            type = "Primaria";
        else if (_particion.part_type == 'E')
            type = "Extendida";
        else
        {
            type = "Libre";
            // free = _mbr.mbr_tamano - _particion.part_start - _particion.part_size;
            // free = _mbr.mbr_tamano - free;
            free = getSizeBeforeFree(_mbr, i);
            std::cout << "free after Found libre: " + std::to_string(free) << std::endl;
        }

        dot += string("<TD HEIGHT=\"160\" WIDTH=\"150\" BGCOLOR=\"" + colors[i] + "\">") +
               "PARTICIÓN " + std::to_string(i + 1) +
               "<br/>" +
               "<br/>" +
               type +
               "<br/>" +
               "<br/>" +
               "<b>" + getPorcentaje(free, _mbr.mbr_tamano) + "</b>" +
               "<br/>" +
               "</TD>" +
               "\n\n";

        if (_particion.part_type == 'E')
        {
            EBR ebr;
            fseek(_file, _particion.part_start, SEEK_SET);
            fread(&ebr, sizeof(EBR), 1, _file);
            _s _retorno;
            _s _retorno_2;
            _retorno.free = ebr.part_size;
            if (ebr.part_size != 0)
                _retorno_2 = getDotLogics(ebr, _mbr.mbr_tamano, _file, colors[i], _retorno);
            // free = _retorno_2.free;
            dot += _retorno_2.dot;
            std::cout << "free after Logics: " + std::to_string(free) << std::endl;
        }
    }
    free = _mbr.mbr_tamano - getPartitionsSize(_mbr);
    if (free > sizeof(MBR) && type != "Libre")
    {
        dot += string("<TD HEIGHT=\"160\" WIDTH=\"100\" BGCOLOR=\"" + colors[4] + "\">") +
               "LIBRE" +
               "<br/>" +
               "<br/>" +
               "<b>" + getPorcentaje(free, _mbr.mbr_tamano) + "</b>" +
               "<br/>" +
               "</TD>" +
               "\n\n";
    }

    dot += "\n</TR>\n\n</TABLE>>];\n";
    return dot;
}

_s getDotLogics(EBR _ebr, int _mbr_tamano, FILE *_file, string _color, _s retorno)
{
    // retorno.dot = _dot;
    // retorno.free = _free;
    if (_ebr.part_size == 0)
        retorno.free = _ebr.part_next - _ebr.part_start - _ebr.part_size;
    else
        retorno.free = _ebr.part_size;
    retorno.dot +=
        string("<TD HEIGHT=\"160\" WIDTH=\"40\" BGCOLOR=\"" + _color + "\">") +
        "EBR" +
        "</TD>" +
        "<TD HEIGHT=\"160\" WIDTH=\"80\" BGCOLOR=\"" + _color + "\">" +
        "<br/>" +
        "Lógica" +
        "<br/>" +
        "<br/>" +
        "<b>" + getPorcentaje(retorno.free, _mbr_tamano) + "</b>" +
        "<br/>" +
        "</TD>" +
        "\n\n";

    if (_ebr.part_next != -1 && _ebr.part_next - _ebr.part_start - _ebr.part_size > 0)
    {
        // retorno.free = _ebr.part_next - _ebr.part_start - _ebr.part_size;
        retorno.dot +=
            string("<TD HEIGHT=\"160\" WIDTH=\"80\" BGCOLOR=\"") + _color + "\">" +
            "<br/>" +
            "Libre" +
            "<br/>" +
            "<br/>" +
            "<b>" + getPorcentaje(_ebr.part_next - _ebr.part_start - _ebr.part_size, _mbr_tamano) + "</b>" +
            "<br/>" +
            "</TD>" +
            "\n\n";
    }

    // retorno.free = _ebr.part_size;
    if (_ebr.part_next != -1)
    {
        fseek(_file, _ebr.part_next, SEEK_SET);
        fread(&_ebr, sizeof(EBR), 1, _file);
        return getDotLogics(_ebr, _mbr_tamano, _file, _color, retorno);
    }
    std::cout << "_free: " + std::to_string(retorno.free) << std::endl;
    return retorno;
    // return {
    //     dot : _dot,
    //     free : _free
    // };
}