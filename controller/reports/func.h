#ifndef FUNC_REPORTS
#define FUNC_REPORTS

#include <iostream>
#include <string.h>
#include <fstream>

std::string getHeader_1()
{
    std::string graph =
        std::string("digraph G {\n") +
        +"graph[bgcolor=\"#141D26\"]\n" +
        +"rankdir=\"TB\";\n" +
        +"node [shape=plaintext fontname= \"Ubuntu\" fontsize=\"14\"];\n" +
        +"edge [style=\"invis\"];\n\n";
    return graph;
}

void writeDot(std::string _dot)
{
    std::ofstream MyFile("../controller/reports/report.dot");
    MyFile << _dot;
    MyFile.close();
}

void generateReport(std::string _path)
{
    std::string extension = _path.substr(_path.find_last_of('.') + 1);
    std::string syst = "dot -T" + extension + " " + "../controller/reports/report.dot" + " -o " + _path;
    system(syst.c_str());
}

int generateJournalingReport(std::string _path)
{
    std::string dir = "../controller/reports/journal.dot";
    std::ofstream f;
    f.open(dir, std::ios_base::app); // append instead of overwrite
    f << "</TABLE>>];\n}";
    f.close();
    std::string extension = _path.substr(_path.find_last_of('.') + 1);
    std::string syst = "dot -T" + extension + " " + dir + " -o " + _path;
    system(syst.c_str());

    remove(dir.c_str());
    return 1;
}

#endif