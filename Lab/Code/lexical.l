%option yylineno
INT 0|[1-9][0-9]*
OCT 0(0|[1-7][0-7]*)
HEX (0x|0X)(0|[1-9a-fA-F][0-9a-fA-F]*)
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
{INT} { printf("INT: %s\n", yytext); }
{FLOAT} { printf("FLOAT: %s\n", yytext); }
{STRUCT} { printf("STRUCT: %s\n", yytext); }
{RETURN} { printf("RETURN: %s\n", yytext); }
{IF} { printf("IF: %s\n", yytext); }
{ELSE} { printf("ELSE: %s\n", yytext); }
{WHILE} { printf("WHILE: %s\n", yytext); }
{TYPE} { printf("TYPE: %s\n", yytext); }
{SPACE} { //printf("SPACE: %s\n", yytext); 
}
{ID} { printf("ID: %s\n", yytext); }
{SEMI} { printf("SEMI: %s\n", yytext); }
{COMMA} { printf("COMMA: %s\n", yytext); }
{ASSIGNOP} { printf("ASSIGNOP: %s\n", yytext); }
{RELOP} { printf("RELOP: %s\n", yytext); }
{PLUS} { printf("PLUS: %s\n", yytext); }
{MINUS} { printf("MINUS: %s\n", yytext); }
{STAR} { printf("STAR: %s\n", yytext); }
{DIV} { printf("DIV: %s\n", yytext); }
{AND} { printf("AND: %s\n", yytext); }
{OR} { printf("OR: %s\n", yytext); }
{DOT} { printf("DOT: %s\n", yytext); }
{NOT} { printf("NOT: %s\n", yytext); }
{LP} { printf("LP: %s\n", yytext); }
{RP} { printf("RP: %s\n", yytext); }
{LB} { printf("LB: %s\n", yytext); }
{RB} { printf("RB: %s\n", yytext); }
{LC} { printf("LC: %s\n", yytext); }
{RC} { printf("RC: %s\n", yytext); }
. { printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%
int main(int argc, char** argv) {
    if (argc > 1) {
    	if (!(yyin = fopen(argv[1], "r"))) {
			perror(argv[1]);
			return 1;
		}
	}
	while (yylex() != 0);
    return 0;
}
