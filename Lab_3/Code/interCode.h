#ifndef __INTERCODE_H__
#define __INTERCODE_H__

#include "stdio.h"
#include "hashTable.h"
#include "syntaxTree.h"

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
// typedef struct InterCodes_* InterCodes;

typedef enum {
    VARIABLE, FUNCTION,
    CONSTANT, ADDRESS,
    LABEL_OP, TEMP_OP
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

struct InterCode_ {
    ICType kind;
    union{
        struct {
            Operand right, left;
        } assign;
        struct {
            Operand res, op1, op2;
        } binop;
        struct {
            Operand label;
        } dec;
    } u;
    InterCode prev;
    InterCode next;
};

// struct InterCodes_ {
//     InterCode code;
//     InterCodes prev;
//     InterCodes next;
// };

// 中间代码双向链表
extern InterCode head;
extern InterCode tail;

void translate_Exp(TreeNode *node, Operand place);
Operand new_temp();
Operand new_label();
void translate_Stmt(TreeNode* node, Operand place);
void translate_Cond(TreeNode* node, Operand label_true, Operand label_false);
void translate_Args(TreeNode* node, Operand* arg_list);
void translate_CompSt(TreeNode* node);
void translate_StmtList(TreeNode *node);
void translate_DefList(TreeNode *node);
void translate_Def(TreeNode *node);
void translate_DecList(TreeNode *node);
void translate_Dec(TreeNode *node);
void insertCode(InterCode code);
void printCode();

// 逻辑运算统一处理
void translate_logical(TreeNode* node, Operand place);
#endif