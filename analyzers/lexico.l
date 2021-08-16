%{
#include <cerrno>
#include <stdio.h>
#include <string>
#include "driver.h"
#include "parser.tab.hh"
#undef yywrap
#define yywrap() 1
static yy::location loc;
%}

%option noyywrap nounput batch debug noinput
%option case-insensitive
%option outfile="scanner.cpp"

COMENTARIOS "#".*
HYPHEN "-"|"–"
STRING_DOUBLE \"(\\.|[^"\\])*\"
STRING_SINGLE \'(\\.|[^"\\])*\'
SPACE " "+
DIGITO [0-9]
LETRA [a-zA-ZñÑ]
NUMERO "-"?{DIGITO}+("."{DIGITO}+)?
IDENTIFICADOR ({LETRA}|{DIGITO}|"_")+
DELIMITADOR [\t\r\n\f]
PATH_DIR \/.[^" "]+|{IDENTIFICADOR}"."{IDENTIFICADOR}

%%

"="		          {return yy::class_parser::make_EQUAL(yytext, loc);}
"f"		          {return yy::class_parser::make_FIT(yytext, loc);}
"u"		          {return yy::class_parser::make_UNIT(yytext, loc);}
"path"          {return yy::class_parser::make_PATH(yytext, loc);}
"size"          {return yy::class_parser::make_SIZE(yytext, loc);}
"type"          {return yy::class_parser::make_TYPE(yytext, loc);}
"delete"        {return yy::class_parser::make_DELETE(yytext, loc);}
"name"          {return yy::class_parser::make_NAME(yytext, loc);}
"add"           {return yy::class_parser::make_ADD(yytext, loc);}
"id"            {return yy::class_parser::make_ID(yytext, loc);}
"fs"            {return yy::class_parser::make_FS(yytext, loc);}

"ruta"            {return yy::class_parser::make_RUTA(yytext, loc);}
"root"            {return yy::class_parser::make_ROOT(yytext, loc);}


"mkdisk"		    {return yy::class_parser::make_MKDISK(yytext, loc);}
"rmdisk"		    {return yy::class_parser::make_RMDISK(yytext, loc);}
"fdisk"		      {return yy::class_parser::make_FDISK(yytext, loc);}
"mount"		      {return yy::class_parser::make_MOUNT(yytext, loc);}
"umount"		    {return yy::class_parser::make_UMOUNT(yytext, loc);}
"mkfs"		      {return yy::class_parser::make_MKFS(yytext, loc);}
"exec"		      {return yy::class_parser::make_RUN_EXEC(yytext, loc);}
"rep"           {return yy::class_parser::make_RUN_REPORT(yytext, loc);}

{HYPHEN}        {return yy::class_parser::make_HYPHEN(yytext, loc);}
{STRING_DOUBLE} {return yy::class_parser::make_STRING_DOUBLE(yytext, loc);}
{STRING_SINGLE} {return yy::class_parser::make_STRING_SINGLE(yytext, loc);}
{PATH_DIR}      {return yy::class_parser::make_PATH_DIR(yytext, loc);}
{NUMERO}        {return yy::class_parser::make_NUMERO(yytext ,loc);}
{IDENTIFICADOR}	{return yy::class_parser::make_IDENTIFICADOR(yytext, loc);}
{SPACE}         {return yy::class_parser::make_SPACE(yytext, loc);}
{DELIMITADOR}   {}
{COMENTARIOS}   {}

.               {printf("ERROR LEXICO %s\n",yytext);}
<<EOF>>		      {return yy::class_parser::make_FIN(loc);}


%%

void parser_driver::iniciarScanner()
{
  yy_flex_debug = false;
  if(file.empty() || file == "-"){
    printf("No se encuentra el archivo de entrada");
    exit(1);
  }else{
    yyin = fopen (file.c_str (), "r");
  }
}

void parser_driver::terminarScanner()
{
  fclose(yyin);
}