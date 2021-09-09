#include <iostream>
#include <fstream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../file_system/func.h"
#include "func.h"

using std::string;

string getDotBlock(int _index_inode, int _start_inodes, int _start_blocks, string _path);
string getDot_folder_block(int _start_blocks, int _index_block, string _path);
string getDot_file_block(int _start_blocks, int _index_block, string _path);
string getDot_pointer_block(int _start_blocks, int _index_block, string _path);

string ReportBlocks(MOUNTED _mounted)
{
    string grafo = getHeader_1();
    FILE *file = fopen(_mounted.path.c_str(), "rb");

    /* Lectura del superbloque */
    int start_byte_sb = startByteSuperBloque(_mounted);
    Superbloque super_bloque;
    fseek(file, start_byte_sb, SEEK_SET);
    fread(&super_bloque, sizeof(Superbloque), 1, file);
    fclose(file);
    file = NULL;

    grafo += getDotBlock(0, super_bloque.s_inode_start, super_bloque.s_block_start, _mounted.path);
    grafo += "}";

    return grafo;
}

string getDotBlock(int _index_inode, int _start_inodes, int _start_blocks, string _path)
{
    FILE *_file = fopen(_path.c_str(), "rb");
    string _dot = "";
    /* Leer el inodo */
    InodosTable inode_current;
    fseek(_file, _start_inodes, SEEK_SET);
    fseek(_file, _index_inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_current, sizeof(InodosTable), 1, _file);
    fclose(_file);
    if (inode_current.i_type == '0')
    {
        CarpetasBlock folder_block;
        for (int i = 0; i < 15; i++) // falta indirectos
        {
            if (inode_current.i_block[i] != -1)
            { /* Leer el bloque y redireccionar al inodo y ver si de nuevo es otra carpeta */
                _dot += getDot_folder_block(_start_blocks, inode_current.i_block[i], _path);
                _file = fopen(_path.c_str(), "rb");
                fseek(_file, _start_blocks, SEEK_SET);
                fseek(_file, inode_current.i_block[i] * 64, SEEK_CUR);
                fread(&folder_block, 64, 1, _file);
                fclose(_file);
                for (int j = 0; j < 4; j++)
                {
                    if (folder_block.b_content[j].b_inodo > 0 && folder_block.b_content[j].b_inodo != _index_inode && string(folder_block.b_content[j].b_name) != ".." && string(folder_block.b_content[j].b_name) != ".")
                    {
                        // std::cout << folder_block.b_content[j].b_name << std::endl;
                        _index_inode = folder_block.b_content[j].b_inodo;
                        _dot += getDotBlock(_index_inode, _start_inodes, _start_blocks, _path);
                    }
                }
            }
        }
    }
    else // Es inodo de archivo
    {
        for (int i = 0; i < 15; i++)
        {
            if (inode_current.i_block[i] != -1)
                _dot += getDot_file_block(_start_blocks, inode_current.i_block[i], _path);
        }
    }
    _file = NULL;
    return _dot;
}

string getDot_folder_block(int _start_blocks, int _index_block, string _path)
{
    FILE *_file = fopen(_path.c_str(), "rb");
    CarpetasBlock cb;
    fseek(_file, _start_blocks, SEEK_SET);
    fseek(_file, _index_block * 64, SEEK_CUR);
    fread(&cb, 64, 1, _file);
    string dot =
        string("\"BLOCK_" + std::to_string(_index_block) + "\" [ fontsize=\"17\" label = <\n") +
        "<TABLE BGCOLOR=\"#009999\" BORDER=\"2\" COLOR=\"BLACK\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n" +
        "<TR>\n" +
        "<TD BGCOLOR=\"#B8860B\" COLSPAN=\"2\">Bloque Carpeta " + std::to_string(_index_block) + "</TD>\n" +
        "</TR>\n" +
        "<TR>\n" +
        "<TD WIDTH=\"130\" BGCOLOR=\"#708090\"><B>b_name</B></TD>\n" +
        "<TD WIDTH=\"70\" BGCOLOR=\"#708090\"><B>b_inodo</B></TD>\n" +
        "</TR>\n" +
        "\n";
    for (int i = 0; i < 4; i++)
    {
        dot +=
            string("<TR>\n") +
            "<TD ALIGN=\"left\">   " + cb.b_content[i].b_name + "</TD>\n" +
            "<TD>" + std::to_string(cb.b_content[i].b_inodo) + "</TD>\n" +
            "</TR>\n" +
            "\n";
    }
    dot += "</TABLE>>];\n\n";
    fclose(_file);
    _file = NULL;
    return dot;
}

string getDot_file_block(int _start_blocks, int _index_block, string _path)
{
    FILE *_file = fopen(_path.c_str(), "rb");
    ArchivosBlock ab;
    fseek(_file, _start_blocks, SEEK_SET);
    fseek(_file, _index_block * 64, SEEK_CUR);
    fread(&ab, 64, 1, _file);
    string content = string(ab.b_content);
    size_t pos;
    while ((pos = content.find("\n")) != std::string::npos)
        content.replace(pos, 1, "<br/>");
    string dot =
        string("\"BLOCK_" + std::to_string(_index_block) + "\" [ fontsize=\"17\" label = <\n") +
        "<TABLE BGCOLOR=\"#009999\"  BORDER=\"2\" COLOR=\"BLACK\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n" +
        "<TR>\n" +
        "<TD WIDTH=\"190\" BGCOLOR=\"#708090\">Bloque Archivo " + std::to_string(_index_block) + "</TD>\n" +
        "</TR>\n" +
        "\n" +
        "<TR>\n" +
        "<TD>\n";
    dot += (content + "\n");
    dot += string("</TD>\n</TR>\n\n</TABLE>>];\n\n");
    fclose(_file);
    _file = NULL;
    return dot;
}

string getDot_pointer_block(int _start_blocks, int _index_block, string _path)
{
    FILE *_file = fopen(_path.c_str(), "rb");
    ApuntadoresBlock ab;
    fseek(_file, _start_blocks, SEEK_SET);
    fseek(_file, _index_block * 64, SEEK_CUR);
    fread(&ab, 64, 1, _file);
    string dot =
        string("\"BLOCK_" + std::to_string(_index_block) + "\" [ fontsize=\"17\" label = <\n") +
        "<TABLE BGCOLOR=\"#009999\"  BORDER=\"2\" COLOR=\"BLACK\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n" +
        "<TR>\n" +
        "<TD WIDTH=\"190\" BGCOLOR=\"#708090\">Bloque Apuntadores " + std::to_string(_index_block) + "</TD>\n" +
        "</TR>\n" +
        "\n" +
        "<TR>\n" +
        "<TD>\n";
    string content = "";
    for (int i = 0; i < 16; i++)
    {
        content += (std::to_string(ab.b_pointers[i]) + ",");
        if ((i + 1) % 4 == 0)
            content += "<br/>";
    }
    content.erase(content.length() - 1);
    dot += (content + "\n");
    dot += string("</TD>\n</TR>\n\n</TABLE>>];\n\n");
    fclose(_file);
    _file = NULL;
    return dot;
}