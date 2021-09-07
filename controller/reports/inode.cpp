#include <iostream>
#include <fstream>
#include <string.h>
#include "../../model/structures.h"
#include "../../model/filesystem.h"
#include "../handler.h"
#include "../file_system/func.h"
#include "func.h"

using std::string;

string getDotInode(FILE *_file, int _index_inode, int _start_inodes, string _dot, int _index);

string ReportInodes(MOUNTED _mounted)
{
    string grafo = getHeader_1();
    FILE *file = fopen(_mounted.path.c_str(), "rb");

    /* Lectura del superbloque */
    int start_byte_sb = startByteSuperBloque(_mounted);
    Superbloque super_bloque;
    fseek(file, start_byte_sb, SEEK_SET);
    fread(&super_bloque, sizeof(Superbloque), 1, file);

    int first_inode = 0;
    grafo += getDotInode(file, first_inode, super_bloque.s_inode_start, "", 0);
    grafo += "}";

    fclose(file);
    file = NULL;
    return grafo;
}

string getDotInode(FILE *_file, int _index_inode, int _start_inodes, string _dot, int _index)
{
    _dot += "";
    return _dot;
}