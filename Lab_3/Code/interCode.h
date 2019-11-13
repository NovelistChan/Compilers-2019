#ifndef __INTERCODE_H__
#define __INTERCODE_H__

#include "stdio.h"
#include "hashTable.h"
#include "syntaxTree.h"

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
typedef struct InterCodes_* InterCodes;

typedef enum {
    VARIABLE, FUNCTION,
    CONSTANT, ADDRESS 
} OpType;

typedef enum {
    LABEL, FUNCTION, ASSIGN,  ADD,
    MIN, MUL, DIV, 
    ADD_2_VAL, // x = &y 将地址赋给变量
    VAL_2_VAL, // x = *y 取值赋给变量
    VAL_2_ADD,  // *x = y 将值填入地址
    GOTO, IF_GOTO, RETURN,
    DEC, ARG, CALL, PARAM,
    READ, WRITE
} ICType;

struct Operand_ {
    OpType kind;
    union {
        int var_no;
        char* value;
    } u;
    Operand next;
};

struct InterCode {
    ICType kind;
    union{
        struct {
            Operand right, left;
        } assign;
        struct {
            Operand res, op1, op2;
        } binop;
    } u;
};

struct InterCodes_ {
    InterCode code;
    InterCodes prev;
    InterCodes next;
};

// 中间代码双相链表
extern InterCodes head;
extern InterCodes tail;

void translate_Exp(TreeNode *Node);
Operand new_temp();
void translate_Stmt(TreeNode* Node);
void translate_Cond(TreeNode* Node, Operand label_true, Operand label_false);
void translate_Args(TreeNode* Node, Operand* arg_list);

#endif