%{
#include <stdio.h>
#include "syntaxTree.h"
#include "lex.yy.c"

int yylex();
extern int yylineno;
%}

/* declared types  */
%union {
  TreeNode* type_node;
}

/* declared tokens */
%token <type_node> INT FLOAT
%token <type_node> ID
%token <type_node> SEMI COMMA
%token <type_node> ASSIGNOP
%token <type_node> RELOP
%token <type_node> PLUS MINUS STAR DIV
%token <type_node> AND OR NOT
%token <type_node> DOT
%token <type_node> TYPE
%token <type_node> LP RP
%token <type_node> LB RB
%token <type_node> LC RC
%token <type_node> STRUCT RETURN IF ELSE WHILE

/* declared non-terminals */
%type <type_node> Program ExtDefList ExtDef ExtDecList
%type <type_node> Specifier StructSpecifier OptTag Tag
%type <type_node> VarDec FunDec VarList ParamDec
%type <type_node> CompSt StmtList Stmt
%type <type_node> DefList Def DecList Dec
%type <type_node> Exp Args

/* %error-verbose */

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right UMINUS NOT
%left DOT LP RP LB RB

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE 

%%
/* High-level Definitions  */
Program : ExtDefList
  ;
ExtDefList : ExtDef ExtDefList
  | /* empty */ {$$=NULL;}
  ;
ExtDef : Specifier ExtDecList SEMI
  | Specifier SEMI
  | Specifier FunDec CompSt
  ;
ExtDecList : VarDec
  | VarDec COMMA ExtDecList
  ;

/* Specifiers */
Specifier : TYPE
  | StructSpecifier
  ;
StructSpecifier : STRUCT OptTag LC DefList RC
  | STRUCT OptTag LC error RC
  | STRUCT Tag
  ;
OptTag : ID
  | /* empty */ {$$=NULL;}
  ;
Tag : ID
  ;

/* Declarators */
VarDec : ID
  | VarDec LB INT RB
  | VarDec LB error RB
  ;
FunDec : ID LP VarList RP
  | ID LP RP
  | error RP {$$=NULL;}
  ;
VarList : ParamDec COMMA VarList
  | ParamDec
  ;
ParamDec : Specifier VarDec
  ;

/* Statements */
CompSt : LC DefList StmtList RC
  | error RC {$$=NULL;}
  ;
StmtList : Stmt StmtList
  | /* empty */ {$$=NULL;}
  ;
Stmt : Exp SEMI
  | CompSt
  | RETURN Exp SEMI
  | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
  | IF LP Exp RP Stmt ELSE Stmt
  | WHILE LP Exp RP Stmt
  | error SEMI {$$=NULL;}
  ;

/* Local Definintions */
DefList : Def DefList
  | /* empty */ {$$=NULL;}
  ;
Def : Specifier DecList SEMI
  ;
DecList : Dec
  | Dec COMMA DecList
  ;
Dec : VarDec
  | VarDec ASSIGNOP Exp
  ;

/* Expressions */
Exp : Exp ASSIGNOP Exp
  | Exp AND Exp
  | Exp OR Exp
  | Exp RELOP Exp
  | Exp PLUS Exp
  | Exp MINUS Exp
  | Exp STAR Exp
  | Exp DIV Exp
  | LP Exp RP
  | MINUS Exp %prec UMINUS
  | NOT Exp
  | ID LP Args RP
  | ID LP RP
  | Exp LB Exp RB
  | Exp DOT ID
  | ID
  | INT
  | FLOAT
  | LP error RP
  | LB error RB
  ;
Args : Exp COMMA Args
  | Exp
  ;


%%
int yyerror(char* msg){
//  fprintf(stderr, "%s\n", msg);
  return fprintf(stderr, "Error type B at Line %d: Unexpected token \"%s\".\n", yylineno, yytext);
}
