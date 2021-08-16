#ifndef FUNC_REPORTS
#define FUNC_REPORTS

#include <iostream>
#include <string.h>
#include <fstream>

void writeDot(std::string _dot)
{
    std::ofstream MyFile("report.dot");
    MyFile << _dot;
    MyFile.close();
}

void generateReport(std::string _path)
{
    std::string extension = _path.substr(_path.find_last_of('.') + 1);
    std::string syst = "dot -T" + extension + " " + "report.dot" + " -o " + _path;
    system(syst.c_str());
}

#endif