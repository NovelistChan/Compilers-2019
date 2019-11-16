#include"interCode.h"
#include"hashTable.h"
#include"syntaxTree.h"
#include"stdio.h"
#include"stdlib.h"

InterCode head = NULL;
InterCode tail = NULL;

Operand new_label() {

}

Operand new_temp() {

}

void translate_Exp(TreeNode *node, Operand place) {
    TreeNode *child = node->children;
    // x := y
    //  INT
    if  (!strcmp(child->name, "INT") && child->next == NULL) {

    }
    // ID
    else if  (!strcmp(child->name, "ID") && child->next == NULL) {
        Operand op = (Operand)malloc(sizeof(struct Operand_));
        op->kind = VARIABLE;
        op->next = NULL;
        op->u.value = child->attr.val_str;
        InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
        code->next = NULL;
        code->prev = NULL;
        code->kind = ASSIGN;
        code->u.assign.left = place;
        code->u.assign.right = op;
    } 
    // Exp1 ASSIGNOP Exp2
    else if  (!strcmp(child->next->name, "ASSIGNOP")) {

    }
    // Exp1 PLUS Exp2
    else if  (!strcmp(child->next->name, "PLUS")) {

    }
    // Exp1 MINUS Exp2
    else if  (!strcmp(child->next->name, "MINUS")) {

    }
    // Exp1 STAR Exp2
    else if  (!strcmp(child->next->name, "STAR")) {

    }
    // Exp1 DIV Exp2
    else if  (!strcmp(child->next->name, "DIV")) {

    }
    // Exp1 RELOP Exp2
    else if  (!strcmp(child->next->name, "RELOP")) {

    }
    // Exp1 AND Exp2
    else if  (!strcmp(child->next->name, "AND")) {

    }
    // Exp1 OR Exp2
    else if  (!strcmp(child->next->name, "OR")) {

    }
    // MINUS Exp1
    else if  (!strcmp(child->name, "MINUS")) {

    }
    // NOT Exp1
    else if  (!strcmp(child->name, "NOT")) {
        
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