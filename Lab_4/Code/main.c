#include <stdio.h>
#include <stdbool.h>

extern void yyrestart();
extern int yyparse();
extern void createHashTable();
extern void semanticAnalysis();
extern void initial();
extern void generateInterCode();
extern void printCode(char* fileName);
extern void initialVarList();
extern void initialRegisters();
extern void printObjectCode(char *fileName);
extern bool is_pass;

int main(int argc, char** argv){
  if(argc<=3){
    fprintf(stderr, "Need 3 params.\n");
    return 1;
  }

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

    if(is_pass){
      initial();
      generateInterCode();
      printCode(argv[2]);

      initialVarList();
      initialRegisters();
      printObjectCode(argv[3]);
    }
  }

  return 0;
}

