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
// %token <std::string> HYPHEN "tk_HYPHEN"
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

%token <std::string> LOGIN "pr_login"
%token <std::string> LOGOUT "pr_logout"
%token <std::string> MKGRP "pr_MKGRP"
%token <std::string> RMGRP "pr_RMGRP"
%token <std::string> MKUSR "pr_MKUSR"
%token <std::string> RMUSR "pr_RMUSR"

%token <std::string> USER "pr_USER"
%token <std::string> PWD "pr_PWD"
%token <std::string> GRP "pr_GRP"

%token <std::string> TOUCH "pr_TOUCH"
%token <std::string> CHMOD "pr_CHMOD"
%token <std::string> R "pr_R"
%token <std::string> CONT "pr_CONT"
%token <std::string> UGO "pr_UGO"
%token <std::string> STDIN "pr_STDIN"

%token <std::string> RUTA "pr_RUTA"
%token <std::string> ROOT "pr_ROOT"


%token <std::string> MKDISK "pr_MKDISK"
%token <std::string> RMDISK "pr_RMDISK"
%token <std::string> FDISK "pr_FDISK"
%token <std::string> MOUNT "pr_MOUNT"
%token <std::string> UMOUNT "pr_UMOUNT"
%token <std::string> MKFS "pr_MKFS"

%token <std::string> STRING_DOUBLE "tk_string_d"
%token <std::string> STRING_SINGLE "tk_string_s"
%token <std::string> PATH_DIR "tk_path"
%token <std::string> RUN_EXEC "pr_exec"
%token <std::string> RUN_REPORT "pr_rep"
%token <std::string> IDENTIFICADOR "tk_identifier"
// %token <std::string> SPACE "tk_space"
%token <std::string> NUMERO "tk_number"
%token FIN 0 "eof"

//Listado de No Terminales
%type E
%type PARAMS
%type DISCOS
%type ADMIN_USERS_GROUPS
%type FILESYSTEM
%type EXEC
%type REPORT
%type <std::string> PARAM
%type <std::string> BOOLEAN_PARAM
%type <std::string> STRING
%type <std::string> DATA

//%printer { yyoutput << $$; } <*>;
%%

%start INICIO;

INICIO: E "eof"   {return bloque(x);}
      | "eof"
;

STRING: "tk_string_d" {$$=$1.substr(1, $1.size() - 2);}
      | "tk_string_s" {$$=$1.substr(1, $1.size() - 2);}
;

DATA: STRING {$$=$1;}
    | "tk_number" {$$=$1;}
    | "tk_path" {$$=$1;}
    | "tk_identifier" {$$=$1;}
;

E: EXEC
 | DISCOS
 | ADMIN_USERS_GROUPS
 | FILESYSTEM
 | REPORT
;

EXEC: "pr_exec" PARAMS {x = newCommand("__EXEC",parametros);}
;

DISCOS: "pr_MKDISK" PARAMS {x = newCommand("__MKDISK",parametros);}
      | "pr_RMDISK" PARAMS {x = newCommand("__RMDISK",parametros);}
      | "pr_FDISK" PARAMS {x = newCommand("__FDISK",parametros);}
      | "pr_MOUNT" PARAMS {x = newCommand("__MOUNT",parametros);}
      | "pr_UMOUNT" PARAMS {x = newCommand("__UMOUNT",parametros);}
      | "pr_MKFS" PARAMS {x = newCommand("__MKFS",parametros);}
;

ADMIN_USERS_GROUPS: "pr_login" PARAMS {x = newCommand("__LOGIN",parametros);}
                  | "pr_logout" {x = newCommand("__LOGOUT",parametros);}
                  | "pr_MKGRP" PARAMS {x = newCommand("__MKGRP",parametros);}
                  | "pr_RMGRP" PARAMS {x = newCommand("__RMGRP",parametros);}
                  | "pr_MKUSR" PARAMS {x = newCommand("__MKUSR",parametros);}
                  | "pr_RMUSR" PARAMS {x = newCommand("__RMUSR",parametros);}
;

FILESYSTEM: "pr_CHMOD" PARAMS {x = newCommand("__CHMOD",parametros);}
            | "pr_TOUCH" PARAMS {x = newCommand("__TOUCH",parametros);}
;

REPORT: "pr_rep" PARAMS {x = newCommand("__REP",parametros);}
;

PARAMS: PARAMS PARAM "tk_EQUAL" DATA {parametros.push_back({$2, $4});}
      | PARAMS BOOLEAN_PARAM {parametros.push_back({$2, "true"});}
      | PARAM "tk_EQUAL" DATA {parametros.clear(); parametros.push_back({$1, $3});}
      | BOOLEAN_PARAM {parametros.clear(); parametros.push_back({$1, "true"});}
;

PARAM: "pr_SIZE" {$$ = "__SIZE";}
      | "pr_FIT" {$$ = "__FIT";}
      | "pr_UNIT" {$$ = "__UNIT";}
      | "pr_PATH" {$$ = "__PATH";}
      | "pr_TYPE" {$$ = "__TYPE";}
      | "pr_DELETE" {$$ = "__DELETE";}
      | "pr_NAME" {$$ = "__NAME";}
      | "pr_ADD" {$$ = "__ADD";}
      | "pr_ID" {$$ = "__ID";}
      | "pr_FS" {$$ = "__FS";}
      | "pr_RUTA" {$$ = "__RUTA";}
      | "pr_ROOT" {$$ = "__ROOT";}
      | "pr_USER" {$$ = "__USER";}
      | "pr_PWD" {$$ = "__PWD";}
      | "pr_GRP" {$$ = "__GRP";}
      | "pr_UGO" {$$ = "__UGO";}
      | "pr_CONT" {$$ = "__CONT";}
;

BOOLEAN_PARAM: "pr_R" {$$ = "__R";}
            | "pr_STDIN" {$$ = "__STDIN";}
;

%%
void yy::class_parser::error(const location_type& lugar, const std::string& lexema)
{
  std::cout << "ERROR SINTÁCTICO: " << lexema << std::endl;
  exit(EXIT_FAILURE);
}
