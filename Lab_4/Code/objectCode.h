#ifndef __OBJECTCODE_H__
#define __OBJECTCODE_H__

#include "syntaxTree.h"
#include  "interCode.h"

typedef struct RegDescription_ *RegDescription;
typedef struct VarDescription_ *VarDescrition;

struct VarDescription_ {
    int regNo; // the reg where the variable stored
    int position; //  the position when used next time
    VarDescrition next;
};

struct RegDescription_ {
    char* name; // name of the reg
    bool ifFree; // if is free
    VarDescrition var; // the variable stored
};

extern RegDescription reg[32];

int getReg(Operand op);
void initialRegisters();
void printObjectCode(InterCode code, char* fileName);

#endif