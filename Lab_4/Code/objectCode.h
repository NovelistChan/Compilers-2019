#ifndef __OBJECTCODE_H__
#define __OBJECTCODE_H__

#include "syntaxTree.h"
#include  "interCode.h"

typedef struct RegDescription_ *RegDescription;
typedef struct VarDescription_ *VarDescription;
typedef struct AddressDescription_ *AddressDescription;

typedef enum {
    REG,
    MEMORY,
    STACK
} AddrType;

struct AddressDescription_{
    AddrType addrType;
    union{
        int regNo;
        int offset; // offset of stack
    }addr;
    AddressDescription next;
};

struct VarDescription_ {
    char* varName;
    AddressDescription addrDescription;

    VarDescription next;
};

/*
struct VarDescription_ {
    int regNo; // the reg where the variable stored
    int position; //  the position when used next time
    VarDescrition next;
};
*/

struct RegDescription_ {
    char* name; // name of the reg
    bool ifFree; // if is free
    VarDescription var; // the variable stored
};

RegDescription regs[32];
VarDescription varHead; // variable list with head node
FILE* fp;

int getReg(Operand op);
void initialVarList();
void initialRegisters();
void printObjectCode(char* fileName);

#endif