#include <iostream>
#include <fstream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../file_system/func.h"
#include "func.h"

using std::string;

string getDot_file_block(string _name, string _content);

string ReportFile(MOUNTED _mounted, string _ruta)
{
    if (_ruta == "")
    {
        std::cout << "\033[1;31mError: Es necesario indicar el parámetro 'ruta' para continuar.\033[0m\n";
        return "-";
    }
    string npath = _ruta.substr(0, _ruta.find_last_of('/'));
    string filename = _ruta.substr(_ruta.find_last_of('/') + 1);

    string grafo = getHeader_1();
    FILE *file = fopen(_mounted.path.c_str(), "rb");

    /* Lectura del superbloque */
    int start_byte_sb = startByteSuperBloque(_mounted);
    Superbloque super_bloque;
    fseek(file, start_byte_sb, SEEK_SET);
    fread(&super_bloque, sizeof(Superbloque), 1, file);

    /* Lectura de la última carpeta padre */
    FolderReference fr;
    std::vector<string> folders = SplitPath(npath);
    for (int j = 0; j < folders.size(); j++)
    {
        fr = getFatherReference(fr, folders[j], file, super_bloque.s_inode_start, super_bloque.s_block_start);
        if (fr.inode == -1)
        {
            // std::cout << "Not found: " + folders[j] + "\n";
            std::cout << "\033[1;31mError: la ruta del archivo no se encuentra.\033[0m\n";
            return "-";
        }
    }
    int _index_inode;
    /* Lectura del inodo de carpeta padre */
    InodosTable inode_father;
    fseek(file, super_bloque.s_inode_start, SEEK_SET);
    fseek(file, fr.inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_father, sizeof(InodosTable), 1, file);
    CarpetasBlock file_block;
    InodosTable inode_current;
    bool x = false;
    for (int j = 0; j < 15 && !x; j++) // Obtener el inodo asociado directo, falta indirectos
    {
        if (inode_father.i_block[j] != -1)
        {
            fseek(file, super_bloque.s_block_start, SEEK_SET);
            fseek(file, inode_father.i_block[j] * 64, SEEK_CUR);
            fread(&file_block, 64, 1, file);
            for (int k = 0; k < 4; k++)
            {
                // std::cout << file_block.b_content[k].b_name << std::endl;
                if (string(file_block.b_content[k].b_name) == filename)
                {
                    // std::cout << file_block.b_content[k].b_inodo << std::endl;
                    _index_inode = file_block.b_content[k].b_inodo;
                    fseek(file, super_bloque.s_inode_start, SEEK_SET);
                    fseek(file, file_block.b_content[k].b_inodo * sizeof(InodosTable), SEEK_CUR);
                    fread(&inode_current, sizeof(InodosTable), 1, file);
                    x = true;
                    break;
                }
            }
        }
    }
    if (!x)
    {
        std::cout << "\033[1;31mEl archivo '" + filename + "' no se encuentra en la ruta: '" + npath + "'.\033[0m\n";
        return "-";
    }
    fclose(file);
    file = NULL;

    /* Obtener todo el archivo concatenado */
    string content = ReadFile(_index_inode, super_bloque.s_inode_start, super_bloque.s_block_start, _mounted.path);

    grafo += getDot_file_block(_ruta, content);
    grafo += "}";
    return grafo;
}

string getDot_file_block(string _name, string _content)
{
    size_t pos;
    while ((pos = _content.find("\n")) != std::string::npos)
        _content.replace(pos, 1, "     <br/>     ");
    string dot =
        string("\"FILE\" [ fontsize=\"18\" margin=\"1\" label = <\n") +
        "<TABLE BGCOLOR=\"#009999\"  BORDER=\"2\" COLOR=\"BLACK\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n" +
        "<TR>\n" +
        "<TD HEIGHT=\"40\" BGCOLOR=\"#708090\">FILE REPORT: \"" + _name + "\"</TD>\n" +
        "</TR>\n" +
        "\n" +
        "<TR>\n" +
        "<TD HEIGHT=\"70\">\n     ";
    dot += (_content + "     \n");
    dot += string("</TD>\n</TR>\n\n</TABLE>>];\n\n");
    return dot;
}
