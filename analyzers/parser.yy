%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.2"
%defines
%define api.parser.class {class_parser}
%define api.token.constructor
%define api.namespace {yy}
%define api.value.type variant
%define parse.assert
%code requires

{
#include <string>
#include <stdio.h>

class parser_driver;
}

%param { parser_driver& driver }
%locations
%define parse.trace
%define parse.error verbose
%code

{
#include "driver.h"
#include "../model/builder.h"
#include "../controller/bloque.cpp"
#include <iostream>
#include <vector>

std::vector<parametro> parametros;
struct command x;

}
%define api.token.prefix {TOK_}

//Listadode Terminales
%token <std::string> HYPHEN "tk_HYPHEN"
%token <std::string> EQUAL "tk_EQUAL"
%token <std::string> FIT "pr_FIT"
%token <std::string> UNIT "pr_UNIT"
%token <std::string> PATH "pr_PATH"
%token <std::string> SIZE "pr_SIZE"
%token <std::string> TYPE "pr_TYPE"
%token <std::string> DELETE "pr_DELETE"
%token <std::string> NAME "pr_NAME"
%token <std::string> ADD "pr_ADD"
%token <std::string> ID "pr_ID"
%token <std::string> FS "pr_FS"

%token <std::string> MKDISK "pr_MKDISK"
%token <std::string> RMDISK "pr_RMDISK"
%token <std::string> FDISK "pr_FDISK"
%token <std::string> MOUNT "pr_MOUNT"
%token <std::string> UMOUNT "pr_UMOUNT"
%token <std::string> MKFS "pr_MKFS"

%token <std::string> STRING_DOUBLE "tk_string_d"
%token <std::string> STRING_SINGLE "tk_string_s"
%token <std::string> PATH_DIR "tk_path"
%token <std::string> IDENTIFICADOR "tk_identifier"
%token <std::string> SPACE "tk_space"
%token <std::string> NUMERO "tk_number"
%token FIN 0 "eof"

//Listado de No Terminales
%type E
%type PARAMS
%type DISCOS
%type <std::string> PARAM
%type <std::string> STRING
%type <std::string> DATA

//%printer { yyoutput << $$; } <*>;
%%

%start INICIO;

INICIO: E "eof" { return bloque(x); }
;

STRING: "tk_string_d" {$$=$1.substr(1, $1.size() - 2);}
      | "tk_string_s" {$$=$1.substr(1, $1.size() - 2);}
;

DATA: STRING {$$=$1;}
    | "tk_number" {$$=$1;}
    | "tk_path" {$$=$1;}
    | "tk_identifier" {$$=$1;}
;

E: DISCOS
;

DISCOS: "pr_MKDISK" PARAMS { x = newCommand("__MKDISK",parametros); }
      | "pr_RMDISK" PARAMS { x = newCommand("__RMDISK",parametros); }
      | "pr_FDISK" PARAMS { x = newCommand("__FDISK",parametros); }
      | "pr_MOUNT" PARAMS { x = newCommand("__MOUNT",parametros); }
      | "pr_UMOUNT" PARAMS { x = newCommand("__UMOUNT",parametros); }
      | "pr_MKFS" PARAMS { x = newCommand("__MKFS",parametros); }
;

PARAMS: PARAMS "tk_space" "tk_HYPHEN" PARAM "tk_EQUAL" DATA { parametros.push_back({$4, $6}); }
      | "tk_space" "tk_HYPHEN" PARAM "tk_EQUAL" DATA { parametros.clear(); parametros.push_back({$3, $5}); }
;

PARAM: "pr_SIZE" {$$ = "__SIZE";}
      | "pr_FIT" { $$ = "__FIT";}
      | "pr_UNIT" {$$ = "__UNIT";}
      | "pr_PATH" {$$ = "__PATH";}
      | "pr_TYPE" {$$ = "__TYPE";}
      | "pr_DELETE" {$$ = "__DELETE";}
      | "pr_NAME" {$$ = "__NAME";}
      | "pr_ADD" {$$ = "__ADD";}
      | "pr_ID" {$$ = "__ID";}
      | "pr_FS" {$$ = "__FS";}
;

%%
void yy::class_parser::error(const location_type& lugar, const std::string& lexema)
{
  std::cout << "ERROR SINTÁCTICO: " << lexema << std::endl;
  exit(EXIT_FAILURE);
}
