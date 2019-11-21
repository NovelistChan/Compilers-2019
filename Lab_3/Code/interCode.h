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
        int var_no;     // label and temp
        int value;      // int constant
        char* varName;  // variable, function name, address
    } u;
    Operand next;
};

struct InterCode_ {
    ICType kind;
    union{
        struct {
            Operand op;
        } dec;
        struct {
            Operand right, left;
        } assign;
        struct {
            Operand res, op1, op2;
        } binop;
        struct {
            Operand dest, left, right;
            char* relop;
        } logic;
    } u;
    InterCode prev;
    InterCode next;
};

// struct InterCodes_ {
//     InterCode code;
//     InterCodes prev;
//     InterCodes next;
// };

// 带头节点的双向循环链表
extern InterCode head;
//extern InterCode tail;

Operand new_temp();
Operand new_label();
Operand new_constant(int val);
Operand new_operand(OpType kind, char* name);

InterCode new_oneOp_interCode(ICType kind, Operand op);
InterCode new_twoOp_interCode(ICType kind, Operand left, Operand right);
InterCode new_threeOp_interCode(ICType kind, Operand res, Operand op1, Operand op2);
InterCode new_logic_goto_interCode(Operand left, Operand right, Operand dest, char* relop);

void jointCode(InterCode dst, InterCode src);

InterCode translate_Exp(TreeNode *node, Operand place);
InterCode translate_Stmt(TreeNode* node);
InterCode translate_Cond(TreeNode* node, Operand label_true, Operand label_false);
InterCode translate_Args(TreeNode* node, Operand arg_list);
InterCode translate_CompSt(TreeNode* node);
void translate_StmtList(TreeNode *node);
void translate_DefList(TreeNode *node);
void translate_Def(TreeNode *node);
void translate_DecList(TreeNode *node);
void translate_Dec(TreeNode *node);
void insertCode(InterCode code);
void printCode();
void initial();
// 逻辑运算统一处理
void translate_logical(TreeNode* node, Operand place);
#endif