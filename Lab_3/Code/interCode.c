#include"interCode.h"
#include"hashTable.h"
#include"syntaxTree.h"
#include"stdio.h"
#include"stdlib.h"
#include"interCode.h"


int tempCnt = 0;
int labelCnt = 0;

void initial() {
    /*
    head = NULL;
    tail = NULL;
    */

    head = (InterCode)malloc(sizeof(struct InterCode_));
    head->next = head;
    head->prev = head;
    tempCnt = 0;
    labelCnt = 0;
}

Operand new_label() {
    Operand label = (Operand)malloc(sizeof(struct Operand_));
    label->kind = LABEL_OP;
    label->next = NULL;
    label->u.var_no = labelCnt;
    labelCnt++;
    return label;
}

Operand new_temp() {
    Operand temp = (Operand)malloc(sizeof(struct Operand_));
    temp->kind = TEMP_OP;
    temp->next = NULL;
    temp->u.var_no = tempCnt;
    tempCnt++;
    return temp;
}

Operand new_constant(int val){
    Operand con = (Operand)malloc(sizeof(struct Operand_));
    con->kind = CONSTANT;
    con->u.value = val;
    con->next = NULL;
    return con;
}

Operand new_operand(OpType kind, char* name){
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = kind;
    op->u.varName = name;
    op->next = NULL;
    return op;
}

InterCode new_oneOp_interCode(ICType kind, Operand op){
    InterCode oneCode = (InterCode)malloc(sizeof(struct InterCode_));
    oneCode->kind = kind;
    oneCode->next = oneCode->prev = NULL;
    oneCode->u.dec.op = op;
    return oneCode;
}

InterCode new_twoOp_interCode(ICType kind, Operand left, Operand right){
    InterCode twoCode = (InterCode)malloc(sizeof(struct InterCode_));
    twoCode->kind = kind;
    twoCode->next = twoCode->prev = NULL;
    twoCode->u.assign.left = left;
    twoCode->u.assign.right = right;
    return twoCode;
}

InterCode new_threeOp_interCode(ICType kind, Operand res, Operand op1, Operand op2){
    InterCode threeCode = (InterCode)malloc(sizeof(struct InterCode_));
    threeCode->kind = kind;
    threeCode->next = threeCode->prev = NULL;
    threeCode->u.binop.op1 = op1;
    threeCode->u.binop.op2 = op2;
    threeCode->u.binop.res = res;
    return threeCode;
}

InterCode new_logic_goto_interCode(Operand left, Operand right, Operand dest, char* relop){
    InterCode logicCode = (InterCode)malloc(sizeof(struct InterCode_));
    logicCode->kind = IF_GOTO;
    logicCode->next = logicCode->prev = NULL;
    logicCode->u.logic.left = left;
    logicCode->u.logic.right = right;
    logicCode->u.logic.dest = dest;
    logicCode->u.logic.relop = relop;
    return logicCode;
}

void jointCode(InterCode dst, InterCode src){
    InterCode p = dst;
    while(p->next){
        p = p->next;
    }
    p->next = src;
    src->prev = p;
}

void printCode() {

}

void insertCode(InterCode code) {
    head->prev->next = code;
    InterCode p = code;
    while(p->next != NULL){
        p = p->next;
    }
    p->next = head;
    head->prev = p;
    /*
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
    */
}

int getTypeSize(Type type){
    if(type->kind == BASIC){
        return 4;
    }
    if(type->kind == ARRAY){
        return getTypeSize(type->u.array.elem)*type->u.array.size;
    }
    if(type->kind == STRUCTURE){
        int ret = 0;
        FieldList p = type->u.structure;
        while(p){
            ret += getTypeSize(p->type);
            p = p->tail;
        }
        return ret;
    }
    fprintf(stderr, "Unexpected error in getTypeSize(), insertCode.c\n");
    exit(-1);
}

void translate_logical(TreeNode* node, Operand place) {
    Operand label1 = new_label();
    Operand label2 = new_label();
    Operand zeroOp = new_constant(0);
    Operand oneOp = new_constant(1);
    InterCode code0 = new_twoOp_interCode(ASSIGN, place, zeroOp);
    InterCode code1 = translate_Cond(node, label1, label2);
    InterCode code2 = new_oneOp_interCode(LABEL, label1);
    jointCode(code2, new_twoOp_interCode(ASSIGN, place, oneOp));
    jointCode(code0, code1);
    jointCode(code1, code2);
    jointCode(code2, new_oneOp_interCode(LABEL, label2));
    return code0;
}

InterCode translate_Exp(TreeNode *node, Operand place) {
    TreeNode *child = node->children;
    // x := y
    //  INT
    if  (!strcmp(child->name, "INT") && child->next == NULL) {
        Operand cons = new_constant(child->attr.val_int);
        InterCode retCode = new_twoOp_interCode(ASSIGN, place, cons);
        return retCode;
    }
    // ID
    else if  (!strcmp(child->name, "ID") && child->next == NULL) {
        Operand var = new_operand(VARIABLE, child->attr.val_str);
        InterCode retCode = new_twoOp_interCode(ASSIGN, place, var);
        return retCode;
    } 
    // Exp1 ASSIGNOP Exp2
    else if  (!strcmp(child->next->name, "ASSIGNOP")) {
        if(!strcmp(child->children->name, "ID")){
            Operand var = new_operand(VARIABLE, child->children->attr.val_str);
            Operand temp = new_temp();
            InterCode code1 = translate_Exp(child->next->next, temp);
            InterCode code2 = new_twoOp_interCode(ASSIGN, var, temp);
            jointCode(code2, new_twoOp_interCode(ASSIGN, place, var));
            jointCode(code1, code2);
            return code1;
        }else if(!strcmp(child->children->name, "Exp")){
            // TODO address use temp!
        }else{
            fprintf(stderr, "Unexpected syntax error occurs in ASSIGNOP translate_Exp(), interCode.c\n");
            exit(-1);
    }
    }
    // Exp1 PLUS Exp2
    else if  (!strcmp(child->next->name, "PLUS")) {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        InterCode code2 = translate_Exp(child->next->next, t2);
        InterCode code3 = new_threeOp_interCode(ADD, place, t1, t2);
        jointCode(code1, code2);
        jointCode(code2, code3);
        return code1;
    }
    // Exp1 MINUS Exp2
    else if  (!strcmp(child->next->name, "MINUS")) {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        InterCode code2 = translate_Exp(child->next->next, t2);
        InterCode code3 = new_threeOp_interCode(MIN, place, t1, t2);
        jointCode(code1, code2);
        jointCode(code2, code3);
        return code1;
    }
    // Exp1 STAR Exp2
    else if  (!strcmp(child->next->name, "STAR")) {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        InterCode code2 = translate_Exp(child->next->next, t2);
        InterCode code3 = new_threeOp_interCode(MUL, place, t1, t2);
        jointCode(code1, code2);
        jointCode(code2, code3);
        return code1;
    }
    // Exp1 DIV Exp2
    else if  (!strcmp(child->next->name, "DIV")) {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        InterCode code2 = translate_Exp(child->next->next, t2);
        InterCode code3 = new_threeOp_interCode(DIV, place, t1, t2);
        jointCode(code1, code2);
        jointCode(code2, code3);
        return code1;
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
        InterCode code1 = translate_Exp(child, temp);
        Operand zeroOp = new_constant(0);
        zeroOp->u.value = 0;
        InterCode code2 = new_threeOp_interCode(MIN, place, zeroOp, temp);
        jointCode(code1, code2);
        return code1;
    }
    // Exp -> ID LP RP
    else if (!strcmp(child->name, "ID") && !strcmp(child->next->name, "LP") && !strcmp(child->next->next->name, "RP")) {
        char* funName = child->attr.val_str;
        Operand funOp = new_operand(FUNCTION, funName);
        if(!strcmp(funName, "read")){
            return new_oneOp_interCode(READ, place);
        }else{
            return new_twoOp_interCode(CALL, place, funOp);
        }
        /*
        HashNode checkNode = hashCheck(child->attr.val_str);
        if (checkNode == NULL || checkInfo(checkNode, FUNC) == NULL) {
            // 不在符号表中
            fprintf(stderr, "Unexpected syntax error occurs in FUNCTION translate_Exp(), interCode.c\n");
            exit(-1);
        } else {
           
        }
        */
    } 
    // Exp -> ID LP Args RP
    else if (!strcmp(child->name, "ID") && !strcmp(child->next->next->name, "Args")) {
        char* funName = child->attr.val_str;
        Operand funOp = new_operand(FUNCTION, funName);

        child = child->next->next;  // skip LP
        Operand arg_list = NULL;
        InterCode code1 = translate_Args(child, arg_list);
        if(!strcmp(funName, "write")){
            jointCode(code1, new_oneOp_interCode(WRITE, arg_list));
            return code1;
        }else{
            InterCode p = arg_list;
            InterCode code2 = new_oneOp_interCode(ARG, p);
            p = p->next;
            while(p){
                jointCode(code2, new_oneOp_interCode(ARG, p));
                p = p->next;
            }
            jointCode(code1, code2);
            jointCode(code2, new_twoOp_interCode(CALL, place, funOp));
            return code1;
        }
    }
    // Exp -> LP Exp RP
    else if(!strcmp(child->name, "LP")) {
        return translate_Exp(child->next, place);
    }
    // Exp -> Exp LB Exp RB
    else if(!strcmp(child->next->name, "LB")){
        // TODO
    }
    // Exp -> Exp DOT ID
    else if(!strcmp(child->next->name, "DOT")){
        // Exp -> ID ? TODO optimize
        Operand t1 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        Operand addr1 = new_operand(ADDRESS, "t"+t1->u.var_no);

        TreeNode* p = child->children;
        while(strcmp(p->name, "ID")){
            p = p->children;
        }
        HashNode checkNode = hashCheck(p->name);
        Type dstType = NULL;    // the type of STRUCTURE
        if(checkNode->info->type->kind == ARRAY){
            dstType = checkNode->info->type->u.array.elem;  // only one-dim array
        }else{
            dstType = checkNode->info->type;
        }
        int offset = 0;
        FieldList fieldList = dstType->u.structure;
        child = child->next->next;
        char* attrName = child->attr.val_str;
        while(strcmp(fieldList->name, attrName)){
            offset += getTypeSize(fieldList->type);
            fieldList = fieldList->tail;
        }
        Operand offsetOp = new_constant(offset);
        Operand t2 = new_temp();
        InterCode code2 = new_threeOp_interCode(ADD, t2, addr1, offsetOp);
        jointCode(code2, new_twoOp_interCode(VAL_2_VAL, place, t2));
        jointCode(code1, code2);
        return code1;
    }
    else{
        fprintf(stderr, "Unexpected syntax error occurs in translate_Exp(), interCode.c\n");
        exit(-1);
    }
 }

InterCode translate_Stmt(TreeNode* node) {
    TreeNode *child = node->children;
    if(!strcmp(child->name, "Exp")){
        return translate_Exp(child, NULL);
        // skip SEMI(";")
    }else if(!strcmp(child->name, "CompSt")){
        return translate_CompSt(child);
    }else if(!strcmp(child->name, "RETURN")){
        child = child->next;    // skip RETURN
        Operand t1 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        InterCode code2 = new_oneOp_interCode(RETURN, t1);
        jointCode(code1, code2);
        return code1;
        
    }else if(!strcmp(child->name, "IF")){
        child = child->next->next;  // skip IF and LP
        Operand label1 = new_label();
        Operand label2 = new_label();
        InterCode label1_code = new_oneOp_interCode(LABEL, label1);
        InterCode label2_code = new_oneOp_interCode(LABEL, label2);
        InterCode code1 = translate_Cond(child, label1, label2);
        
        child = child->next->next;  // skip RP
        InterCode code2 = translate_Stmt(child);

        jointCode(code1, label1_code);
        jointCode(label1_code, code2);

        child = child->next;
        if(child){  // ELSE Stmt
            child = child->next;    // skip ELSE
            Operand label3 = new_label();
            InterCode label3_code = new_oneOp_interCode(LABEL, label3);
            InterCode goto_code = new_oneOp_interCode(GOTO, label3);
            InterCode code3 = translate_Stmt(child);
            jointCode(code2, goto_code);
            jointCode(goto_code, label2_code);
            jointCode(label2_code, code3);
            jointCode(code3, label3_code);
        }else{
            jointCode(code2, label2_code);
        }
        return code1;
    }else{
        child = child->next->next;  // skip WHILE and LP
        Operand label1 = new_label();
        Operand label2 = new_label();
        Operand label3 = new_label();
        InterCode label1_code = new_oneOp_interCode(LABEL, label1);
        InterCode label2_code = new_oneOp_interCode(LABEL, label2);
        InterCode label3_code = new_oneOp_interCode(LABEL, label3);
        InterCode goto_code = new_oneOp_interCode(GOTO, label1);
        InterCode code1 = translate_Cond(child, label2, label3);
        child = child->next->next;  // skip RP
        InterCode code2 = translate_Stmt(child);
        jointCode(label1_code, code1);
        jointCode(code1, label2_code);
        jointCode(label2_code, code2);
        jointCode(code2, goto_code);
        jointCode(goto_code, label3_code);
        return label1_code;
    }
}

InterCode translate_Cond(TreeNode* node, Operand label_true, Operand label_false) {
    TreeNode* child = node->children;
    // Exp1 RELOP Exp2
    if  (!strcmp(child->next->name, "RELOP")) {
       Operand t1 = new_temp();
       Operand t2 = new_temp();
       InterCode code1 = translate_Exp(child, t1);
       InterCode code2 = translate_Exp(child->next->next, t2);
       InterCode code3 = new_logic_goto_interCode(t1, t2, label_true, child->next->attr.val_str);
       InterCode code4 = new_oneOp_interCode(GOTO, label_false);
       jointCode(code1, code2);
       jointCode(code2, code3);
       jointCode(code3, code4);
       return code1;
    }
    // Exp1 AND Exp2
    else if  (!strcmp(child->next->name, "AND")) {
        Operand label = new_label();
        InterCode code1 = translate_Cond(child, label, label_false);
        InterCode code2 = translate_Cond(child->next->next, label_true, label_false);
        InterCode code3 = new_oneOp_interCode(LABEL, label);
        jointCode(code1, code3);
        jointCode(code3, code2);
        return code1;
    }
    // Exp1 OR Exp2
    else if  (!strcmp(child->next->name, "OR")) {
        Operand label = new_label();
        InterCode code1 = translate_Cond(child, label_true, label);
        InterCode code2 = translate_Cond(child->next->next, label_true, label_false);
        InterCode code3 = new_oneOp_interCode(LABEL, label);
        jointCode(code1, code3);
        jointCode(code3, code2);
        return code1;
    }
    // NOT Exp1
    else if  (!strcmp(child->name, "NOT")) {
        return translate_Cond(child->next, label_false, label_true);
    }
    else{
        Operand t1 = new_temp();
        Operand zeroOp = new_constant(0);
        InterCode code1 = translate_Exp(node, t1);
        InterCode code2 = new_logic_goto_interCode(t1, zeroOp, label_true, "!=");
        InterCode code3 = new_oneOp_interCode(GOTO, label_false);
        jointCode(code1, code2);
        jointCode(code2, code3);
        return code1;
    }
}

InterCode translate_Args(TreeNode* node, Operand arg_list) {
    TreeNode* child = node->children;
    Operand t1 = new_temp();
    InterCode code1 = translate_Exp(child, t1);
    t1->next = arg_list;
    arg_list = t1;
    child = child->next;
    if(child){
        child = child->next;
        InterCode code2 = translate_Args(child, arg_list);
        jointCode(code1, code2);
        return code1;
    }
    return code1;
}

InterCode translate_CompSt(TreeNode* node) {

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