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
BADHEX (0x|0X)(([0-9]*[g-zG-Z][0-9]*)*)
BADOCT 0((0|8|9)+)
BADPREFIX 0[a-wA-Wy-zY-Z]([0-9]*[a-zA-Z]*[0-9]*)*
DEC 0|[1-9][0-9]*
OCT 0(0|[1-7][0-7]*)
HEX (0x|0X)(0|[1-9a-fA-F][0-9a-fA-F]*)
INT {DEC}|{OCT}|{HEX}
FLOAT {INT}\.[0-9]+
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

{BADPREFIX} {
  printf("Error type A at Line %d: Mysterious characters \"%s\".\n", yylineno, yytext);
  
  is_pass = false;
  yylval.type_node = NULL;
  return ID;
}

{INT} {
  yylval.type_node = createNewNodeVal("INT", yytext);
  return INT;
}

{FLOAT} {
  yylval.type_node = createNewNodeVal("FLOAT", yytext);
  return FLOAT;
}

{STRUCT} {
  yylval.type_node = createNewNodeID("STRUCT");
  return STRUCT;
}

{RETURN} {
  yylval.type_node = createNewNodeID("RETURN");
  return RETURN;
}

{IF} {
  yylval.type_node = createNewNodeID("IF");
  return IF;
}

{ELSE} {
  yylval.type_node = createNewNodeID("ELSE");
  return ELSE;
}

{WHILE} {
  yylval.type_node = createNewNodeID("WHILE");
  return WHILE;
}

{TYPE} {
  yylval.type_node = createNewNodeVal("TYPE", yytext);
  return TYPE;
}

{SPACE} {}

{LINE} { yycolumn = 1; }

{ID} {
  yylval.type_node = createNewNodeVal("ID", yytext);
  return ID;
}

{SEMI} {
  yylval.type_node = createNewNodeID("SEMI");
  return SEMI;
}

{COMMA} {
  yylval.type_node = createNewNodeID("COMMA");
  return COMMA;
}

{ASSIGNOP} {
  yylval.type_node = createNewNodeID("ASSIGNOP");
  return ASSIGNOP;
}

{RELOP} {
  yylval.type_node = createNewNodeVal("RELOP", yytext);
  return RELOP;
}

{PLUS} {
  yylval.type_node = createNewNodeID("PLUS");
  return PLUS;
}

{MINUS} {
  yylval.type_node = createNewNodeID("MINUS");
  return MINUS;
}

{STAR} {
  yylval.type_node = createNewNodeID("STAR");
  return STAR;
}

{DIV} {
  yylval.type_node = createNewNodeID("DIV");
  return DIV;
}

{AND} {
  yylval.type_node = createNewNodeID("AND");
  return AND;
}

{OR} {
  yylval.type_node = createNewNodeID("OR");
  return OR;
}

{DOT} {
  yylval.type_node = createNewNodeID("DOT");
  return DOT;
}

{NOT} {
  yylval.type_node = createNewNodeID("NOT");
  return NOT;
}

{LP} {
  yylval.type_node = createNewNodeID("LP");
  return LP;
}

{RP} {
  yylval.type_node = createNewNodeID("RP");
  return RP;
}

{LB} {
  yylval.type_node = createNewNodeID("LB");
  return LB;
}

{RB} {
  yylval.type_node = createNewNodeID("RB");
  return RB;
}

{LC} {
  yylval.type_node = createNewNodeID("LC");
  return LC;
}

{RC} {
  yylval.type_node = createNewNodeID("RC");
  return RC;
}

. {
  printf("Error type A at Line %d: Mysterious characters \"%s\".\n", yylineno, yytext);
  
  is_pass = false;
}
%%

