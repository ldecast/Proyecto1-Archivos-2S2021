#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include "driver.h"
#include <bits/stdc++.h>

void RunAnalyzer(std::string _filename)
{
  parser_driver driver;
  if (driver.parse(_filename))
    std::cout << "\033[1;32mSuccessfully\033[0m\n";
}

int exec(std::string _path)
{
  std::ifstream file(_path);
  std::string line;
  std::string filename = "exec.txt";
  while (std::getline(file, line))
  {
    if (line != "" && line[0] != '#')
    {
      std::cout << std::endl;
      std::cout << ">> " + line << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(350));
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
  // exec("/home/ldecast/Escritorio/run.sh");
  std::string input, output;
  std::string filename = "command.txt";
  while (true)
  {
    std::cout << "\n>> ";
    getline(std::cin, input);
    if (strstr(input.c_str(), std::string("exec").c_str()))
    {
      std::string path = input.substr(input.find("-path") + 6);
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

    std::ofstream MyFile(filename);
    MyFile << input;
    MyFile.close();

    RunAnalyzer(filename);
  }
  return 0;
}
