#include <iostream>
#include <fstream>
#include <string.h>
#include "../../model/filesystem.h"
#include "../file_system/func.h"
#include "func.h"

using std::string;

string getDotLS(FILE *_file, int _index_inode, int _start_inodes, int _start_blocks, string _dot, string _name);

string ReportLS(MOUNTED _mounted)
{
    string grafo = getHeader_1();
    FILE *file = fopen(_mounted.path.c_str(), "rb");

    /* Lectura del superbloque */
    int start_byte_sb = startByteSuperBloque(_mounted);
    Superbloque super_bloque;
    fseek(file, start_byte_sb, SEEK_SET);
    fread(&super_bloque, sizeof(Superbloque), 1, file);

    grafo += string("\"Ls Report\" [margin=\"1\" label = <\n") +
             "<TABLE BGCOLOR=\"#009999\" BORDER=\"2\" COLOR=\"BLACK\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n" +
             "<TR>\n" +
             "<TD HEIGHT=\"30\" BGCOLOR=\"#B8860B\" COLSPAN=\"8\">LS REPORT</TD>\n" +
             "</TR>\n\n" +

             "<TR>\n" +
             "<TD HEIGHT=\"35\" WIDTH=\"100\" BGCOLOR=\"#708090\"><B>Permisos</B></TD>\n" +
             "<TD WIDTH=\"100\" BGCOLOR=\"#708090\"><B>Propietario</B></TD>\n" +
             "<TD WIDTH=\"100\" BGCOLOR=\"#708090\"><B>Grupo</B></TD>\n" +
             "<TD WIDTH=\"100\" BGCOLOR=\"#708090\"><B>Size<br/>(en bytes)</B></TD>\n" +
             "<TD WIDTH=\"100\" BGCOLOR=\"#708090\"><B>Fecha y hora<br/>de creación</B></TD>\n" +
             "<TD WIDTH=\"100\" BGCOLOR=\"#708090\"><B>Fecha y hora<br/>de modificación</B></TD>\n" +
             "<TD WIDTH=\"100\" BGCOLOR=\"#708090\"><B>Tipo</B></TD>\n" +
             "<TD WIDTH=\"100\" BGCOLOR=\"#708090\"><B>Nombre</B></TD>\n" +
             "</TR>\n\n";

    grafo += getDotLS(file, 0, super_bloque.s_inode_start, super_bloque.s_block_start, "", "");
    grafo += "</TABLE>>];\n}";

    fclose(file);
    file = NULL;
    return grafo;
}

string getDotLS(FILE *_file, int _index_inode, int _start_inodes, int _start_blocks, string _dot, string _name)
{
    /* Leer lo que está dentro de la carpeta */
    InodosTable inode_current;
    fseek(_file, _start_inodes, SEEK_SET);
    fseek(_file, _index_inode * sizeof(InodosTable), SEEK_CUR);
    fread(&inode_current, sizeof(InodosTable), 1, _file);
    /* Agregar al dot */
    string i_ctime = ctime(&inode_current.i_ctime);
    string i_mtime = ctime(&inode_current.i_mtime);
    _dot += string("<TR>\n") +
            "<TD HEIGHT=\"60\">" + std::to_string(inode_current.i_perm) + "</TD>\n" +
            "<TD>" + ((inode_current.i_uid == 0) ? ("1") : (std::to_string(inode_current.i_uid))) + "</TD>\n" +
            "<TD>" + std::to_string(inode_current.i_gid) + "</TD>\n" +
            "<TD>" + std::to_string(inode_current.i_size) + "</TD>\n" +
            "<TD>    " + i_ctime + "    </TD>\n" +
            "<TD>    " + i_mtime + "    </TD>\n" +
            "<TD>" + ((inode_current.i_type == '0') ? ("Carpeta") : ("Archivo")) + "</TD>\n" +
            "<TD>" + ((inode_current.i_type == '0') ? ("/" + _name) : (_name)) + "</TD>\n" +
            "</TR>\n\n";
    if (inode_current.i_type == '0')
    {
        CarpetasBlock file_block;
        for (int i = 0; i < 15; i++) // falta indirectos
        {
            if (inode_current.i_block[i] != -1)
            { /* Leer el bloque y redireccionar al inodo y ver si de nuevo es otra carpeta */
                fseek(_file, _start_blocks, SEEK_SET);
                fseek(_file, inode_current.i_block[i] * 64, SEEK_CUR);
                fread(&file_block, 64, 1, _file);
                for (int j = 0; j < 4; j++)
                {
                    _name = file_block.b_content[j].b_name;
                    if (file_block.b_content[j].b_inodo > 0 && file_block.b_content[j].b_inodo != _index_inode && string(_name) != ".." && string(_name) != ".")
                    {
                        // std::cout << _name << std::endl;
                        _index_inode = file_block.b_content[j].b_inodo;
                        _dot = getDotLS(_file, _index_inode, _start_inodes, _start_blocks, _dot, _name);
                    }
                }
            }
        }
    }
    return _dot;
}