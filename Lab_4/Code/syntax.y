%{
#include <stdio.h>
#include <stdbool.h>
#include "syntaxTree.h"
#include "lex.yy.c"

extern void yyerror(char* msg);

extern int yylineno;
extern bool is_pass;
TreeNode* root = NULL;
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
%type <type_node> Program ExtDefList ExtDef ExtDecList FunDeclaration
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
Program : ExtDefList { $$ = createNewNodeNot("Program", @$.first_line); insertNode($$, $1); root = $$;
    /* printTree(root, 0); */
  } 
  ;
ExtDefList : ExtDef ExtDefList { $$ = createNewNodeNot("ExtDefList", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | FunDeclaration ExtDefList { $$ = createNewNodeNot("ExtDefList", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | /* empty */ { $$ = NULL; }
  ;
ExtDef : Specifier ExtDecList SEMI { $$ = createNewNodeNot("ExtDef", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Specifier SEMI { $$ = createNewNodeNot("ExtDef", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | Specifier FunDec CompSt { $$ = createNewNodeNot("ExtDef", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  ;
ExtDecList : VarDec { $$ = createNewNodeNot("ExtDecList", @$.first_line); insertNode($$, $1); }
  | VarDec COMMA ExtDecList { $$ = createNewNodeNot("ExtDecList", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  ;
FunDeclaration : Specifier FunDec SEMI { $$ = createNewNodeNot("FunDeclaration", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  ;

/* Specifiers */
Specifier : TYPE { $$ = createNewNodeNot("Specifier", @$.first_line); insertNode($$, $1); }
  | StructSpecifier { $$ = createNewNodeNot("Specifier", @$.first_line); insertNode($$, $1); }
  ;
StructSpecifier : STRUCT OptTag LC DefList RC { $$ = createNewNodeNot("StructSpecifier", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); insertNode($$, $5); }
  | STRUCT Tag { $$ = createNewNodeNot("StructSpecifier", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  ;
OptTag : ID { $$ = createNewNodeNot("OptTag", @$.first_line); insertNode($$, $1); }
  | /* empty */ { $$ = NULL; }
  ;
Tag : ID { $$ = createNewNodeNot("Tag", @$.first_line); insertNode($$, $1); }
  ;

/* Declarators */
VarDec : ID { $$ = createNewNodeNot("VarDec", @$.first_line); insertNode($$, $1); }
  | VarDec LB INT RB { $$ = createNewNodeNot("VarDec", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); }
  | VarDec LB error RB { $$ = NULL; }
  ;
FunDec : ID LP VarList RP { $$ = createNewNodeNot("FunDec", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); }
  | ID LP RP { $$ = createNewNodeNot("FunDec", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | error RP { $$ = NULL; }
  ;
VarList : ParamDec COMMA VarList { $$ = createNewNodeNot("VarList", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | ParamDec { $$ = createNewNodeNot("VarList", @$.first_line); insertNode($$, $1); }
  ;
  
ParamDec : Specifier VarDec { $$ = createNewNodeNot("ParamDec", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  ;

/* Statements */
CompSt : LC DefList StmtList RC { $$ = createNewNodeNot("CompSt", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); }
  | error RC { $$ = NULL; }
  ;
StmtList : Stmt StmtList { $$ = createNewNodeNot("StmtList", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | /* empty */ { $$ = NULL; }
  ;
Stmt : Exp SEMI { $$ = createNewNodeNot("Stmt", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | CompSt { $$ = createNewNodeNot("Stmt", @$.first_line); insertNode($$, $1); }
  | RETURN Exp SEMI { $$ = createNewNodeNot("Stmt", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | IF LP Exp RP Stmt { $$ = createNewNodeNot("Stmt", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); insertNode($$, $5); }
  | IF LP Exp RP Stmt ELSE Stmt { $$ = createNewNodeNot("Stmt", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); insertNode($$, $5); insertNode($$, $6); insertNode($$, $7); }
  | WHILE LP Exp RP Stmt { $$ = createNewNodeNot("Stmt", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); insertNode($$, $5); }
  | error SEMI { $$ = NULL; }
  ;

/* Local Definintions */
DefList : Def DefList { $$ = createNewNodeNot("DefList", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | /* empty */ { $$ = NULL; }
  ;
Def : Specifier DecList SEMI { $$ = createNewNodeNot("Def", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  ;
DecList : Dec { $$ = createNewNodeNot("DecList", @$.first_line); insertNode($$, $1); }
  | Dec COMMA DecList { $$ = createNewNodeNot("DecList", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  ;
Dec : VarDec { $$ = createNewNodeNot("Dec", @$.first_line); insertNode($$, $1); }
  | VarDec ASSIGNOP Exp { $$ = createNewNodeNot("Dec", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  ;

/* Expressions */
Exp : Exp ASSIGNOP Exp { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp AND Exp { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp OR Exp { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp RELOP Exp { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp PLUS Exp { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp MINUS Exp { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp STAR Exp { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp DIV Exp { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | LP Exp RP { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | MINUS Exp %prec UMINUS { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | NOT Exp { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | ID LP Args RP { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); }
  | ID LP RP { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp LB Exp RB { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); }
  | Exp DOT ID { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | ID { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); }
  | INT { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); }
  | FLOAT { $$ = createNewNodeNot("Exp", @$.first_line); insertNode($$, $1); }
  | LP error RP { $$ = NULL; }
  | LB error RB { $$ = NULL; }
  ;
Args : Exp COMMA Args { $$ = createNewNodeNot("Args", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp { $$ =createNewNodeNot("Args", @$.first_line); insertNode($$, $1); }
  ;


%%
void yyerror(char* msg){
//  fprintf(stderr, "%s\n", msg);
  is_pass = false;
  fprintf(stderr, "Error type B at Line %d: Unexpected token \"%s\".\n", yylineno, yytext);
}
