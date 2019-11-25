#ifndef __INTERCODE_H__
#define __INTERCODE_H__

#include "stdio.h"
#include "hashTable.h"
#include "syntaxTree.h"

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
// typedef struct InterCodes_* InterCodes;

typedef enum {
    VARIABLE, FUNCNAME,
    CONSTANT, ADDRESS,
    ADDTOVAL,
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
        } one;
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
        struct {
            Operand op;
            int size;
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

// 带头节点的双向循环链表
InterCode head;
//extern InterCode tail;

Operand new_temp();
Operand new_label();
Operand new_constant(int val);
Operand new_operand(OpType kind, char* name);
char* getOperand(Operand op);

InterCode new_oneOp_interCode(ICType kind, Operand op);
InterCode new_twoOp_interCode(ICType kind, Operand left, Operand right);
InterCode new_threeOp_interCode(ICType kind, Operand res, Operand op1, Operand op2);
InterCode new_logic_goto_interCode(Operand left, Operand right, Operand dest, char* relop);
InterCode new_dec_interCode(Operand op, int size);

InterCode jointCode(InterCode dst, InterCode src);

//TODO encapsulation of translate_Exp(), to reduce the interCode, aim to reduce Exp -> ID || INT

InterCode translate_Program(TreeNode* node);
InterCode translate_ExtDefList(TreeNode* node);
InterCode translate_ExtDef(TreeNode* node);

InterCode translate_FunDec(TreeNode* node);

InterCode translate_CompSt(TreeNode* node);
InterCode translate_StmtList(TreeNode *node);
InterCode translate_Stmt(TreeNode* node);

InterCode translate_DefList(TreeNode *node);
InterCode translate_Def(TreeNode *node);
InterCode translate_DecList(TreeNode *node);
InterCode translate_Dec(TreeNode *node);

InterCode translate_Exp(TreeNode *node, Operand place);
InterCode translate_Cond(TreeNode* node, Operand label_true, Operand label_false);
InterCode translate_Args(TreeNode* node, Operand arg_list);

void insertCode(InterCode code);
void printCode();
void initial();
void generateInterCode();
// 逻辑运算统一处理
InterCode translate_logical(TreeNode* node, Operand place);
#endif