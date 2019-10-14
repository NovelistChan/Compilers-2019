%{
  #include <stdio.h>
  #include "syntaxTree.h"
  #include "lex.yy.c"
  TreeNode *root;
%}

/* declared types */
%union {
  TreeNode node;
}

/* declared tokens */
%token INT
%token FLOAT
%token ID
%token SEMI COMMA
%token ASSIGNOP
%token RELOP
%token PLUS MINUS STAR DIV
%token AND OR NOT
%token DOT
%token TYPE
%token LP RP
%token LB RB
%token LC RC
%token STRUCT RETURN IF ELSE WHILE

%%
/* High-level Definitions  */
Program : ExtDefList { $$ = createNewNode("Program", @$.first_line); insertNode($$, $1); root = $$; }
  ;
ExtDefList : ExtDef ExtDefList { $$ = createNewNode("ExtDefList", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | /* empty */ { $$ = NULL; }
  ;
ExtDef : Specifier ExtDecList SEMI { $$ = createNewNode("ExtDef", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Specifier SEMI { $$ = createNewNode("ExtDef", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | Specifier FunDec CompSt { $$ = createNewNode("ExtDef", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  ;
ExtDecList : VarDec { $$ = createNewNode("ExtDecList", @$.first_line); insertNode($$, $1); }
  | VarDec COMMA ExtDecList { $$ = createNewNode("ExtDecList", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  ;

/* Specifiers */
Specifier : TYPE { $$ = createNewNode("Specifier", @$.first_line); insertNode($$, $1); }
  | StructSpecifier { $$ = createNewNode("Specifier", @$.first_line); insertNode($$, $1); }
  ;
StructSpecifier : STRUCT OptTag LC DefList RC { $$ = createNewNode("StructSpecifier", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); insertNode($$, $5); }
  | STRUCT Tag { $$ = createNewNode("StructSpecifier", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  ;
OptTag : ID { $$ = createNewNode("OptTag", @$.first_line); insertNode($$, $1); }
  | /* empty */ { $$ = NULL; }
  ;
Tag : ID { $$ = createNewNode("Tag", @$.first_line); insertNode($$, $1); }
  ;

/* Declarators */
VarDec : ID { $$ = createNewNode("VarDec", @$.first_line); insertNode($$, $1); }
  | VarDec LB INT RB { $$ = createNewNode("VarDec", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); }
  ;
FunDec : ID LP VarList RP { $$ = createNewNode("FunDec", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); }
  | ID LP RP { $$ = createNewNode("FunDec", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  ;
VarList : ParamDec COMMA VarList { $$ = createNewNode("VarList", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | ParamDec { $$ = createNewNode("VarList", @$.first_line); insertNode($$, $1); }
  ;
ParamDec : Specifier VarDec { $$ = createNewNode("ParamDec", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  ;

/* Statements */
CompSt : LC DefList StmtList RC { $$ = createNewNode("ParamDec", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); }
  ;
StmtList : Stmt StmtList { $$ = createNewNode("StmtList", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | /* empty */ { $$ = NULL; }
  ;
Stmt : Exp SEMI { $$ = createNewNode("Stmt", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | CompSt { $$ = createNewNode("Stmt", @$.first_line); insertNode($$, $1); }
  | RETURN Exp SEMI { $$ = createNewNode("Stmt", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | IF LP Exp RP Stmt { $$ = createNewNode("Stmt", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); insertNode($$, $5); }
  | IF LP Exp RP Stmt ELSE Stmt { $$ = createNewNode("Stmt", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); insertNode($$, $5); insertNode($$, $6); insertNode($$, $7); }
  | WHILE LP Exp RP Stmt { $$ = createNewNode("Stmt", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); insertNode($$, $5); }
  ;

/* Local Definintions */
DefList : Def DefList { $$ = createNewNode("DefList", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | /* empty */ { $$ = NULL; }
  ;
Def : Specifier DecList SEMI { $$ = createNewNode("Def", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  ;
DecList : Dec { $$ = createNewNode("DecList", @$.first_line); insertNode($$, $1); }
  | Dec COMMA DecList { $$ = createNewNode("DecList", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  ;
Dec : VarDec { $$ = createNewNode("Dec", @$.first_line); insertNode($$, $1); }
  | VarDec ASSIGNOP Exp { $$ = createNewNode("Dec", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  ;

/* Expressions */
Exp : Exp ASSIGNOP Exp { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp AND Exp { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp OR Exp { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp RELOP Exp { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp PLUS Exp { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp MINUS Exp { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp STAR Exp { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp DIV Exp { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | LP Exp RP { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | MINUS Exp { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | NOT Exp { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); }
  | ID LP Args RP { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); }
  | ID LP RP { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp LB Exp RB { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); insertNode($$, $4); }
  | Exp DOT ID { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | ID { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); }
  | INT { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); }
  | FLOAT { $$ = createNewNode("Exp", @$.first_line); insertNode($$, $1); }
  ;
Args : Exp COMMA Args { $$ = createNewNode("Args", @$.first_line); insertNode($$, $1); insertNode($$, $2); insertNode($$, $3); }
  | Exp { $$ =createNewNode("Args", @$.first_line); insertNode($$, $1); }
  ;


%%

