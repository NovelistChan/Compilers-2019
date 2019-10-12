%{
  #include <stdio.h>
  #include "syntaxTree.h"
  #include "lex.yy.c"
  TreeNode *root;
%}

/* declared types */

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
CompSt : LC DefList StmtList RC { $$ = createNewNode("ParamDec")}
  ;
StmtList : Stmt StmtList
  | /* empty */
  ;
Stmt : Exp SEMI
  | CompSt
  | RETURN Exp SEMI
  | IF LP Exp RP Stmt
  | IF LP Exp RP Stmt ELSE Stmt
  | WHILE LP Exp RP Stmt
  ;

/* Local Definintions */
DefList : Def DefList
  | /* empty */
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
  | MINUS Exp
  | NOT Exp
  | ID LP Args RP
  | ID LP RP
  | Exp LB Exp RB
  | Exp DOT ID
  | ID
  | INT
  | FLOAT
  ;
Args : Exp COMMA Args
  | Exp
  ;


%%

