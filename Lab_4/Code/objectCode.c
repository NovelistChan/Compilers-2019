#include"interCode.h"
#include"hashTable.h"
#include"syntaxTree.h"
#include"semantic.h"
#include<stdlib.h>
#include<string.h>
#include"objectCode.h"

int getReg(Operand op) {
    // TODO: 局部寄存器分配算法
}

void initialRegisters() {
    for (int i = 0; i < 32; i++) {
        reg[i]->var = NULL;
        reg[i]->ifFree = true;
	}
    
    strcpy(reg[0]->name, "$zero");
    strcpy(reg[1]->name, "$at");
    strcpy(reg[2]->name, "$v0");
    strcpy(reg[3]->name, "$v1");
    strcpy(reg[4]->name, "$a0");
    strcpy(reg[5]->name, "$a1");
    strcpy(reg[6]->name, "$a2");
    strcpy(reg[7]->name, "$a3");
    strcpy(reg[8]->name, "$t0");
    strcpy(reg[9]->name, "$t1");
    strcpy(reg[10]->name, "$t2");
    strcpy(reg[11]->name, "$t3");
    strcpy(reg[12]->name, "$t4");
    strcpy(reg[13]->name, "$t5");
    strcpy(reg[14]->name, "$t6");
    strcpy(reg[15]->name, "$t7");
    strcpy(reg[16]->name, "$s0");
    strcpy(reg[17]->name, "$s1");
    strcpy(reg[18]->name, "$s2");
    strcpy(reg[19]->name, "$s3");
    strcpy(reg[20]->name, "$s4");
    strcpy(reg[21]->name, "$s5");
    strcpy(reg[22]->name, "$s6");
    strcpy(reg[23]->name, "$s7");
    strcpy(reg[24]->name, "$t8");
    strcpy(reg[25]->name, "$t9");
    strcpy(reg[26]->name, "$k0");
    strcpy(reg[27]->name, "$k1");
    strcpy(reg[28]->name, "$gp");
    strcpy(reg[29]->name, "$sp");
    strcpy(reg[30]->name, "$s8");
    strcpy(reg[31]->name, "$ra");
}

void printObjectCode(InterCode code, char *fileName) {
    // if (code->kind == LABEL) ...
}