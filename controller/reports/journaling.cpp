#include <iostream>
#include <fstream>
#include <string.h>
#include "../../model/builder.h"
#include "func.h"

using std::string;

string FileJournaling()
{
    string grafo = getHeader_1();
    grafo +=
        string("\"Journaling\" [margin=\"1\" label = <\n") +
        "<TABLE BGCOLOR=\"#009999\" BORDER=\"2\" COLOR=\"BLACK\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n" +
        "<TR>\n" +
        "<TD HEIGHT=\"30\" BGCOLOR=\"#B8860B\" COLSPAN=\"14\">JOURNAL REPORT</TD>\n" +
        "</TR>\n\n" +

        "<TR>\n" +
        "<TD HEIGHT=\"35\" WIDTH=\"100\" BGCOLOR=\"#708090\"><B>Acción</B></TD>\n" +
        "<TD BGCOLOR=\"#708090\"><B>Size</B></TD>\n" +
        "<TD BGCOLOR=\"#708090\"><B>Path</B></TD>\n" +
        "<TD BGCOLOR=\"#708090\"><B>Type</B></TD>\n" +
        "<TD BGCOLOR=\"#708090\"><B>Name</B></TD>\n" +
        "<TD BGCOLOR=\"#708090\"><B>Fs</B></TD>\n" +
        "<TD BGCOLOR=\"#708090\"><B>Usr</B></TD>\n" +
        "<TD BGCOLOR=\"#708090\"><B>Pwd</B></TD>\n" +
        "<TD BGCOLOR=\"#708090\"><B>Grp</B></TD>\n" +
        "<TD BGCOLOR=\"#708090\"><B>Dest</B></TD>\n" +
        "<TD BGCOLOR=\"#708090\"><B>Ugo</B></TD>\n" +
        "<TD BGCOLOR=\"#708090\"><B>Recursivo</B></TD>\n" +
        "<TD BGCOLOR=\"#708090\"><B>Cont</B></TD>\n" +
        "<TD BGCOLOR=\"#708090\"><B>stdin</B></TD>\n" +
        "</TR>\n\n";
    return grafo;
}

string getDotJournaling(command tmp)
{
    string dot =
        string("<TR>\n") +
        "<TD HEIGHT=\"60\">" + tmp.keyword + "</TD>\n" +
        "<TD>" + tmp.size + "</TD>\n" +
        "<TD>" + tmp.path + "</TD>\n" +
        "<TD>" + tmp.type + "</TD>\n" +
        "<TD>" + tmp.name + "</TD>\n" +
        "<TD>" + tmp.fs + "</TD>\n" +
        "<TD>" + tmp.user + "</TD>\n" +
        "<TD>" + tmp.pwd + "</TD>\n" +
        "<TD>" + tmp.grp + "</TD>\n" +
        "<TD>" + tmp.dest + "</TD>\n" +
        "<TD>" + tmp.ugo + "</TD>\n" +
        "<TD>" + ((tmp.r == "") ? ("") : ("true")) + "</TD>\n" +
        "<TD>" + tmp.cont + "</TD>\n" +
        "<TD>" + ((tmp._stdin == "") ? ("") : ("true")) + "</TD>\n" +
        "</TR>\n\n";
    return dot;
}