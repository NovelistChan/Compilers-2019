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
        regs[i]->var = NULL;
        regs[i]->ifFree = true;
	}
    
    strcpy(regs[0]->name, "$zero");
    strcpy(regs[1]->name, "$at");
    strcpy(regs[2]->name, "$v0");
    strcpy(regs[3]->name, "$v1");
    strcpy(regs[4]->name, "$a0");
    strcpy(regs[5]->name, "$a1");
    strcpy(regs[6]->name, "$a2");
    strcpy(regs[7]->name, "$a3");
    strcpy(regs[8]->name, "$t0");
    strcpy(regs[9]->name, "$t1");
    strcpy(regs[10]->name, "$t2");
    strcpy(regs[11]->name, "$t3");
    strcpy(regs[12]->name, "$t4");
    strcpy(regs[13]->name, "$t5");
    strcpy(regs[14]->name, "$t6");
    strcpy(regs[15]->name, "$t7");
    strcpy(regs[16]->name, "$s0");
    strcpy(regs[17]->name, "$s1");
    strcpy(regs[18]->name, "$s2");
    strcpy(regs[19]->name, "$s3");
    strcpy(regs[20]->name, "$s4");
    strcpy(regs[21]->name, "$s5");
    strcpy(regs[22]->name, "$s6");
    strcpy(regs[23]->name, "$s7");
    strcpy(regs[24]->name, "$t8");
    strcpy(regs[25]->name, "$t9");
    strcpy(regs[26]->name, "$k0");
    strcpy(regs[27]->name, "$k1");
    strcpy(regs[28]->name, "$gp");
    strcpy(regs[29]->name, "$sp");
    strcpy(regs[30]->name, "$s8");
    strcpy(regs[31]->name, "$ra");
}

void printObjectCode(InterCode code, char *fileName) {
    FILE* fp = fopen(fileName, "w");

    InterCode p = head->next;
    while(p != head){
        switch(p->kind){
            case LABEL: case FUNCTION:{
                char* op = getOperand(p->u.one.op);
                fprintf(fp, "%s:\n", op);
                break;
            }
            case ASSIGN:{
                char* leftOp = getOperand(p->u.assign.left);
                // TODO
                if(p->u.assign.right->kind == CONSTANT){
                    fprintf(fp, "%s := %d\n", leftOp, p->u.assign.right->u.value);
                }else{
                    char* rightOp = getOperand(p->u.assign.right);
                    fprintf(fp, "%s := %s\n", leftOp, rightOp);
                }
                break;
            }
            case ADD:{
                char* resOp = getOperand(p->u.binop.res);
                char* op1 = getOperand(p->u.binop.op1);
                char* op2 = getOperand(p->u.binop.op2);
                fprintf(fp, "%s := %s + %s\n", resOp, op1, op2);
                break;
            }
            case MIN:{
                char* resOp = getOperand(p->u.binop.res);
                char* op1 = getOperand(p->u.binop.op1);
                char* op2 = getOperand(p->u.binop.op2);
                fprintf(fp, "%s := %s - %s\n", resOp, op1, op2);
                break;
            }
            case MUL:{
                char* resOp = getOperand(p->u.binop.res);
                char* op1 = getOperand(p->u.binop.op1);
                char* op2 = getOperand(p->u.binop.op2);
                fprintf(fp, "%s := %s * %s\n", resOp, op1, op2);
                break;
            }
            case DIV:{
                char* resOp = getOperand(p->u.binop.res);
                char* op1 = getOperand(p->u.binop.op1);
                char* op2 = getOperand(p->u.binop.op2);
                fprintf(fp, "%s := %s / %s\n", resOp, op1, op2);
                break;
            }
            case ADD_2_VAL:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                fprintf(fp, "%s := &%s\n", leftOp, rightOp);
                break;
            }
            case VAL_2_VAL:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                fprintf(fp, "%s := *%s\n", leftOp, rightOp);
                break;
            }
            case VAL_2_ADD:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                fprintf(fp, "*%s := %s\n", leftOp, rightOp);
                break;
            }
            case GOTO:{
                char* op = getOperand(p->u.one.op);
                fprintf(fp, "GOTO %s\n", op);
                break;
            }
            case IF_GOTO:{
                char* leftOp = getOperand(p->u.logic.left);
                char* rightOp = getOperand(p->u.logic.right);
                char* destOp = getOperand(p->u.logic.dest);
                fprintf(fp, "IF %s %s %s GOTO %s\n", leftOp, p->u.logic.relop, rightOp, destOp);
                break;
            }
            case RETURN:{
                char* op = getOperand(p->u.one.op);
                fprintf(fp, "RETURN %s\n", op);
                break;
            }
            case DEC:{
                char* op = getOperand(p->u.dec.op);
                fprintf(fp, "DEC %s %d\n", op, p->u.dec.size);
                break;
            }
            case ARG:{
                char* op = getOperand(p->u.one.op);
                fprintf(fp, "ARG %s\n", op);
                break;
            }
            case CALL:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                fprintf(fp, "%s := CALL %s\n", leftOp, rightOp);
                break;
            }
            case PARAM:{
                char* op = getOperand(p->u.one.op);
                fprintf(fp, "PARAM %s\n", op);
                break;
            }
            case READ:{
                char* op = getOperand(p->u.one.op);
                fprintf(fp, "READ %s\n", op);
                break;
            }
            case WRITE:{
                char* op = getOperand(p->u.one.op);
                fprintf(fp, "WRITE %s\n", op);
                break;
            }
            default:{
                fprintf(stderr, "Unexpected error in printObjectCode() objectCode.c\n");
                exit(-1);
            }
        }
        p = p->next;
    }
    fclose(fp);
}