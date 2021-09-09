clear
#flex lexico.l
#bison parser.yy
g++ -std=c++1z driver.cpp main.cpp parser.tab.cc scanner.cpp
./a.out
