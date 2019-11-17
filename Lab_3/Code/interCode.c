#include"interCode.h"
#include"hashTable.h"
#include"syntaxTree.h"
#include"stdio.h"
#include"stdlib.h"


int tempCnt = 0;
int labelCnt = 0;

void initial() {
    head = NULL;
    tail = NULL;
    tempCnt = 0;
    labelCnt = 0;
}

Operand new_label() {
    Operand label = (Operand)malloc(sizeof(struct Operand_));
    label->kind = LABEL_OP;
    label->next = NULL;
    label->u.var_no = labelCnt;
    labelCnt++;
}

Operand new_temp() {
    Operand temp = (Operand)malloc(sizeof(struct Operand_));
    temp->kind = TEMP_OP;
    temp->next = NULL;
    temp->u.var_no = tempCnt;
    tempCnt++;
}

void printCode() {

}

void insertCode(InterCode code) {
    if (head == NULL) {
        head = code;
        tail = code;
        head->prev = tail;
        head->next = tail;
        tail->prev = head;
        tail->next = head;
    } else {
        InterCode p = head;
        while (p != tail) {
            p = p->next;
        }
        p->next = code;
        code->prev = p;
        code->next = head;
        head->prev = code;
        tail = code;
    }
}

void translate_logical(TreeNode* node, Operand place) {
     Operand label1 = new_label();
        Operand label2 = new_label();
        InterCode code0 = (InterCode)malloc(sizeof(struct InterCode_));
        Operand zero = (Operand)malloc(sizeof(struct Operand_));
        zero->kind = CONSTANT;
        zero->next = NULL;
        zero->u.var_no = 0;
        code0->next = NULL; code0->prev = NULL;
        code0->kind = ASSIGN;
        code0->u.assign.left = place; 
        code0->u.assign.right = zero;
        insertCode(code0);
        translate_Cond(node, label1, label2);
        InterCode code1 = (InterCode)malloc(sizeof(struct InterCode_));
        code1->kind = DEC; code1->next = NULL; code1->prev = NULL;
        code1->u.dec.label = label1;
        insertCode(code1);
        InterCode code2 = (InterCode)malloc(sizeof(struct InterCode_));
        code2->kind = ASSIGN; code2->next = NULL; code2->prev = NULL;
        Operand one = (Operand)malloc(sizeof(struct Operand_));
        one->kind = CONSTANT; one->next = NULL; one->u.var_no = 1;
        code2->u.assign.left = place; code2->u.assign.right = one;
        insertCode(code2);
        InterCode code3 = (InterCode)malloc(sizeof(struct InterCode_));
        code3->kind = DEC; code3->next = NULL; code3->prev = NULL;
        code3->u.dec.label = label2;
        insertCode(code3);
}

void translate_Exp(TreeNode *node, Operand place) {
    TreeNode *child = node->children;
    // x := y
    //  INT
    if  (!strcmp(child->name, "INT") && child->next == NULL) {
        Operand op = (Operand)malloc(sizeof(struct Operand_));
        op->next = CONSTANT;
        op->next = NULL;
        op->u.var_no = child->attr.val_int;
        InterCode code = (InterCode)malloc(sizeof(struct Operand_));
        code->next = NULL;  code->prev = NULL;
        code->kind = ASSIGN;
        code->u.assign.left = place;
        code->u.assign.right = op;
        insertCode(code);
    }
    // ID
    else if  (!strcmp(child->name, "ID") && child->next == NULL) {
        Operand op = (Operand)malloc(sizeof(struct Operand_));
        op->kind = VARIABLE;
        op->next = NULL;
        op->u.value = child->attr.val_str;
        InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
        code->next = NULL; code->prev = NULL;
        code->kind = ASSIGN;
        code->u.assign.left = place;
        code->u.assign.right = op;
        insertCode(code);
    } 
    // Exp1 ASSIGNOP Exp2
    else if  (!strcmp(child->next->name, "ASSIGNOP")) {
        Operand op1 = (Operand)malloc(sizeof(struct Operand_));
        op1->kind = VARIABLE;
        op1->next = NULL;
        op1->u.var_no = child->children->attr.val_str;
        Operand temp = new_temp();
        translate_Exp(child->next->next, temp);
        InterCode code1 = (InterCode)malloc(sizeof(struct InterCode_));
        code1->next = NULL;  code1->prev = NULL;
        code1->kind = ASSIGN;
        code1->u.assign.left = op1;
        code1->u.assign.right = temp;
        InterCode code2 = (InterCode)malloc(sizeof(struct InterCode_));
        code2->next = NULL;  code2->prev = NULL;
        code2->kind = ASSIGN;
        code2->u.assign.left = place;
        code2->u.assign.right = op1;
        insertCode(code1);
        insertCode(code2);
    }
    // Exp1 PLUS Exp2
    else if  (!strcmp(child->next->name, "PLUS")) {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        translate_Exp(child, t1);
        translate_Exp(child->next->next, t2);
        InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
        code->next = NULL;  code->prev = NULL;
        code->kind = ADD;
        code->u.binop.op1 = t1;
        code->u.binop.op2 = t2;
        code->u.binop.res = place;
        InterCode(code);
    }
    // Exp1 MINUS Exp2
    else if  (!strcmp(child->next->name, "MINUS")) {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        translate_Exp(child, t1);
        translate_Exp(child->next->next, t2);
        InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
        code->next = NULL;  code->prev = NULL;
        code->kind = MIN;
        code->u.binop.op1 = t1;
        code->u.binop.op2 = t2;
        code->u.binop.res = place;
        InterCode(code);
    }
    // Exp1 STAR Exp2
    else if  (!strcmp(child->next->name, "STAR")) {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        translate_Exp(child, t1);
        translate_Exp(child->next->next, t2);
        InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
        code->next = NULL;  code->prev = NULL;
        code->kind = MUL;
        code->u.binop.op1 = t1;
        code->u.binop.op2 = t2;
        code->u.binop.res = place;
        InterCode(code);
    }
    // Exp1 DIV Exp2
    else if  (!strcmp(child->next->name, "DIV")) {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        translate_Exp(child, t1);
        translate_Exp(child->next->next, t2);
        InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
        code->next = NULL;  code->prev = NULL;
        code->kind = DIV;
        code->u.binop.op1 = t1;
        code->u.binop.op2 = t2;
        code->u.binop.res = place;
        InterCode(code);
    }
    // Exp1 RELOP Exp2
    else if  (!strcmp(child->next->name, "RELOP")) {
       translate_logical(node, place);
    }
    // Exp1 AND Exp2
    else if  (!strcmp(child->next->name, "AND")) {
        translate_logical(node, place);
    }
    // Exp1 OR Exp2
    else if  (!strcmp(child->next->name, "OR")) {
        translate_logical(node, place);
    }
    // NOT Exp1
    else if  (!strcmp(child->name, "NOT")) {
        translate_logical(node, place);
    }
    // MINUS Exp1
    else if  (!strcmp(child->name, "MINUS")) {
        Operand temp = new_temp();
        translate_Exp(child, temp);
        InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
        Operand zero = (Operand)malloc(sizeof(struct Operand_));
        zero->kind = CONSTANT; zero->next = NULL;  zero->u.var_no = 0;
        code->kind = MIN; code->next = NULL; code->prev = NULL;
        code->u.binop.op1 = zero;
        code->u.binop.op2 = temp;
        code->u.binop.res = place;
        insertCode(code);
    }
 }


void translate_Stmt(TreeNode* node, Operand place) {

}

void translate_Cond(TreeNode* node, Operand label_true, Operand label_false) {

}

void translate_Args(TreeNode* node, Operand* arg_list) {

}

void translate_CompSt(TreeNode* node) {

}

void translate_StmtList(TreeNode *node) {

}

void translate_DefList(TreeNode *node) {

}

void translate_Def(TreeNode *node) {

}

void translate_DecList(TreeNode *node) {

}

void translate_Dec(TreeNode *node) {

}