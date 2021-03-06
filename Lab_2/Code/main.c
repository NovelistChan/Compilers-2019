#include <stdio.h>
#include <stdbool.h>

extern void yyrestart();
extern int yyparse();
extern void createHashTable();
extern void semanticAnalysis();
extern bool is_pass;

int main(int argc, char** argv){
  if(argc<=1) return 1;

  FILE* f = fopen(argv[1], "r");
  if(!f){
    perror(argv[1]);
    return 1;
  }

  yyrestart(f);
  yyparse();

  if(is_pass){
    createHashTable();
    semanticAnalysis();
  }
  return 0;
}

