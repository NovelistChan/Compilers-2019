#include"interCode.h"
#include"hashTable.h"
#include"syntaxTree.h"
#include"semantic.h"
#include<stdlib.h>
#include<string.h>
#include"objectCode.h"

int activeRegs[10];
int activeNum = 0;
int paramNum = 0;

int getReg(Operand op) {    // VARIABLE, ADDRESS, ADDTOVAL; TEMP_OP; CONSTANT;
    char* varName = NULL;
    if(op->kind == TEMP_OP || op->kind == CONSTANT){
        varName = getOperand(op);
    }else if(op->kind == VARIABLE || op->kind == ADDRESS || op->kind == ADDTOVAL){
        varName = op->u.varName;
    }else{
        fprintf(stderr, "Unexpected kind error in getReg(), objectCode.c\n");
        exit(-1);
    }

    VarDescription var = varHead->next;
    while (var != NULL) {
        if (!strcmp(varName, var->varName))
            break;
        var = var->next;
    }

    if (var != NULL) { // in varHead
        if(var->addrDescription[0] != NULL){
            return var->addrDescription[0]->regNo;
        }
    }

    AddressDescription newAddr = (AddressDescription)malloc(sizeof(union AddressDescription_));
    int regNo = -1;
    for (int i = 8; i <= 25; i++) {// t0~t9 s0~s7
        if (regs[i]->ifFree) {
            regNo = i;
            break;
        }
    }
    if (regNo == -1) {// all regs in use
        int farthest = -99999;
        for (int i = 8; i <= 25; i++) {
            if (!regs[i]->ifFree) {
                regs[i]->dirty++;
                farthest = farthest > regs[i]->dirty ? farthest:regs[i]->dirty;
            }
        }
        for (int i = 8; i <= 25; i++) {
            if (!regs[i]->ifFree) {
                if (regs[i]->dirty == farthest) {
                    char *spillName = regs[i]->var->varName;
                    if (spillName[0] == '#') {
                        VarDescription p = varHead;
                        while (p->next) { // delete CONSTANT
                            if (!strcmp(p->next->varName, spillName)) {
                                VarDescription q = p->next;
                                p->next = q->next;
                                q->next = NULL;
                                free(q);
                                break;
                            }
                            p = p->next;
                        }
                    } else {
                        VarDescription p = varHead;
                        while(p->next) {
                            if(!strcmp(p->next->varName, spillName)) {
                                VarDescription q = p->next;
                                q->addrDescription[0] = NULL; 
                                if  (q->addrDescription[2] == NULL) {
                                    q->addrDescription[1] = (AddressDescription)malloc(sizeof(union AddressDescription_));
                                    fprintf(fp, "addi $sp, $sp, -4\n");
                                    fprintf(fp, "sw %s, 0($sp)\n", regs[i]->name);
                                    nowOffset -= 4;
                                    q->addrDescription[1]->offset = nowOffset;
                                }
                                break;
                            }
                            p = p->next;
                        }
                    }
                    regNo = i;
                    break;
                }
            }
        }
    }
    newAddr->regNo = regNo;
    regs[regNo]->ifFree = false;
    regs[regNo]->dirty = 0;
    
    if (op->kind == CONSTANT)
        fprintf(fp, "ori %s, $0, %d\n", regs[regNo]->name, op->u.value);

    if(var != NULL){
        if(var->addrDescription[2] != NULL){    // segment
            fprintf(fp, "lui %s, %s\n", regs[regNo]->name, var->addrDescription[2]->name);
        }else if(var->addrDescription[1] != NULL){  // stack
            fprintf(fp, "lw %s, %d($fp)\n", regs[regNo]->name, var->addrDescription[1]->offset);
        }
        regs[regNo]->var = var;
    }else { // a new var
        var = (VarDescription)malloc(sizeof(struct VarDescription_));
        var->varName = varName;
        var->addrDescription[0] = newAddr;
        var->addrDescription[1] = var->addrDescription[2] = NULL;
        
        var->next = varHead->next;
        varHead->next = var;

        regs[regNo]->var = var;
    }
    return regNo;
}

void initialVarList(){
    fprintf(fp, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(fp, "_ret: .asciiz \"\\n\"\n");

    varHead = (VarDescription)malloc(sizeof(struct VarDescription_));
    varHead->varName = NULL;
    varHead->addrDescription[0] = varHead->addrDescription[1] = varHead->addrDescription[2] = NULL;
    varHead->next = NULL;

    InterCode p = head->next;
    while(p != head){
        if(p->kind == DEC){
            char* varName = p->u.dec.op->u.varName;
            fprintf(fp, "%s: .space %d\n", varName, p->u.dec.size);
            VarDescription var = (VarDescription)malloc(sizeof(struct VarDescription_));
            var->varName = varName;
            AddressDescription addr = (AddressDescription)malloc(sizeof(union AddressDescription_));
            addr->name = varName;
            var->addrDescription[2] = addr;
            var->addrDescription[0] = var->addrDescription[1] = NULL;
            var->next = NULL;

            var->next = varHead->next;
            varHead->next = var;
        }
        p = p->next;
    }

    fprintf(fp, ".globl main\n");
}

void initialRegisters() {
    for (int i = 0; i < 32; i++) {
        regs[i] = (RegDescription)malloc(sizeof(struct RegDescription_));
        regs[i]->var = NULL;
        regs[i]->ifFree = true;
        regs[i]->dirty = 0;
        regs[i]->name = (char*)malloc(6);
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
    strcpy(regs[30]->name, "$fp");
    strcpy(regs[31]->name, "$ra");
}

void printObjectCode(char *fileName) {
    fp = fopen(fileName, "w");

    fprintf(fp, ".data\n");
    initialVarList();
    initialRegisters();
    fprintf(fp, ".text\n");

    fprintf(fp, "read:\n");
    fprintf(fp, "ori $v0, $0, 4\n");
    fprintf(fp, "lui $a0, _prompt\n");
    fprintf(fp, "syscall\n");
    fprintf(fp, "ori $v0, $0, 5\n");
    fprintf(fp, "syscall\n");
    fprintf(fp, "jr $ra\n");

    fprintf(fp, "write:\n");
    fprintf(fp, "ori $v0, $0, 1\n");
    fprintf(fp, "syscall\n");
    fprintf(fp, "ori $v0, $0, 4\n");
    fprintf(fp, "lui $a0, _ret\n");
    fprintf(fp, "syscall\n");
    fprintf(fp, "addu $v0, $0, $0\n");
    fprintf(fp, "jr $ra\n");

    InterCode p = head->next;
    while(p != head){
        switch(p->kind){
            case LABEL:{
                char* labelName = getOperand(p->u.one.op);
                fprintf(fp, "%s:\n", labelName);
                break;
            }
            case FUNCTION:{
                nowFunc = getOperand(p->u.one.op);
                fprintf(fp, "%s:\n", nowFunc);

                // save $ra, $fp in stack
                fprintf(fp, "addi $sp, $sp, -4\n");
                fprintf(fp, "sw $ra, 0($sp)\n");
                fprintf(fp, "addi $sp, $sp, -4\n");
                fprintf(fp, "sw $fp, 0($sp)\n");
                fprintf(fp, "addi $fp, $sp, 8\n");

                nowOffset = -8;

                if(!strcmp(nowFunc, "main")){
                    break;
                }

                // save $s0~$s7 in stack
                for(int i=16;i<=23;i++){
                    fprintf(fp, "addi $sp, $sp, -4\n");
                    fprintf(fp, "sw %s, 0($sp)\n", regs[i]->name);
                    nowOffset -= 4;
                }

                break;
            }
            case ASSIGN:{
                int leftNo = -1;
                if(p->u.assign.left->kind == ADDTOVAL){
                    if(p->u.assign.right->kind == ADDTOVAL){  // *x = *y
                        int rightNo = getReg(p->u.assign.right);
                        Operand t1 = new_temp();
                        int tempNo = getReg(t1);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[rightNo]->name);
                        leftNo = getReg(p->u.assign.left);
                        fprintf(fp, "sw %s, 0(%s)\n", regs[tempNo]->name, regs[leftNo]->name);
                    }else{  // *x = y
                        int rightNo = getReg(p->u.assign.right);
                        leftNo = getReg(p->u.assign.left);
                        fprintf(fp, "sw %s, 0(%s)\n", regs[rightNo]->name, regs[leftNo]->name);
                    }
                }else{
                    if(p->u.assign.right->kind == ADDTOVAL){  // x = *y
                        int rightNo = getReg(p->u.assign.right);
                        leftNo = getReg(p->u.assign.left);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[leftNo]->name, regs[rightNo]->name);
                    }else if(p->u.assign.right->kind == CONSTANT){  // x = #k
                        leftNo = getReg(p->u.assign.left);
                        fprintf(fp, "ori %s, $0, %d\n", regs[leftNo]->name, p->u.assign.right->u.value);
                    }else{  // x = y
                        int rightNo = getReg(p->u.assign.right);
                        leftNo = getReg(p->u.assign.left);
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
                    opNo2 = getReg(p->u.binop.op2);
                    resNo = getReg(p->u.binop.res);
                    fprintf(fp, "addi %s, %s, %d\n", regs[resNo]->name, regs[opNo2]->name, p->u.binop.op1->u.value);
                }else if(p->u.binop.op2->kind == CONSTANT){
                    opNo1 = getReg(p->u.binop.op1);
                    resNo = getReg(p->u.binop.res);
                    fprintf(fp, "addi %s, %s, %d\n", regs[resNo]->name, regs[opNo1]->name, p->u.binop.op2->u.value);
                }else{
                    if(p->u.binop.op1->kind != ADDTOVAL && p->u.binop.op2->kind != ADDTOVAL){
                        opNo1 = getReg(p->u.binop.op1);
                        opNo2 = getReg(p->u.binop.op2);
                    }else{
                        if(p->u.binop.op1->kind == ADDTOVAL){
                            opNo1 = getReg(p->u.binop.op1);
                            Operand t1 = new_temp();
                            int tempNo = getReg(t1);
                            fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo1]->name);
                            opNo1 = tempNo;
                        } 
                        if(p->u.binop.op2->kind == ADDTOVAL){
                            opNo2 = getReg(p->u.binop.op1);
                            Operand t1 = new_temp();
                            int tempNo = getReg(t1);
                            fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo2]->name);
                            opNo2 = tempNo;
                        }
                    }
                    resNo = getReg(p->u.binop.res);
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
                    opNo1 = getReg(p->u.binop.op1);
                    resNo = getReg(p->u.binop.res);
                    fprintf(fp, "addi %s, %s, %d\n", regs[resNo]->name, regs[opNo1]->name, -p->u.binop.op2->u.value);
                }else{
                    if(p->u.binop.op1->kind != ADDTOVAL && p->u.binop.op2->kind != ADDTOVAL){
                        opNo1 = getReg(p->u.binop.op1);
                        opNo2 = getReg(p->u.binop.op2);
                    }else{
                        if(p->u.binop.op1->kind == ADDTOVAL){
                            opNo1 = getReg(p->u.binop.op1);
                            Operand t1 = new_temp();
                            int tempNo = getReg(t1);
                            fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo1]->name);
                            opNo1 = tempNo;
                        } 
                        if(p->u.binop.op2->kind == ADDTOVAL){
                            opNo2 = getReg(p->u.binop.op1);
                            Operand t1 = new_temp();
                            int tempNo = getReg(t1);
                            fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo2]->name);
                            opNo2 = tempNo;
                        }
                    }
                    resNo = getReg(p->u.binop.res);
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
                    opNo1 = getReg(p->u.binop.op1);
                    opNo2 = getReg(p->u.binop.op2);
                }else{
                    if(p->u.binop.op1->kind == ADDTOVAL){
                        opNo1 = getReg(p->u.binop.op1);
                        Operand t1 = new_temp();
                        int tempNo = getReg(t1);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo1]->name);
                        opNo1 = tempNo;
                    } 
                    if(p->u.binop.op2->kind == ADDTOVAL){
                        opNo2 = getReg(p->u.binop.op1);
                        Operand t1 = new_temp();
                        int tempNo = getReg(t1);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo2]->name);
                        opNo2 = tempNo;
                    }
                }
                resNo = getReg(p->u.binop.res);
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
                    opNo1 = getReg(p->u.binop.op1);
                    opNo2 = getReg(p->u.binop.op2);
                }else{
                    if(p->u.binop.op1->kind == ADDTOVAL){
                        opNo1 = getReg(p->u.binop.op1);
                        Operand t1 = new_temp();
                        int tempNo = getReg(t1);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo1]->name);
                        opNo1 = tempNo;
                    } 
                    if(p->u.binop.op2->kind == ADDTOVAL){
                        opNo2 = getReg(p->u.binop.op1);
                        Operand t1 = new_temp();
                        int tempNo = getReg(t1);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[opNo2]->name);
                        opNo2 = tempNo;
                    }
                }
                resNo = getReg(p->u.binop.res);

                fprintf(fp, "div %s, %s\n", regs[opNo1]->name, regs[opNo2]->name);
                fprintf(fp, "mflo %s\n", regs[resNo]->name);
                break;
            }
            case ADD_2_VAL:{
                int leftNo = getReg(p->u.assign.left);
                fprintf(fp, "lui %s, %s\n", regs[leftNo]->name, getOperand(p->u.assign.right));
                break;
            }
            case VAL_2_VAL:{
                int leftNo = getReg(p->u.assign.left);
                int rightNo = getReg(p->u.assign.right);
                fprintf(fp, "lw %s, 0(%s)\n", regs[leftNo]->name, regs[rightNo]->name);
                break;
            }
            case VAL_2_ADD:{
                int leftNo = getReg(p->u.assign.left);
                int rightNo = getReg(p->u.assign.right);
                fprintf(fp, "sw %s, 0(%s)\n", regs[rightNo]->name, regs[leftNo]->name);
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

                int leftNo = -1, rightNo = -1;
                if(p->u.logic.left->kind != ADDTOVAL && p->u.logic.right->kind != ADDTOVAL){
                    leftNo = getReg(p->u.logic.left);
                    rightNo = getReg(p->u.logic.right);
                }else{
                    if(p->u.logic.left->kind == ADDTOVAL){
                        leftNo = getReg(p->u.binop.op1);
                        Operand t1 = new_temp();
                        int tempNo = getReg(t1);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[leftNo]->name);
                        leftNo = tempNo;
                    } 
                    if(p->u.logic.right->kind == ADDTOVAL){
                        rightNo = getReg(p->u.binop.op1);
                        Operand t1 = new_temp();
                        int tempNo = getReg(t1);
                        fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[rightNo]->name);
                        rightNo = tempNo;
                    }
                }
                // fprintf(fp, "IF %s %s %s GOTO %s\n", leftNo, p->u.logic.relop, rightNo, destOp);
                if(!strcmp(relop, "==")){
                    fprintf(fp, "beq %s, %s, %s\n", regs[leftNo]->name, regs[rightNo]->name, destOp);
                }else if(!strcmp(relop, "!=")){
                    fprintf(fp, "bne %s, %s, %s\n", regs[leftNo]->name, regs[rightNo]->name, destOp);
                }else if(!strcmp(relop, ">")){
                    // fprintf(fp, "bgt %s, %s, %s\n", regs[leftNo]->name, regs[rightNo]->name, destOp);
                    fprintf(fp, "slt $1, %s, %s\n", regs[leftNo]->name, regs[rightNo]->name);
                    fprintf(fp, "bne $1, $0, %s\n", destOp);
                }else if(!strcmp(relop, "<")){
                    // fprintf(fp, "blt %s, %s, %s\n", regs[leftNo]->name, regs[rightNo]->name, destOp);
                    fprintf(fp, "sgt $1, %s, %s\n", regs[leftNo]->name, regs[rightNo]->name);
                    fprintf(fp, "bne $1, $0, %s\n", destOp);
                }else if(!strcmp(relop, ">=")){
                    // fprintf(fp, "bge %s, %s, %s\n", regs[leftNo]->name, regs[rightNo]->name, destOp);
                    fprintf(fp, "sle $1, %s, %s\n", regs[leftNo]->name, regs[rightNo]->name);
                    fprintf(fp, "bne $1, $0, %s\n", destOp);
                }else if(!strcmp(relop, "<=")){
                    // fprintf(fp, "ble %s, %s, %s\n", regs[leftNo]->name, regs[rightNo]->name, destOp);
                    fprintf(fp, "sge $1, %s, %s\n", regs[leftNo]->name, regs[rightNo]->name);
                    fprintf(fp, "bne $1, $0, %s\n", destOp);
                }else{
                    fprintf(stderr, "Unexpected relop in printObjectCode in objectCode.c\n");
                }
                break;
            }
            case RETURN:{
                int retNo = getReg(p->u.one.op);
                if(p->u.one.op->kind == ADDTOVAL){
                    Operand t1 = new_temp();
                    int tempNo = getReg(t1);
                    fprintf(fp, "lw %s, 0(%s)\n", regs[tempNo]->name, regs[retNo]->name);
                    retNo = tempNo;
                }
                // fprintf(fp, "move $v0, %s\n", regs[retNo]->name);
                fprintf(fp, "addu $v0, %s, $0\n", regs[retNo]->name);

                if(strcmp(nowFunc, "main")&&strcmp(nowFunc, "read")&&strcmp(nowFunc, "write")){
                    for(int i=16;i<=23;i++){
                        fprintf(fp, "lw %s, %d($fp)\n", regs[i]->name, -8-4*(i-15));
                    }
                    fprintf(fp, "lw $ra, -4($fp)\n");
                    fprintf(fp, "lw $fp, -8($fp)\n");
                }

                fprintf(fp, "jr $ra\n");
                
                nowOffset = 0;
                break;
            }
            
            case DEC:{
                // do nothing
                break;
            }
            case ARG:{
                // save all active regs{$t0 ~ $t9} in stack
                for(int i=8;i<=15;i++){ // $t0~$t7
                    if(!regs[i]->ifFree){
                        activeRegs[activeNum++] = i;
                        fprintf(fp, "addi $sp, $sp, -4\n");
                        fprintf(fp, "sw %s, 0($sp)\n", regs[i]->name);
                        nowOffset -= 4;
                    }
                }
                for(int i=24;i<=25;i++){ // $t8~$t9
                    if(!regs[i]->ifFree){
                        activeRegs[activeNum++] = i;
                        fprintf(fp, "addi $sp, $sp, -4\n");
                        fprintf(fp, "sw %s, 0($sp)\n", regs[i]->name);
                        nowOffset -= 4;
                    }
                }

                // VARIABLE, CONSTANT, ADDRESS(?)
                paramNum = 0;
                while(p->kind == ARG){
                    Operand op = p->u.one.op;
                    if(op->kind == ADDRESS){
                        fprintf(stderr, "Unexpected kind in case ARG, printObjectCode(), objectCode.c\n");
                        exit(-1);
                    }

                    int opNo = getReg(op);
                    switch(paramNum){
                        case 0:{
                            fprintf(fp, "addu $a0, %s, $0\n", regs[opNo]->name);
                            break;
                        }
                        case 1:{
                            fprintf(fp, "addu $a1, %s, $0\n", regs[opNo]->name);
                            break;
                        }
                        case 2:{
                            fprintf(fp, "addu $a2, %s, $0\n", regs[opNo]->name);
                            break;
                        }
                        case 3:{
                            fprintf(fp, "addu $a3, %s, $0\n", regs[opNo]->name);
                            break;
                        }
                        default:{
                            fprintf(fp, "addi $sp, $sp, -4\n");
                            fprintf(fp, "sw %s, 0($sp)\n", regs[opNo]->name);
                            nowOffset -= 4;
                        }
                    }

                    paramNum ++;
                    p = p->next;
                }
                p = p->prev;
                break;
            }
            case CALL:{
                // 0 params call
                if(paramNum == 0){
                    // save all active regs{$t0 ~ $t9} in stack
                    activeNum = 0;
                    for(int i=8;i<=15;i++){ // $t0~$t7
                        if(!regs[i]->ifFree){
                            activeRegs[activeNum++] = i;
                            fprintf(fp, "addi $sp, $sp, -4\n");
                            fprintf(fp, "sw %s, 0($sp)\n", regs[i]->name);
                            nowOffset -= 4;
                        }
                    }
                    for(int i=24;i<=25;i++){ // $t8~$t9
                        if(!regs[i]->ifFree){
                            activeRegs[activeNum++] = i;
                            fprintf(fp, "addi $sp, $sp, -4\n");
                            fprintf(fp, "sw %s, 0($sp)\n", regs[i]->name);
                            nowOffset -= 4;
                        }
                    }
                }

                char* funcName = getOperand(p->u.assign.right);
                fprintf(fp, "jal %s\n", funcName);

                // recover all saved regs{$t0 ~ $t9} from stack
                int paramsOffset = 0;
                if(paramNum > 5){
                    paramsOffset = 4*(paramNum - 5);
                    fprintf(fp, "addi $sp, $sp, %d\n", paramsOffset);
                }
                paramNum = 0;
                for(int i=activeNum-1;i>=0;i--){
                    fprintf(fp, "addi $sp, $sp, 4\n");
                    fprintf(fp, "lw %s, 0($sp)\n", regs[activeRegs[i]]->name);
                }
                activeNum = 0;

                int leftNo = getReg(p->u.assign.left);
                // fprintf(fp, "move %s, $v0\n", regs[leftNo]->name);
                fprintf(fp, "addu %s, $v0, $0\n", regs[leftNo]->name);
                break;
            }
            case PARAM:{
                /*
                Operand op = p->u.one.op;
                if(op->kind != VARIABLE){
                    fprintf(stderr, "Unexpected kind in case PARAM, printObjectCode(), objectCode.c\n");
                    exit(-1);
                }

                if(paramNum == 0){
                    fprintf(stderr, "Unexpected params error in case PARAM, printObjectCode(), objectCode.c\n");
                    exit(-1);
                }

                VarDescription newVar = (VarDescription)malloc(sizeof(struct VarDescription_));
                newVar->varName = op->u.varName;
                newVar->next = NULL;
                newVar->addrDescription[2] = NULL;

                AddressDescription newAddr = (AddressDescription)malloc(sizeof(union AddressDescription_));
                switch(paramNum){
                    case 1:{
                        newAddr->regNo = 4; // $a0
                        break;
                    }
                    case 2:{
                        newAddr->regNo = 5; // $a1
                        break;
                    }
                    case 3:{
                        newAddr->regNo = 6; // $a2
                        break;
                    }
                    case 4:{
                        newAddr->regNo = 7; // $a3
                        break;
                    }
                    default:{
                        newAddr->offset = 4*(paramNum - 4);
                    }
                }

                if(paramNum <= 4){
                    newVar->addrDescription[0] = newAddr;
                    newVar->addrDescription[1] = NULL;
                }else{
                    newVar->addrDescription[1] = newAddr;
                    newVar->addrDescription[0] = NULL;
                }
                paramNum --;
                break;
                */
                
                int paramsNum = 0;
                while(p->kind == PARAM){
                    Operand op = p->u.one.op;
                    if(op->kind != VARIABLE){
                        fprintf(stderr, "Unexpected kind in case PARAM, printObjectCode(), objectCode.c\n");
                        exit(-1);
                    }

                    VarDescription newVar = (VarDescription)malloc(sizeof(struct VarDescription_));
                    newVar->varName = op->u.varName;
                    newVar->next = NULL;
                    newVar->addrDescription[2] = NULL;

                    AddressDescription newAddr = (AddressDescription)malloc(sizeof(union AddressDescription_));
                    switch(paramsNum){
                        case 0:{
                            newAddr->regNo = 4; // $a0
                            break;
                        }
                        case 1:{
                            newAddr->regNo = 5; // $a1
                            break;
                        }
                        case 2:{
                            newAddr->regNo = 6; // $a2
                            break;
                        }
                        case 3:{
                            newAddr->regNo = 7; // $a3
                            break;
                        }
                        default:{
                            newAddr->offset = 4*(paramsNum - 4);
                        }
                    }

                    if(paramsNum < 4){
                        newVar->addrDescription[0] = newAddr;
                        newVar->addrDescription[1] = NULL;
                    }else{
                        newVar->addrDescription[1] = newAddr;
                        newVar->addrDescription[0] = NULL;
                    }

                    paramsNum ++;
                    p = p->next;
                }
                p = p->prev;
                break;
            }
            case READ:{
                fprintf(fp, "jal read\n");
                
                int opNo = getReg(p->u.one.op);
                fprintf(fp, "addu %s, $v0, $0\n", regs[opNo]->name);
                break;
            }
            case WRITE:{
                fprintf(fp, "jal write\n");
                
                int opNo = getReg(p->u.one.op);
                fprintf(fp, "addu %s, $v0, $0\n", regs[opNo]->name);
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