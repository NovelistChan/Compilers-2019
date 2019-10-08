%{
  #include "syntax.tab.h"
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
SPACE " "|\t|\n|\r
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
{BADHEX} { printf("Error type A:BADHEX at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
{BADOCT} { printf("Error type A:BADOCT at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
{BADPREFIX} { printf("Error type A:BADPREFIX at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
{INT} { return INT; }
{FLOAT} { return FLOAT; }
{STRUCT} { return STRUCT; }
{RETURN} { return RETURN; }
{IF} { return IF; }
{ELSE} { return ELSE; }
{WHILE} { return WHILE; }
{TYPE} { return TYPE; }
{SPACE} { //printf("SPACE: %s\n", yytext); 
}
{ID} { return ID; }
{SEMI} { return SEMI; }
{COMMA} { return COMMA; }
{ASSIGNOP} { return ASSIGNOP; }
{RELOP} { return RELOP; }
{PLUS} { return PLUS; }
{MINUS} { return MINUS; }
{STAR} { return STAR; }
{DIV} { return DIV; }
{AND} { return AND; }
{OR} { return OR; }
{DOT} { return DOT; }
{NOT} { return NOT; }
{LP} { return LP; }
{RP} { return RP; }
{LB} { return LB; }
{RB} { return RB; }
{LC} { return LC; }
{RC} { return RC; }
. { printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%

