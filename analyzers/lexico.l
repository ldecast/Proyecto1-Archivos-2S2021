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
{HYPHEN}"f"		          {return yy::class_parser::make_FIT(yytext, loc);}
{HYPHEN}"u"		          {return yy::class_parser::make_UNIT(yytext, loc);}
{HYPHEN}"path"          {return yy::class_parser::make_PATH(yytext, loc);}
{HYPHEN}"size"          {return yy::class_parser::make_SIZE(yytext, loc);}
{HYPHEN}"type"          {return yy::class_parser::make_TYPE(yytext, loc);}
{HYPHEN}"delete"        {return yy::class_parser::make_DELETE(yytext, loc);}
{HYPHEN}"name"          {return yy::class_parser::make_NAME(yytext, loc);}
{HYPHEN}"add"           {return yy::class_parser::make_ADD(yytext, loc);}
{HYPHEN}"id"            {return yy::class_parser::make_ID(yytext, loc);}
{HYPHEN}"fs"            {return yy::class_parser::make_FS(yytext, loc);}

{HYPHEN}"ruta"          {return yy::class_parser::make_RUTA(yytext, loc);}
{HYPHEN}"root"          {return yy::class_parser::make_ROOT(yytext, loc);}


"mkdisk"		    {return yy::class_parser::make_MKDISK(yytext, loc);}
"rmdisk"		    {return yy::class_parser::make_RMDISK(yytext, loc);}
"fdisk"		      {return yy::class_parser::make_FDISK(yytext, loc);}
"mount"		      {return yy::class_parser::make_MOUNT(yytext, loc);}
"umount"		    {return yy::class_parser::make_UMOUNT(yytext, loc);}
"mkfs"		      {return yy::class_parser::make_MKFS(yytext, loc);}
"exec"		      {return yy::class_parser::make_RUN_EXEC(yytext, loc);}
"rep"           {return yy::class_parser::make_RUN_REPORT(yytext, loc);}

"login"         {return yy::class_parser::make_LOGIN(yytext, loc);}
"logout"        {return yy::class_parser::make_LOGOUT(yytext, loc);}
"mkgrp"         {return yy::class_parser::make_MKGRP(yytext, loc);}
"rmgrp"         {return yy::class_parser::make_RMGRP(yytext, loc);}
"mkusr"         {return yy::class_parser::make_MKUSR(yytext, loc);}
"rmusr"         {return yy::class_parser::make_RMUSR(yytext, loc);}
{HYPHEN}("user"|"usr")  {return yy::class_parser::make_USER(yytext, loc);}
{HYPHEN}"pwd"           {return yy::class_parser::make_PWD(yytext, loc);}
{HYPHEN}"grp"           {return yy::class_parser::make_GRP(yytext, loc);}

"chmod"           {return yy::class_parser::make_CHMOD(yytext, loc);}
"touch"           {return yy::class_parser::make_TOUCH(yytext, loc);}
"cat"             {return yy::class_parser::make_CAT(yytext, loc);}
"rm"              {return yy::class_parser::make_RM(yytext, loc);}
"edit"            {return yy::class_parser::make_EDIT(yytext, loc);}
"mkdir"           {return yy::class_parser::make_MKDIR(yytext, loc);}
{HYPHEN}"ugo"     {return yy::class_parser::make_UGO(yytext, loc);}
{HYPHEN}"cont"    {return yy::class_parser::make_CONT(yytext, loc);}
{HYPHEN}"stdin"   {return yy::class_parser::make_STDIN(yytext, loc);}
{HYPHEN}"r"       {return yy::class_parser::make_R(yytext, loc);}
{HYPHEN}"file"{NUMERO}  {return yy::class_parser::make_FILEN(yytext, loc);}
{HYPHEN}"p"       {return yy::class_parser::make_P(yytext, loc);}

"pause"           {
                    std::cout << "Ingrese cualquier tecla para continuar: ";
                    std::cin.ignore();
                  }
"__quit__"        {exit(1);}

{STRING_DOUBLE} {return yy::class_parser::make_STRING_DOUBLE(yytext, loc);}
{STRING_SINGLE} {return yy::class_parser::make_STRING_SINGLE(yytext, loc);}
{PATH_DIR}      {return yy::class_parser::make_PATH_DIR(yytext, loc);}
{NUMERO}        {return yy::class_parser::make_NUMERO(yytext ,loc);}
{IDENTIFICADOR}	{return yy::class_parser::make_IDENTIFICADOR(yytext, loc);}
{SPACE}         {}
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