#include"interCode.h"
#include"hashTable.h"
#include"syntaxTree.h"
#include"semantic.h"
#include<stdlib.h>
#include<string.h>
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

void generateInterCode(){
    
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

int intlen(unsigned num){
    int n = 0;
    do{
        n++;
        num /= 10;
    }while(num);
    return n;
}

char* getOperand(Operand op){
    switch(op->kind){
        case VARIABLE: case FUNCTION:{
            return op->u.varName;
        }
        case CONSTANT:{
            char* ret = (char*)malloc(1+intlen(op->u.value)+1);
            sprintf(ret, "#%d", op->u.value);
            return ret;
        }
        case ADDRESS:{
            char* ret = (char*)malloc(1+strlen(op->u.varName)+1);
            sprintf(ret, "&%s", op->u.varName);
            return ret;
        }
        case LABEL_OP:{
            char* ret = (char*)malloc(5+intlen(op->u.var_no)+1);
            sprintf(ret, "label%d", op->u.var_no);
            return ret;
        }
        case TEMP_OP:{
            char* ret = (char*)malloc(1+intlen(op->u.var_no)+1);
            sprintf(ret, "t%d", op->u.var_no);
            return ret;
        }
        default:{
            fprintf(stderr, "Unexpected error in getOperand() insertCode.c\n");
            exit(-1);
        }
    }
}

InterCode new_oneOp_interCode(ICType kind, Operand op){
    InterCode oneCode = (InterCode)malloc(sizeof(struct InterCode_));
    oneCode->kind = kind;
    oneCode->next = oneCode->prev = NULL;
    oneCode->u.one.op = op;
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

InterCode new_dec_interCode(Operand op, int size){
    InterCode decCode = (InterCode)malloc(sizeof(struct InterCode_));
    decCode->kind = DEC;
    decCode->next = decCode->prev = NULL;
    decCode->u.dec.op = op;
    decCode->u.dec.size = size;
    return decCode;
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
    InterCode p = head->next;
    while(p != head){
        switch(p->kind){
            case LABEL: {
                char* op = getOperand(p->u.one.op);
                char* res = (char*)malloc(6+strlen(op)+2+1);
                sprintf(res, "LABEL %s :", op);

                printf("%s\n", res);
                break;
            }
            case FUNCTION:{
                char* op = getOperand(p->u.one.op);
                char* res = (char*)malloc(9+strlen(op)+2+1);
                sprintf(res, "FUNCTION %s :", op);

                printf("%s\n", res);
                break;
            }
            case ASSIGN:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                char* res = (char*)malloc(strlen(leftOp)+4+strlen(rightOp)+1);
                sprintf(res, "%s := %s", leftOp, rightOp);
                
                printf("%s\n", res);
                break;
            }
            case ADD:{
                char* resOp = getOperand(p->u.binop.res);
                char* op1 = getOperand(p->u.binop.op1);
                char* op2 = getOperand(p->u.binop.op2);
                char* res = (char*)malloc(strlen(resOp)+4+strlen(op1)+3+strlen(op2)+1);
                sprintf(res, "%s := %s + %s", resOp, op1, op2);
                
                printf("%s\n", res);
                break;
            }
            case MIN:{
                char* resOp = getOperand(p->u.binop.res);
                char* op1 = getOperand(p->u.binop.op1);
                char* op2 = getOperand(p->u.binop.op2);
                char* res = (char*)malloc(strlen(resOp)+4+strlen(op1)+3+strlen(op2)+1);
                sprintf(res, "%s := %s - %s", resOp, op1, op2);

                printf("%s\n", res);
                break;
            }
            case MUL:{
                char* resOp = getOperand(p->u.binop.res);
                char* op1 = getOperand(p->u.binop.op1);
                char* op2 = getOperand(p->u.binop.op2);
                char* res = (char*)malloc(strlen(resOp)+4+strlen(op1)+3+strlen(op2)+1);
                sprintf(res, "%s := %s * %s", resOp, op1, op2);

                printf("%s\n", res);
                break;
            }
            case DIV:{
                char* resOp = getOperand(p->u.binop.res);
                char* op1 = getOperand(p->u.binop.op1);
                char* op2 = getOperand(p->u.binop.op2);
                char* res = (char*)malloc(strlen(resOp)+4+strlen(op1)+3+strlen(op2)+1);
                sprintf(res, "%s := %s / %s", resOp, op1, op2);

                printf("%s\n", res);
                break;
            }
            case ADD_2_VAL:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                char* res = (char*)malloc(strlen(leftOp)+5+strlen(rightOp)+1);
                sprintf(res, "%s := &%s", leftOp, rightOp);

                printf("%s\n", res);
                break;
            }
            case VAL_2_VAL:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                char* res = (char*)malloc(strlen(leftOp)+5+strlen(rightOp)+1);
                sprintf(res, "%s := *%s", leftOp, rightOp);

                printf("%s\n", res);
                break;
            }
            case VAL_2_ADD:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                char* res = (char*)malloc(1+strlen(leftOp)+4+strlen(rightOp)+1);
                sprintf(res, "*%s := %s", leftOp, rightOp);

                printf("%s\n", res);
                break;
            }
            case GOTO:{
                char* op = getOperand(p->u.one.op);
                char* res = (char*)malloc(5+strlen(op)+1);
                sprintf(res, "GOTO %s", op);
                
                printf("%s\n", res);
                break;
            }
            case IF_GOTO:{
                char* leftOp = getOperand(p->u.logic.left);
                char* rightOp = getOperand(p->u.logic.right);
                char* destOp = getOperand(p->u.logic.dest);
                char* res = (char*)malloc(3+strlen(leftOp)+1+strlen(p->u.logic.relop)+1+strlen(rightOp)+6+strlen(destOp)+1);
                sprintf(res, "IF %s %s %s GOTO %s", leftOp, p->u.logic.relop, rightOp, destOp);

                printf("%s\n", res);
                break;
            }
            case RETURN:{
                char* op = getOperand(p->u.one.op);
                char* res = (char*)malloc(7+strlen(op)+1);
                sprintf(res, "RETURN %s", op);
                
                printf("%s\n", res);
                break;
            }
            case DEC:{
                char* op = getOperand(p->u.dec.op);
                char* res = (char*)malloc(4+strlen(op)+1+intlen(p->u.dec.size)+1);
                sprintf(res, "DEC %s %d", op, p->u.dec.size);

                printf("%s\n", res);
                break;
            }
            case ARG:{
                char* op = getOperand(p->u.one.op);
                char* res = (char*)malloc(4+strlen(op)+1);
                sprintf(res, "ARG %s", op);

                printf("%s\n", res);
                break;
            }
            case CALL:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                char* res = (char*)malloc(strlen(leftOp)+9+strlen(rightOp)+1);
                sprintf(res, "%s := CALL %s", leftOp, rightOp);

                printf("%s\n", res);
                break;
            }
            case PARAM:{
                char* op = getOperand(p->u.one.op);
                char* res = (char*)malloc(6+strlen(op)+1);
                sprintf(res, "PARAM %s", op);

                printf("%s\n", res);
                break;
            }
            case READ:{
                char* op = getOperand(p->u.one.op);
                char* res = (char*)malloc(5+strlen(op)+1);
                sprintf(res, "READ %s", op);

                printf("%s\n", res);
                break;
            }
            case WRITE:{
                char* op = getOperand(p->u.one.op);
                char* res = (char*)malloc(6+strlen(op)+1);
                sprintf(res, "WRITE %s", op);

                printf("%s\n", res);
                break;
            }
            default:{
                fprintf(stderr, "Unexpected error in printCode() insertCode.c\n");
                exit(-1);
            }
        }
    }
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
        Operand t1 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        Operand addr1 = new_operand(ADDRESS, "t"+t1->u.var_no);

        Type dstType = Exp(child);    // the type of ARRAY
        Operand sizeOp = new_constant(getTypeSize(dstType->u.array.elem));
        child = child->next->next;
        Operand t2 = new_temp();
        InterCode code2 = translate_Exp(child, t2);
        Operand t3 = new_temp();
        InterCode code3 = new_threeOp_interCode(MUL, t3, sizeOp, t2);
        Operand t4 = new_temp();
        InterCode code4 = new_threeOp_interCode(ADD, t4, addr1, t3);
        jointCode(code1, code2);
        jointCode(code2, code3);
        jointCode(code3, code4);
        jointCode(code4, new_twoOp_interCode(VAL_2_VAL, place, t4));
        return code1;
    }
    // Exp -> Exp DOT ID
    else if(!strcmp(child->next->name, "DOT")){
        // Exp -> ID ? TODO optimize
        Operand t1 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        Operand addr1 = new_operand(ADDRESS, "t"+t1->u.var_no);

        Type dstType = Exp(child);    // the type of STRUCTURE

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
    TreeNode* child = node->children->next;
    InterCode retCode = translate_DefList(child);
    jointCode(retCode, translate_StmtList(child->next));
    return retCode;
}

InterCode translate_StmtList(TreeNode *node) {
    TreeNode* child = node->children;
    InterCode retCode = NULL;
    if(child){
        retCode = translate_Stmt(child);
        jointCode(retCode, translate_StmtList(child->next));
    }
    return retCode;
}

InterCode translate_DefList(TreeNode *node) {
    TreeNode* child = node->children;
    InterCode retCode = NULL;
    if(child){
        retCode = translate_Def(child);
        jointCode(retCode, translate_DefList(child->next));
    }
    return retCode;
}

InterCode translate_Def(TreeNode *node) {
    TreeNode* child = node->children->next;
    return translate_DecList(child);
}

InterCode translate_DecList(TreeNode *node) {
    TreeNode* child = node->children;

    InterCode retCode = translate_Dec(child);
    child = child->next;
    if(child){
        jointCode(retCode, translate_DecList(child->next));
    }
    return retCode;
}

InterCode translate_Dec(TreeNode *node) {
    TreeNode* child = node->children;
    TreeNode* p = child->children;
    while(strcmp(p->name, "ID")){
        p = p->children;
    }
    char* varName = p->attr.val_str;
    Type type = hashCheck(varName)->info->type;
    Operand op = new_operand(VARIABLE, varName);

    if(type->kind == BASIC){
        child = child->next;
        if(child){
            child = child->next; // skip ASSIGNOP
            Operand t1 = new_temp();
            InterCode code1 = translate_Exp(child, t1);
            InterCode code2 = new_twoOp_interCode(ASSIGN, op, t1);
            jointCode(code1, code2);
            return code1;
        }
        return NULL;
    }
    return new_dec_interCode(op, getTypeSize(type));
}