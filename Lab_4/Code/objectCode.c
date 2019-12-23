#include"interCode.h"
#include"hashTable.h"
#include"syntaxTree.h"
#include"semantic.h"
#include<stdlib.h>
#include<string.h>
#include"objectCode.h"

int getReg(FILE* fp, Operand op) {    // VARIABLE, ADDRESS, ADDTOVAL; TEMP_OP; CONSTANT;
    // TODO: 局部寄存器分配算法
    char* varName = getOperand(op);
    for(int i=0;i<32;i++){
        if(regs[i]->var!=NULL && !strcmp(varName, regs[i]->var->varName)){

        }
    }

    VarDescription p = varHead->next;
    while(p){
        if(!strcmp(varName, p->varName)){
            AddressDescription q = p->addrDescription;
            while(q){
                if(q->addrType == REG){
                    return q->addr.regNo;
                }
            }
            // TODO not in regs
        }
    }
}

void initialVarList(){
    varHead = (VarDescription)malloc(sizeof(struct VarDescription_));
    varHead->varName = NULL;
    varHead->addrDescription = NULL;
    varHead->next = NULL;

    // TODO initial all variable with their addr
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
                int leftNo = -1;
                if(p->u.assign.left->kind == ADDTOVAL){
                    if(p->u.assign.right->kind == ADDTOVAL){  // *x = *y
                        int rightNo = getReg(fp, p->u.assign.right);
                        Operand t1 = new_temp();
                        int tempNo = getReg(fp, t1);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[rightNo]->name);
                        leftNo = getReg(fp, p->u.assign.left);
                        fprintf(fp, "sw %s, 0(%s)\n", regs[tempNo]->name, regs[leftNo]->name);
                    }else{  // *x = y
                        int rightNo = getReg(fp, p->u.assign.right);
                        leftNo = getReg(fp, p->u.assign.left);
                        fprintf(fp, "sw %s, 0(%s)\n", regs[rightNo]->name, regs[leftNo]->name);
                    }
                }else{
                    if(p->u.assign.right->kind == ADDTOVAL){  // x = *y
                        int rightNo = getReg(fp, p->u.assign.right);
                        leftNo = getReg(fp, p->u.assign.left);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[leftNo]->name, regs[rightNo]->name);
                    }else if(p->u.assign.right->kind == CONSTANT){  // x = #k
                        leftNo = getReg(fp, p->u.assign.left);
                        fprintf(fp, "ori %s, $0, %d\n", regs[leftNo]->name, p->u.assign.right->u.value);
                    }else{  // x = y
                        int rightNo = getReg(fp, p->u.assign.right);
                        leftNo = getReg(fp, p->u.assign.left);
                        fprintf(fp, "addu %s, %s, $0\n", regs[leftNo]->name, regs[rightNo]->name);
                    }
                }
                break;
            }
            case ADD:{
                if(p->u.binop.res->kind == ADDTOVAL){
                    fprintf(stderr, "Unexpected kind in case ADD, printObjectCode, objectCode.c\n");
                    exit(-1);
                }

                int resNo = -1 ,opNo1 = -1, opNo2 = -1;

                if(p->u.binop.op1->kind == CONSTANT){
                    opNo2 = getReg(fp, p->u.binop.op2);
                    resNo = getReg(fp, p->u.binop.res);
                    fprintf(fp, "addi %s, %s, %d\n", regs[resNo]->name, regs[opNo2]->name, p->u.binop.op1->u.value);
                }else if(p->u.binop.op2->kind == CONSTANT){
                    opNo1 = getReg(fp, p->u.binop.op1);
                    resNo = getReg(fp, p->u.binop.res);
                    fprintf(fp, "addi %s, %s, %d\n", regs[resNo]->name, regs[opNo1]->name, p->u.binop.op2->u.value);
                }else{
                    if(p->u.binop.op1->kind != ADDTOVAL && p->u.binop.op2->kind != ADDTOVAL){
                        opNo1 = getReg(fp, p->u.binop.op1);
                        opNo2 = getReg(fp, p->u.binop.op2);
                    }else{
                        if(p->u.binop.op1->kind == ADDTOVAL){
                            opNo1 = getReg(fp, p->u.binop.op1);
                            Operand t1 = new_temp();
                            int tempNo = getReg(fp, t1);
                            fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo1]->name);
                            opNo1 = tempNo;
                        } 
                        if(p->u.binop.op2->kind == ADDTOVAL){
                            opNo2 = getReg(fp, p->u.binop.op1);
                            Operand t1 = new_temp();
                            int tempNo = getReg(fp, t1);
                            fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo2]->name);
                            opNo2 = tempNo;
                        }
                    }
                    resNo = getReg(fp, p->u.binop.res);
                    fprintf(fp, "add %s, %s, %s\n", regs[resNo]->name, regs[opNo1]->name, regs[opNo2]->name);
                }
                break;
            }
            case MIN:{
                if(p->u.binop.res->kind == ADDTOVAL){
                    fprintf(stderr, "Unexpected kind in case MIN, printObjectCode, objectCode.c\n");
                    exit(-1);
                }

                int resNo = -1 ,opNo1 = -1, opNo2 = -1;

                if(p->u.binop.op2->kind == CONSTANT){
                    opNo1 = getReg(fp, p->u.binop.op1);
                    resNo = getReg(fp, p->u.binop.res);
                    fprintf(fp, "addi %s, %s, %d\n", regs[resNo]->name, regs[opNo1]->name, -p->u.binop.op2->u.value);
                }else{
                    if(p->u.binop.op1->kind != ADDTOVAL && p->u.binop.op2->kind != ADDTOVAL){
                        opNo1 = getReg(fp, p->u.binop.op1);
                        opNo2 = getReg(fp, p->u.binop.op2);
                    }else{
                        if(p->u.binop.op1->kind == ADDTOVAL){
                            opNo1 = getReg(fp, p->u.binop.op1);
                            Operand t1 = new_temp();
                            int tempNo = getReg(fp, t1);
                            fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo1]->name);
                            opNo1 = tempNo;
                        } 
                        if(p->u.binop.op2->kind == ADDTOVAL){
                            opNo2 = getReg(fp, p->u.binop.op1);
                            Operand t1 = new_temp();
                            int tempNo = getReg(fp, t1);
                            fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo2]->name);
                            opNo2 = tempNo;
                        }
                    }
                    resNo = getReg(fp, p->u.binop.res);
                    fprintf(fp, "sub %s, %s, %s\n", regs[resNo]->name, regs[opNo1]->name, regs[opNo2]->name);
                }
                break;
            }
            case MUL:{
                if(p->u.binop.res->kind == ADDTOVAL){
                    fprintf(stderr, "Unexpected kind in case MUL, printObjectCode, objectCode.c\n");
                    exit(-1);
                }

                int resNo = -1 ,opNo1 = -1, opNo2 = -1;
                if(p->u.binop.op1->kind != ADDTOVAL && p->u.binop.op2->kind != ADDTOVAL){
                    opNo1 = getReg(fp, p->u.binop.op1);
                    opNo2 = getReg(fp, p->u.binop.op2);
                }else{
                    if(p->u.binop.op1->kind == ADDTOVAL){
                        opNo1 = getReg(fp, p->u.binop.op1);
                        Operand t1 = new_temp();
                        int tempNo = getReg(fp, t1);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo1]->name);
                        opNo1 = tempNo;
                    } 
                    if(p->u.binop.op2->kind == ADDTOVAL){
                        opNo2 = getReg(fp, p->u.binop.op1);
                        Operand t1 = new_temp();
                        int tempNo = getReg(fp, t1);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo2]->name);
                        opNo2 = tempNo;
                    }
                }
                resNo = getReg(fp, p->u.binop.res);
                fprintf(fp, "mul %s, %s, %s\n", regs[resNo]->name, regs[opNo1]->name, regs[opNo2]->name);
                break;
            }
            case DIV:{
                if(p->u.binop.res->kind == ADDTOVAL){
                    fprintf(stderr, "Unexpected kind in case DIV, printObjectCode, objectCode.c\n");
                    exit(-1);
                }

                int resNo = -1 ,opNo1 = -1, opNo2 = -1;
                if(p->u.binop.op1->kind != ADDTOVAL && p->u.binop.op2->kind != ADDTOVAL){
                    opNo1 = getReg(fp, p->u.binop.op1);
                    opNo2 = getReg(fp, p->u.binop.op2);
                }else{
                    if(p->u.binop.op1->kind == ADDTOVAL){
                        opNo1 = getReg(fp, p->u.binop.op1);
                        Operand t1 = new_temp();
                        int tempNo = getReg(fp, t1);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo1]->name);
                        opNo1 = tempNo;
                    } 
                    if(p->u.binop.op2->kind == ADDTOVAL){
                        opNo2 = getReg(fp, p->u.binop.op1);
                        Operand t1 = new_temp();
                        int tempNo = getReg(fp, t1);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo2]->name);
                        opNo2 = tempNo;
                    }
                }
                resNo = getReg(fp, p->u.binop.res);

                fprintf(fp, "div %s, %s\n", regs[opNo1]->name, regs[opNo2]->name);
                fprintf(fp, "mflo %s\n", regs[resNo]->name);
                break;
            }
            case ADD_2_VAL:{
                int leftOp = getReg(fp, p->u.assign.left);
                fprintf(fp, "lui %s, %s\n", regs[leftOp]->name, getOperand(p->u.assign.right));
                break;
            }
            case VAL_2_VAL:{
                int leftOp = getReg(fp, p->u.assign.left);
                int rightOp = getReg(fp, p->u.assign.right);
                fprintf(fp, "lw %s, 0(%s)\n", regs[leftOp]->name, regs[rightOp]->name);
                break;
            }
            case VAL_2_ADD:{
                int leftOp = getReg(fp, p->u.assign.left);
                int rightOp = getReg(fp, p->u.assign.right);
                fprintf(fp, "sw %s, 0(%s)\n", regs[rightOp]->name, regs[leftOp]->name);
                break;
            }
            case GOTO:{
                char* op = getOperand(p->u.one.op);
                fprintf(fp, "j %s\n", op);
                break;
            }
            case IF_GOTO:{
                char* destOp = getOperand(p->u.logic.dest);
                char* relop = p->u.logic.relop;
                int leftOp = getReg(fp, p->u.logic.left);
                int rightOp = getReg(fp, p->u.assign.right);
                // fprintf(fp, "IF %s %s %s GOTO %s\n", leftOp, p->u.logic.relop, rightOp, destOp);
                if(!strcmp(relop, "==")){
                    fprintf(fp, "beq %s, %s, %s\n", regs[leftOp]->name, regs[rightOp]->name, destOp);
                }else if(!strcmp(relop, "!=")){
                    fprintf(fp, "bne %s, %s, %s\n", regs[leftOp]->name, regs[rightOp]->name, destOp);
                }else if(!strcmp(relop, ">")){
                    fprintf(fp, "bgt %s, %s, %s\n", regs[leftOp]->name, regs[rightOp]->name, destOp);
                }else if(!strcmp(relop, "<")){
                    fprintf(fp, "blt %s, %s, %s\n", regs[leftOp]->name, regs[rightOp]->name, destOp);
                }else if(!strcmp(relop, ">=")){
                    fprintf(fp, "bge %s, %s, %s\n", regs[leftOp]->name, regs[rightOp]->name, destOp);
                }else if(!strcmp(relop, "<=")){
                    fprintf(fp, "ble %s, %s, %s\n", regs[leftOp]->name, regs[rightOp]->name, destOp);
                }else{
                    fprintf(stderr, "Unexpected relop in printObjectCode in objectCode.c\n");
                }
                break;
            }
            case RETURN:{
                int retOp = getReg(fp, p->u.one.op);
                fprintf(fp, "move $v0, %s\n", regs[retOp]->name);
                fprintf(fp, "jr $ra");
                break;
            }
            // TODO
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