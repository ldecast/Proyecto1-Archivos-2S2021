#include <iostream>
#include <fstream>
#include <string.h>
#include "../../model/structures.h"
#include "../handler.h"
#include "../disks/func.h"
#include "func.h"

using std::string;

std::string getDot();

int ReportMBR(string _path, Disk_id _disk_id)
{
    string grafo =
        string("digraph MBR {\n") +
        "graph[bgcolor=\"#141D26\"]\n" +
        "node [shape=box style=filled color=\"#48D1CC\" fontname= \"Ubuntu\" fontsize=\"14\"];\n" +
        "edge[color=\"#B22222\" penwidth=\"1.5\"];\n";
    grafo += getDot();
    std::cout << grafo << std::endl;
    writeDot(grafo);
    generateReport(_path);
    return 1;
}

string getDot()
{
    string dot;
    dot += "1->2;\n";
    dot += "}";
    return dot;
}