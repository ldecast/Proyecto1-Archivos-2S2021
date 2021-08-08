#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "driver.h"

using namespace std;

int main()
{
  string input, output;
  while (true)
  {
    cout << ">> ";
    getline(cin, input);

    if (input == "exit" || input == "quit")
      return 0;
    if (input == "")
      continue;

    // Create and open a text file
    ofstream MyFile("entrada.txt");
    // Write to the file
    MyFile << input;
    // Close the file
    MyFile.close();

    parser_driver driver;
    driver.parse("entrada.txt");
  }
  return 0;
}