#ifndef __OBJECTCODE_H__
#define __OBJECTCODE_H__

#include "syntaxTree.h"
#include  "interCode.h"

typedef struct RegDescription_ *RegDescription;
typedef struct VarDescription_ *VarDescription;
typedef union AddressDescription_ *AddressDescription;
typedef struct OffsetNode_ *OffsetNode;

/*
typedef enum {
    REG,
    STACK,
    SEGMENT
} AddrType;

struct AddressDescription_{
    AddrType addrType;
    union{
        int regNo;
        int offset; // offset of stack
        char *name;  // name of variable in segment
    }addr;
    AddressDescription next;
};
*/

union AddressDescription_{
    int regNo;
    int offset; // offset of stack
    char *name;  // name of variable in segment
};

struct VarDescription_ {
    char* varName;
    AddressDescription addrDescription[3]; // 0: REG; 1: STACK; 2: SEGMENT

    VarDescription next;
};

struct RegDescription_ {
    char* name; // name of the reg
    bool ifFree; // if is free
    int dirty; // next use
    VarDescription var; // the variable stored
};

struct OffsetNode_ {
    int offset;
    OffsetNode next;
};

OffsetNode nowOffset;

RegDescription regs[32];
VarDescription varHead; // variable list with head node
FILE* fp;

int getReg(Operand op);
void initialVarList();
void initialRegisters();
void printObjectCode(char* fileName);

#endif