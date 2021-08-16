#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include "driver.h"
#include <bits/stdc++.h>

using namespace std;

int RunAnalyzer(std::string _filename)
{
  parser_driver driver;
  return driver.parse(_filename);
}

int exec(std::string _path)
{
  std::ifstream file(_path);
  std::string line;
  string filename = "exec.txt";
  while (std::getline(file, line))
  {
    if (line != "" && line[0] != '#')
    {
      std::cout << std::endl;
      std::cout << ">> " + line << std::endl;
      // std::this_thread::sleep_for(0.1s);
      std::ofstream MyFile(filename);
      MyFile << line;
      MyFile.close();
      RunAnalyzer(filename);
    }
  }
  return 1;
}

int main()
{
  exec("/home/ldecast/Escritorio/run.sh");
  string input, output;
  string filename = "command.txt";
  while (true)
  {
    cout << "\n>> ";
    getline(cin, input);
    if (strstr(input.c_str(), string("exec").c_str()))
    {
      string path = input.substr(input.find("-path") + 6);
      exec(path);
      continue;
    }

    if (input == "exit" || input == "quit")
      return 0;
    if (input == "cls" || input == "clear")
    {
      system("clear");
      continue;
    }
    if (input == "")
      continue;

    ofstream MyFile(filename);
    MyFile << input;
    MyFile.close();

    RunAnalyzer(filename);
  }
  return 0;
}