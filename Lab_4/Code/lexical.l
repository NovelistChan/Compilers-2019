%{
  #include <stdbool.h>
  #include "syntax.tab.h"
  #include "syntaxTree.h"

  bool is_pass = true; // check error

  int yycolumn = 1;
  #define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}

%option yylineno
BADPOSTFIX (0x|0X)(([0-9]*[g-zG-Z][0-9]*)*)
BADOCT 0((0|8|9)+)
BADPREFIX 0[a-wA-Wy-zY-Z]([0-9]*[a-zA-Z]*[0-9]*)*
BADHEX {BADPREFIX}|{BADPOSTFIX}
DEC 0|[1-9][0-9]*
OCT 0(0|[1-7][0-7]*)
HEX (0x|0X)(0|[1-9a-fA-F][0-9a-fA-F]*)
FLOAT {DEC}\.[0-9]+
TYPE int|float
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
SPACE [ \f\t\r\v]+
LINE \n
ID [_a-zA-Z][_a-zA-Z0-9]*
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
%%
{BADHEX} {
  printf("Error type A at Line %d: Illegal hexadecimal number \'%s\'.\n", yylineno, yytext);
  
  is_pass = false;
  yylval.type_node = NULL;
  return INT;
}

{BADOCT} {
  printf("Error type A at Line %d: Illegal octal number \'%s\'.\n", yylineno, yytext);
  
  is_pass = false;
  yylval.type_node = NULL;
  return INT;
}

{DEC} {
  yylval.type_node = createNewNodeVal("DEC", yytext, yylineno);
  return INT;
}

{HEX} {
  yylval.type_node = createNewNodeVal("HEX", yytext, yylineno);
  return INT;
}

{OCT} {
  yylval.type_node = createNewNodeVal("OCT", yytext, yylineno);
  return INT;
}

{FLOAT} {
  yylval.type_node = createNewNodeVal("FLOAT", yytext, yylineno);
  return FLOAT;
}

{STRUCT} {
  yylval.type_node = createNewNodeID("STRUCT", yylineno);
  return STRUCT;
}

{RETURN} {
  yylval.type_node = createNewNodeID("RETURN", yylineno);
  return RETURN;
}

{IF} {
  yylval.type_node = createNewNodeID("IF", yylineno);
  return IF;
}

{ELSE} {
  yylval.type_node = createNewNodeID("ELSE", yylineno);
  return ELSE;
}

{WHILE} {
  yylval.type_node = createNewNodeID("WHILE", yylineno);
  return WHILE;
}

{TYPE} {
  yylval.type_node = createNewNodeVal("TYPE", yytext, yylineno);
  return TYPE;
}

{SPACE} {}

{LINE} { yycolumn = 1; }

{ID} {
  yylval.type_node = createNewNodeVal("ID", yytext, yylineno);
  return ID;
}

{SEMI} {
  yylval.type_node = createNewNodeID("SEMI", yylineno);
  return SEMI;
}

{COMMA} {
  yylval.type_node = createNewNodeID("COMMA", yylineno);
  return COMMA;
}

{ASSIGNOP} {
  yylval.type_node = createNewNodeID("ASSIGNOP", yylineno);
  return ASSIGNOP;
}

{RELOP} {
  yylval.type_node = createNewNodeVal("RELOP", yytext, yylineno);
  return RELOP;
}

{PLUS} {
  yylval.type_node = createNewNodeID("PLUS", yylineno);
  return PLUS;
}

{MINUS} {
  yylval.type_node = createNewNodeID("MINUS", yylineno);
  return MINUS;
}

{STAR} {
  yylval.type_node = createNewNodeID("STAR", yylineno);
  return STAR;
}

{DIV} {
  yylval.type_node = createNewNodeID("DIV", yylineno);
  return DIV;
}

{AND} {
  yylval.type_node = createNewNodeID("AND", yylineno);
  return AND;
}

{OR} {
  yylval.type_node = createNewNodeID("OR", yylineno);
  return OR;
}

{DOT} {
  yylval.type_node = createNewNodeID("DOT", yylineno);
  return DOT;
}

{NOT} {
  yylval.type_node = createNewNodeID("NOT", yylineno);
  return NOT;
}

{LP} {
  yylval.type_node = createNewNodeID("LP", yylineno);
  return LP;
}

{RP} {
  yylval.type_node = createNewNodeID("RP", yylineno);
  return RP;
}

{LB} {
  yylval.type_node = createNewNodeID("LB", yylineno);
  return LB;
}

{RB} {
  yylval.type_node = createNewNodeID("RB", yylineno);
  return RB;
}

{LC} {
  yylval.type_node = createNewNodeID("LC", yylineno);
  return LC;
}

{RC} {
  yylval.type_node = createNewNodeID("RC", yylineno);
  return RC;
}

. {
  printf("Error type A at Line %d: Mysterious characters \"%s\".\n", yylineno, yytext);
  
  is_pass = false;
}
%%

