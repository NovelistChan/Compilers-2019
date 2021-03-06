#include"interCode.h"
#include"hashTable.h"
#include"syntaxTree.h"
#include"semantic.h"
#include<stdlib.h>
#include<string.h>


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
    InterCode interCode = translate_Program(root);
    insertCode(interCode);
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

int intlen(int num){
    int n = 0;
    if(num < 0){
        num = -num;
        n = 1;
    }

    do{
        n++;
        num /= 10;
    }while(num);
    return n;
}

bool isParam(char* varName){
    int i = 0;

    for (; i <TABLE_SIZE; i++) {
        if (hashTable[i]->hashList != NULL) {
            HashNode p = hashTable[i]->hashList;
            while (p) {
                if(p->info->kind == FUNC){
                    FieldList q = p->info->func->paramList;
                    while(q){
                        if(!strcmp(q->name, varName)){
                            return true;
                        }
                        q = q->tail;
                    }
                }

                p = p->next;
            }
        }
    }

    return false;
}

char* getOperand(Operand op){
    switch(op->kind){
        case VARIABLE: case FUNCNAME:{
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
        case ADDTOVAL:{
            char* ret = (char*)malloc(1+strlen(op->u.varName)+1);
            sprintf(ret, "*%s", op->u.varName);
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

InterCode jointCode(InterCode dst, InterCode src){
    if(!dst){
        return src;
    }else if(!src){
        return dst;
    }
    else{
        InterCode p = dst;
        while(p->next){
            p = p->next;
        }
        p->next = src;
        src->prev = p;
        return dst;
    }
}

void printCode(char* fileName) {
    FILE* fp = fopen(fileName, "w");

    InterCode p = head->next;
    while(p != head){
        switch(p->kind){
            case LABEL: {
                char* op = getOperand(p->u.one.op);
                //char* res = (char*)malloc(6+strlen(op)+2+1);
                //sprintf(res, "LABEL %s :", op);
                fprintf(fp, "LABEL %s :\n", op);

                //printf("%s\n", res);
                break;
            }
            case FUNCTION:{
                char* op = getOperand(p->u.one.op);
                //char* res = (char*)malloc(9+strlen(op)+2+1);
                //sprintf(res, "FUNCTION %s :", op);
                fprintf(fp, "FUNCTION %s :\n", op);

                //printf("%s\n", res);
                break;
            }
            case ASSIGN:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                //char* res = (char*)malloc(strlen(leftOp)+4+strlen(rightOp)+1);
                //sprintf(res, "%s := %s", leftOp, rightOp);
                fprintf(fp, "%s := %s\n", leftOp, rightOp);
                
                //printf("%s\n", res);
                break;
            }
            case ADD:{
                char* resOp = getOperand(p->u.binop.res);
                char* op1 = getOperand(p->u.binop.op1);
                char* op2 = getOperand(p->u.binop.op2);
                //char* res = (char*)malloc(strlen(resOp)+4+strlen(op1)+3+strlen(op2)+1);
                //sprintf(res, "%s := %s + %s", resOp, op1, op2);
                fprintf(fp, "%s := %s + %s\n", resOp, op1, op2);
                
                //printf("%s\n", res);
                break;
            }
            case MIN:{
                char* resOp = getOperand(p->u.binop.res);
                char* op1 = getOperand(p->u.binop.op1);
                char* op2 = getOperand(p->u.binop.op2);
                //char* res = (char*)malloc(strlen(resOp)+4+strlen(op1)+3+strlen(op2)+1);
                //sprintf(res, "%s := %s - %s", resOp, op1, op2);
                fprintf(fp, "%s := %s - %s\n", resOp, op1, op2);

                //printf("%s\n", res);
                break;
            }
            case MUL:{
                char* resOp = getOperand(p->u.binop.res);
                char* op1 = getOperand(p->u.binop.op1);
                char* op2 = getOperand(p->u.binop.op2);
                //char* res = (char*)malloc(strlen(resOp)+4+strlen(op1)+3+strlen(op2)+1);
                //sprintf(res, "%s := %s * %s", resOp, op1, op2);
                fprintf(fp, "%s := %s * %s\n", resOp, op1, op2);

                //printf("%s\n", res);
                break;
            }
            case DIV:{
                char* resOp = getOperand(p->u.binop.res);
                char* op1 = getOperand(p->u.binop.op1);
                char* op2 = getOperand(p->u.binop.op2);
                //char* res = (char*)malloc(strlen(resOp)+4+strlen(op1)+3+strlen(op2)+1);
                //sprintf(res, "%s := %s / %s", resOp, op1, op2);
                fprintf(fp, "%s := %s / %s\n", resOp, op1, op2);

                //printf("%s\n", res);
                break;
            }
            case ADD_2_VAL:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                //char* res = (char*)malloc(strlen(leftOp)+5+strlen(rightOp)+1);
                //sprintf(res, "%s := &%s", leftOp, rightOp);
                fprintf(fp, "%s := &%s\n", leftOp, rightOp);

                //printf("%s\n", res);
                break;
            }
            case VAL_2_VAL:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                //char* res = (char*)malloc(strlen(leftOp)+5+strlen(rightOp)+1);
                //sprintf(res, "%s := *%s", leftOp, rightOp);
                fprintf(fp, "%s := *%s\n", leftOp, rightOp);

                //printf("%s\n", res);
                break;
            }
            case VAL_2_ADD:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                //char* res = (char*)malloc(1+strlen(leftOp)+4+strlen(rightOp)+1);
                //sprintf(res, "*%s := %s", leftOp, rightOp);
                fprintf(fp, "*%s := %s\n", leftOp, rightOp);

                //printf("%s\n", res);
                break;
            }
            case GOTO:{
                char* op = getOperand(p->u.one.op);
                //char* res = (char*)malloc(5+strlen(op)+1);
                //sprintf(res, "GOTO %s", op);
                fprintf(fp, "GOTO %s\n", op);
                
                //printf("%s\n", res);
                break;
            }
            case IF_GOTO:{
                char* leftOp = getOperand(p->u.logic.left);
                char* rightOp = getOperand(p->u.logic.right);
                char* destOp = getOperand(p->u.logic.dest);
                //char* res = (char*)malloc(3+strlen(leftOp)+1+strlen(p->u.logic.relop)+1+strlen(rightOp)+6+strlen(destOp)+1);
                //sprintf(res, "IF %s %s %s GOTO %s", leftOp, p->u.logic.relop, rightOp, destOp);
                fprintf(fp, "IF %s %s %s GOTO %s\n", leftOp, p->u.logic.relop, rightOp, destOp);

                //printf("%s\n", res);
                break;
            }
            case RETURN:{
                char* op = getOperand(p->u.one.op);
                //char* res = (char*)malloc(7+strlen(op)+1);
                //sprintf(res, "RETURN %s", op);
                fprintf(fp, "RETURN %s\n", op);
                
                //printf("%s\n", res);
                break;
            }
            case DEC:{
                char* op = getOperand(p->u.dec.op);
                //char* res = (char*)malloc(4+strlen(op)+1+intlen(p->u.dec.size)+1);
                //sprintf(res, "DEC %s %d", op, p->u.dec.size);
                fprintf(fp, "DEC %s %d\n", op, p->u.dec.size);

                //printf("%s\n", res);
                break;
            }
            case ARG:{
                char* op = getOperand(p->u.one.op);
                //char* res = (char*)malloc(4+strlen(op)+1);
                //sprintf(res, "ARG %s", op);
                fprintf(fp, "ARG %s\n", op);

                //printf("%s\n", res);
                break;
            }
            case CALL:{
                char* leftOp = getOperand(p->u.assign.left);
                char* rightOp = getOperand(p->u.assign.right);
                //char* res = (char*)malloc(strlen(leftOp)+9+strlen(rightOp)+1);
                //sprintf(res, "%s := CALL %s", leftOp, rightOp);
                fprintf(fp, "%s := CALL %s\n", leftOp, rightOp);

                //printf("%s\n", res);
                break;
            }
            case PARAM:{
                char* op = getOperand(p->u.one.op);
                //char* res = (char*)malloc(6+strlen(op)+1);
                //sprintf(res, "PARAM %s", op);
                fprintf(fp, "PARAM %s\n", op);

                //printf("%s\n", res);
                break;
            }
            case READ:{
                char* op = getOperand(p->u.one.op);
                //char* res = (char*)malloc(5+strlen(op)+1);
                //sprintf(res, "READ %s", op);
                fprintf(fp, "READ %s\n", op);

                //printf("%s\n", res);
                break;
            }
            case WRITE:{
                char* op = getOperand(p->u.one.op);
                //char* res = (char*)malloc(6+strlen(op)+1);
                //sprintf(res, "WRITE %s", op);
                fprintf(fp, "WRITE %s\n", op);

                //printf("%s\n", res);
                break;
            }
            default:{
                fprintf(stderr, "Unexpected error in printCode() insertCode.c\n");
                exit(-1);
            }
        }
        p = p->next;
    }
    fclose(fp);
}

void insertCode(InterCode code) {
    if(!code)   return;

    head->prev->next = code;
    code->prev = head->prev;
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

Type getFirstAddress(TreeNode* node, Operand firstName, int* offset, InterCode retCode){
    if(strcmp(node->name, "Exp")){
        fprintf(stderr, "Unexpected error node in getExpId(), interCode.c\n");
        exit(-1);
    }
    TreeNode* child = node->children;
    // Exp -> ID
    if(!strcmp(child->name, "ID")&&child->next==NULL){
        firstName->u.varName = child->attr.val_str;
        if(isParam(firstName->u.varName)){
            firstName->kind = VARIABLE; // PARAM ID
        }else{
            firstName->kind = ADDRESS;  // DEC ID
        }
        *offset = 0;
        return hashCheck(child->attr.val_str)->info->type;
    }
    // Exp -> Exp LB Exp RB
    if(!strcmp(child->next->name, "LB")){
        if(child->children->next&& !strcmp(child->children->next->name, "LB")){
            fprintf(stderr, "Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
            exit(-1);
        }

        Type arrayType = getFirstAddress(child, firstName, offset, retCode);
        Operand sizeOp = new_constant(getTypeSize(arrayType->u.array.elem));

        child = child->next->next;
        Operand t1 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        
        if(t1->kind == CONSTANT){
            *offset += sizeOp->u.value * t1->u.value;
            retCode->next = jointCode(retCode->next, code1);
            return arrayType->u.array.elem;
        }

        Operand t2 = new_temp();
        InterCode code2 = new_threeOp_interCode(MUL, t2, sizeOp, t1);
        Operand t3 = new_temp();
        InterCode code3 = NULL;
        if(*offset){
            Operand offsetOp = new_constant(*offset);
            code3 = new_threeOp_interCode(ADD, t3, t2, offsetOp);
            Operand t4 = new_temp();
            InterCode code4 = new_threeOp_interCode(ADD, t4, firstName, t3);
            code3 = jointCode(code3, code4);
            firstName->u.varName = getOperand(t4);
            *offset = 0;
        }else{
            code3 = new_threeOp_interCode(ADD, t3, firstName, t2);
            firstName->u.varName = getOperand(t3);
        }
        firstName->kind = VARIABLE;
        
        code2 = jointCode(code2, code3);
        code1 = jointCode(code1, code2);
        retCode->next = jointCode(retCode->next, code1);
        return arrayType->u.array.elem;
    }
    // Exp -> Exp DOT ID
    if(!strcmp(child->next->name, "DOT")){
        Type arrayType = getFirstAddress(child, firstName, offset, retCode);
        
        int off = 0;
        FieldList fieldList = arrayType->u.structure;
        child = child->next->next;
        char* attrName = child->attr.val_str;
        while(strcmp(fieldList->name, attrName)){
            off += getTypeSize(fieldList->type);
            fieldList = fieldList->tail;
        }
        *offset += off;
        return fieldList->type;
    }
}

InterCode translate_logical(TreeNode* node, Operand place) {
    if(!place){
        return NULL;
    }

    Operand label1 = new_label();
    Operand label2 = new_label();
    Operand zeroOp = new_constant(0);
    Operand oneOp = new_constant(1);
    InterCode code0 = new_twoOp_interCode(ASSIGN, place, zeroOp);
    InterCode code1 = translate_Cond(node, label1, label2);
    InterCode code2 = new_oneOp_interCode(LABEL, label1);
    code2 = jointCode(code2, new_twoOp_interCode(ASSIGN, place, oneOp));
    code2 = jointCode(code2, new_oneOp_interCode(LABEL, label2));
    code1 = jointCode(code1, code2);
    code0 = jointCode(code0, code1);
    return code0;
}

InterCode translate_Exp(TreeNode *node, Operand place) {
    TreeNode *child = node->children;
    // x := y
    //  INT
    if  (!strcmp(child->name, "INT") && child->next == NULL) {
        /*
        Operand cons = new_constant(child->attr.val_int);
        InterCode retCode = NULL;
        if(place){
            retCode = new_twoOp_interCode(ASSIGN, place, cons);
        }
        return retCode;
        */

        if(place){
            place->kind = CONSTANT;
            place->u.value = child->attr.val_int;
        }
        return NULL;
    }
    // ID
    else if  (!strcmp(child->name, "ID") && child->next == NULL) {
        /*
        Operand var = new_operand(VARIABLE, child->attr.val_str);
        InterCode retCode = NULL;
        if(place){
            retCode = new_twoOp_interCode(ASSIGN, place, var);
        }
        return retCode;
        */

        if(place){
            place->kind = VARIABLE;
            place->u.varName = child->attr.val_str;
        }
        return NULL;
    } 
    // Exp1 ASSIGNOP Exp2
    else if  (!strcmp(child->next->name, "ASSIGNOP")) {
        if(!strcmp(child->children->name, "ID")){
            Operand var = new_operand(VARIABLE, child->children->attr.val_str);
            Operand temp = new_temp();
            InterCode code1 = translate_Exp(child->next->next, temp);
            InterCode code2 = new_twoOp_interCode(ASSIGN, var, temp);
            if(place){
                code2 = jointCode(code2, new_twoOp_interCode(ASSIGN, place, var));
            }
            code1 = jointCode(code1, code2);
            return code1;
        }else if(!strcmp(child->children->name, "Exp")){
            Operand leftOp = new_temp();
            InterCode code1 = translate_Exp(child, leftOp);
            child = child->next->next;
            Operand rightOp = new_temp();
            InterCode code2 = translate_Exp(child, rightOp);
            InterCode code3 = new_twoOp_interCode(ASSIGN, leftOp, rightOp);
            if(place){
                code3 = jointCode(code3, new_twoOp_interCode(ASSIGN, place, leftOp));
            }
            code2 = jointCode(code2, code3);
            code1 = jointCode(code1, code2);
            return code1;
        }else{
            fprintf(stderr, "Unexpected syntax error occurs in ASSIGNOP translate_Exp(), interCode.c\n");
            exit(-1);
        }
    }
    // Exp1 PLUS Exp2
    else if  (!strcmp(child->next->name, "PLUS")) {
        /*
        if (!strcmp(child->children->name, "INT")) {
            if(!strcmp(child->next->next->children->name, "INT")) {
                if(place) {
                    place->kind = CONSTANT;
                    place->u.value = child->children->attr.val_int + child->next->next->children->attr.val_int;
                    return NULL;
                }
            }
        }
        */
        Operand t1 = new_temp();
        InterCode code1 = translate_Exp(child, t1);

        Operand t2 = new_temp();
        InterCode code2 = translate_Exp(child->next->next, t2);
        if(place){
            if(t1->kind == CONSTANT && t2->kind == CONSTANT){
                place->kind = CONSTANT;
                place->u.value = t1->u.value + t2->u.value;
                return NULL;
            }

            InterCode code3 = new_threeOp_interCode(ADD, place, t1, t2);
            code2 = jointCode(code2, code3);
        }
        code1 = jointCode(code1, code2);
        return code1;
    }
    // Exp1 MINUS Exp2
    else if  (!strcmp(child->next->name, "MINUS")) {
        /*
        if (!strcmp(child->children->name, "INT")) {
            if(!strcmp(child->next->next->children->name, "INT")) {
                if(place) {
                    place->kind = CONSTANT;
                    place->u.value = child->children->attr.val_int - child->next->next->children->attr.val_int;
                    return NULL;
                }
            }
        }
        */
        Operand t1 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        
        Operand t2 = new_temp();
        InterCode code2 = translate_Exp(child->next->next, t2);
        if(place){
            if(t1->kind == CONSTANT && t2->kind == CONSTANT){
                place->kind = CONSTANT;
                place->u.value = t1->u.value + t2->u.value;
                return NULL;
            }

            InterCode code3 = new_threeOp_interCode(MIN, place, t1, t2);
            code2 = jointCode(code2, code3);
        }
        code1 = jointCode(code1, code2);
        return code1;
    }
    // Exp1 STAR Exp2
    else if  (!strcmp(child->next->name, "STAR")) {
        /*
        if (!strcmp(child->children->name, "INT")) {
            if(!strcmp(child->next->next->children->name, "INT")) {
                if(place) {
                    place->kind = CONSTANT;
                    place->u.value = child->children->attr.val_int * child->next->next->children->attr.val_int;
                    return NULL;
                }
            }
        }
        */
        Operand t1 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        
        Operand t2 = new_temp();
        InterCode code2 = translate_Exp(child->next->next, t2);
        if(place){
            if(t1->kind == CONSTANT && t2->kind == CONSTANT){
                place->kind = CONSTANT;
                place->u.value = t1->u.value + t2->u.value;
                return NULL;
            }

            InterCode code3 = new_threeOp_interCode(MUL, place, t1, t2);
            code2 = jointCode(code2, code3);
        }
        code1 = jointCode(code1, code2);
        return code1;
    }
    // Exp1 DIV Exp2
    else if  (!strcmp(child->next->name, "DIV")) {
        /*
        if (!strcmp(child->children->name, "INT")) {
            if(!strcmp(child->next->next->children->name, "INT")) {
                if(place) {
                    place->kind = CONSTANT;
                    place->u.value = child->children->attr.val_int / child->next->next->children->attr.val_int;
                    return NULL;
                }
            }
        }
        */
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        InterCode code2 = translate_Exp(child->next->next, t2);
        if(place){
            if(t1->kind == CONSTANT && t2->kind == CONSTANT){
                place->kind = CONSTANT;
                place->u.value = t1->u.value + t2->u.value;
                return NULL;
            }

            InterCode code3 = new_threeOp_interCode(DIV, place, t1, t2);
            code2 = jointCode(code2, code3);
        }
        code1 = jointCode(code1, code2);
        return code1;
    }
    // Exp1 RELOP Exp2
    else if  (!strcmp(child->next->name, "RELOP")) {
        if(place && !strcmp(child->children->name, "INT") && !strcmp(child->next->next->children->name, "INT")){
            place->kind = CONSTANT;
            int left = child->children->attr.val_int;
            int right = child->next->next->children->attr.val_int;
            child = child->next;
            if(!strcmp(child->attr.val_str, ">")){
                place->u.value = left>right?1:0;
            }else if(!strcmp(child->attr.val_str, "<")){
                place->u.value = left<right?1:0;
            }else if(!strcmp(child->attr.val_str, ">=")){
                place->u.value = left>=right?1:0;
            }else if(!strcmp(child->attr.val_str, "<=")){
                place->u.value = left<=right?1:0;
            }else if(!strcmp(child->attr.val_str, "==")){
                place->u.value = left==right?1:0;
            }else if(!strcmp(child->attr.val_str, "!=")){
                place->u.value = left!=right?1:0;
            }else{
                fprintf(stderr, "Unexpected RELOP error in translate_Exp(), interCode.c\n");
                exit(-1);
            }
            return NULL;
        }

        return translate_logical(node, place);
    }
    // Exp1 AND Exp2
    else if  (!strcmp(child->next->name, "AND")) {
        if(place && !strcmp(child->children->name, "INT") && !strcmp(child->next->next->children->name, "INT")){
            place->kind = CONSTANT;
            int left = child->children->attr.val_int;
            int right = child->next->next->children->attr.val_int;
            
            place->u.value = (left && right)?1:0;
            return NULL;
        }

        return translate_logical(node, place);
    }
    // Exp1 OR Exp2
    else if  (!strcmp(child->next->name, "OR")) {
        if(place && !strcmp(child->children->name, "INT") && !strcmp(child->next->next->children->name, "INT")){
            place->kind = CONSTANT;
            int left = child->children->attr.val_int;
            int right = child->next->next->children->attr.val_int;
            
            place->u.value = (left || right)?1:0;
            return NULL;
        }

        return translate_logical(node, place);
    }
    // NOT Exp1
    else if  (!strcmp(child->name, "NOT")) {
        if(place && !strcmp(child->next->children->name, "INT")){
            place->kind = CONSTANT;
            int valOp = child->next->children->attr.val_int;
            
            place->u.value = (!valOp)?1:0;
            return NULL;
        }

        return translate_logical(node, place);
    }
    // MINUS Exp1
    else if  (!strcmp(child->name, "MINUS")) {
        Operand temp = new_temp();
        InterCode code1 = translate_Exp(child->next, temp);

        if(place){
            if(temp->kind = CONSTANT){
                place->kind = CONSTANT;
                place->u.value = -(temp->u.value);
                return NULL;
            }

            Operand zeroOp = new_constant(0);
            zeroOp->u.value = 0;
            InterCode code2 = new_threeOp_interCode(MIN, place, zeroOp, temp);
            code1 = jointCode(code1, code2);
        }
        return code1;
    }
    // Exp -> ID LP RP
    else if (!strcmp(child->name, "ID") && !strcmp(child->next->name, "LP") && !strcmp(child->next->next->name, "RP")) {
        if(!place){
            return NULL;
        }

        char* funName = child->attr.val_str;
        Operand funOp = new_operand(FUNCNAME, funName);
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
        Operand funOp = new_operand(FUNCNAME, funName);

        child = child->next->next;  // skip LP
        Operand arg_list = (Operand)malloc(sizeof(struct Operand_));
        arg_list->next = NULL;
        InterCode code1 = translate_Args(child, arg_list);
        if(!strcmp(funName, "write")){
            code1 = jointCode(code1, new_oneOp_interCode(WRITE, arg_list->next));
            return code1;
        }else{
            Operand p = arg_list->next;
            InterCode code2 = NULL;
            while(p){
                code2 = jointCode(code2, new_oneOp_interCode(ARG, p));
                p = p->next;
            }
            if(!place){
                place = new_temp();
            }
            code2 = jointCode(code2, new_twoOp_interCode(CALL, place, funOp));
            code1 = jointCode(code1, code2);
            return code1;
        }
    }
    // Exp -> LP Exp RP
    else if(!strcmp(child->name, "LP")) {
        return translate_Exp(child->next, place);
    }
    // Exp -> Exp LB Exp RB
    else if(!strcmp(child->next->name, "LB")){
        /*
        Type dstType = Exp(child);    // the type of ARRAY
        if(dstType->u.array.elem->kind == ARRAY){
            fprintf(stderr, "Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
            exit(-1);
        }

        Operand t1 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        Operand addr1 = new_operand(ADDRESS, getOperand(t1));

        Operand sizeOp = new_constant(getTypeSize(dstType->u.array.elem));
        child = child->next->next;
        Operand t2 = new_temp();
        InterCode code2 = translate_Exp(child, t2);
        Operand t3 = new_temp();
        InterCode code3 = new_threeOp_interCode(MUL, t3, sizeOp, t2);
        Operand t4 = new_temp();
        InterCode code4 = new_threeOp_interCode(ADD, t4, addr1, t3);
        code3= jointCode(code3, code4);
        code2 = jointCode(code2, code3);
        code1 = jointCode(code1, code2);
        if(place){
            place->kind = ADDTOVAL;
            place->u.varName = getOperand(t4);
        }
        return code1;
        */

        Operand t1 = new_temp();
        int offset = 0;
        InterCode retCode = (InterCode)malloc(sizeof(struct InterCode_));
        retCode->next = NULL;
        getFirstAddress(node, t1, &offset, retCode);
        retCode = retCode->next;
        if(offset){
            Operand offsetOp = new_constant(offset);
            if(t1->kind == ADDRESS){
                Operand t2 = new_temp();
                t1->kind = VARIABLE;
                InterCode code1 = new_twoOp_interCode(ADD_2_VAL, t2, t1);
                Operand t3 = new_temp();
                InterCode code2 = new_threeOp_interCode(ADD, t3, t2, offsetOp);
                code1 = jointCode(code1, code2);
                retCode = jointCode(retCode, code1);
                if(place){
                    place->kind = ADDTOVAL;
                    place->u.varName = getOperand(t3);
                }
            }else{
                Operand t2 = new_temp();
                InterCode code1 = new_threeOp_interCode(ADD, t2, t1, offsetOp);
                retCode = jointCode(retCode, code1);
                if(place){
                    place->kind = ADDTOVAL;
                    place->u.varName = getOperand(t2);
                }
            }
        }else{
            if(place){
                place->kind = ADDTOVAL;
                if(t1->kind != ADDRESS){
                    place->u.varName = getOperand(t1);
                }else{
                    Operand t2 = new_temp();
                    t1->kind = VARIABLE;
                    InterCode code1 = new_twoOp_interCode(ADD_2_VAL, t2, t1);
                    place->u.varName = getOperand(t2);
                    retCode = jointCode(retCode, code1);
                }
            }
        }
        return retCode;
    }
    // Exp -> Exp DOT ID
    else if(!strcmp(child->next->name, "DOT")){
        /*
        Type dstType = Exp(child);    // the type of STRUCTURE

        Operand t1 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        Operand addr1 = new_operand(ADDRESS, getOperand(t1));


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
        code1 = jointCode(code1, code2);
        if(place){
            place->kind = ADDTOVAL;
            place->u.varName = getOperand(t2);
        }
        return code1;
        */

        Operand t1 = new_temp();
        int offset = 0;
        InterCode retCode = (InterCode)malloc(sizeof(struct InterCode_));
        retCode->next = NULL;
        getFirstAddress(node, t1, &offset, retCode);
        retCode = retCode->next;
        if(offset){
            Operand offsetOp = new_constant(offset);

            if(t1->kind == ADDRESS){
                Operand t2 = new_temp();
                t1->kind = VARIABLE;
                InterCode code1 = new_twoOp_interCode(ADD_2_VAL, t2, t1);
                Operand t3 = new_temp();
                InterCode code2 = new_threeOp_interCode(ADD, t3, t2, offsetOp);
                code1 = jointCode(code1, code2);
                retCode = jointCode(retCode, code1);
                if(place){
                    place->kind = ADDTOVAL;
                    place->u.varName = getOperand(t3);
                }
            }else{
                Operand t2 = new_temp();
                InterCode code1 = new_threeOp_interCode(ADD, t2, t1, offsetOp);
                retCode = jointCode(retCode, code1);
                if(place){
                    place->kind = ADDTOVAL;
                    place->u.varName = getOperand(t2);
                }
            }
        }else{
            if(place){
                place->kind = ADDTOVAL;
                if(t1->kind != ADDRESS){
                    place->u.varName = getOperand(t1);
                }else{
                    Operand t2 = new_temp();
                    t1->kind = VARIABLE;
                    InterCode code1 = new_twoOp_interCode(ADD_2_VAL, t2, t1);
                    place->u.varName = getOperand(t2);
                    retCode = jointCode(retCode, code1);
                }
            }
        }
        return retCode;
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
        if (!strcmp(child->next->children->name, "INT")) {
            Operand cons = new_constant(child->next->children->attr.val_int);
            InterCode code = new_oneOp_interCode(RETURN, cons);
            return code;
        }
        child = child->next;    // skip RETURN
        Operand t1 = new_temp();
        InterCode code1 = translate_Exp(child, t1);
        InterCode code2 = new_oneOp_interCode(RETURN, t1);
        code1 = jointCode(code1, code2);
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

        child = child->next;
        if(child){  // ELSE Stmt
            child = child->next;    // skip ELSE
            Operand label3 = new_label();
            InterCode label3_code = new_oneOp_interCode(LABEL, label3);
            InterCode goto_code = new_oneOp_interCode(GOTO, label3);
            InterCode code3 = translate_Stmt(child);
            code3 = jointCode(code3, label3_code);
            label2_code = jointCode(label2_code, code3);
            goto_code = jointCode(goto_code, label2_code);
            code2 = jointCode(code2, goto_code);
        }else{
            code2 = jointCode(code2, label2_code);
        }
        label1_code = jointCode(label1_code, code2);
        code1 = jointCode(code1, label1_code);
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
        
        goto_code = jointCode(goto_code, label3_code);
        code2 = jointCode(code2, goto_code);
        label2_code = jointCode(label2_code, code2);
        code1 = jointCode(code1, label2_code);
        label1_code = jointCode(label1_code, code1);
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
       
       code3 = jointCode(code3, code4);
       code2 = jointCode(code2, code3);
       code1 = jointCode(code1, code2);
       return code1;
    }
    // Exp1 AND Exp2
    else if  (!strcmp(child->next->name, "AND")) {
        Operand label = new_label();
        InterCode code1 = translate_Cond(child, label, label_false);
        InterCode code2 = translate_Cond(child->next->next, label_true, label_false);
        InterCode code3 = new_oneOp_interCode(LABEL, label);
        
        code3 = jointCode(code3, code2);
        code1 = jointCode(code1, code3);
        return code1;
    }
    // Exp1 OR Exp2
    else if  (!strcmp(child->next->name, "OR")) {
        Operand label = new_label();
        InterCode code1 = translate_Cond(child, label_true, label);
        InterCode code2 = translate_Cond(child->next->next, label_true, label_false);
        InterCode code3 = new_oneOp_interCode(LABEL, label);
        
        code3 = jointCode(code3, code2);
        code1 = jointCode(code1, code3);
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

        code2 = jointCode(code2, code3);
        code1 = jointCode(code1, code2);
        return code1;
    }
}

InterCode translate_Args(TreeNode* node, Operand arg_list) {
    TreeNode* child = node->children;
    Operand t1 = new_temp();
    InterCode code1 = translate_Exp(child, t1);
    t1->next = arg_list->next;
    arg_list->next = t1;
    child = child->next;
    if(child){
        child = child->next;
        InterCode code2 = translate_Args(child, arg_list);
        code1 = jointCode(code1, code2);
        return code1;
    }
    return code1;
}

InterCode translate_CompSt(TreeNode* node) {
    TreeNode* child = node->children->next;
    InterCode retCode = NULL;
    if(!strcmp(child->name, "DefList")){
        retCode = translate_DefList(child);
        child = child->next;
    }
    if(!strcmp(child->name, "StmtList")){
        retCode = jointCode(retCode, translate_StmtList(child));
    }
    return retCode;
}

InterCode translate_StmtList(TreeNode *node) {
    TreeNode* child = node->children;
    InterCode retCode = NULL;
    if(child){
        retCode = translate_Stmt(child);
        child = child->next;
        if(child){
            retCode = jointCode(retCode, translate_StmtList(child));
        }
    }
    return retCode;
}

InterCode translate_DefList(TreeNode *node) {
    TreeNode* child = node->children;
    InterCode retCode = NULL;
    if(child){
        retCode = translate_Def(child);
        child = child->next;
        if(child){
            retCode = jointCode(retCode, translate_DefList(child));
        }
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
        retCode = jointCode(retCode, translate_DecList(child->next));
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
            code1 = jointCode(code1, code2);
            return code1;
        }
        return NULL;
    }
    if(type->kind == ARRAY && type->u.array.elem->kind == ARRAY){
        fprintf(stderr, "Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
        exit(-1);
    }
    return new_dec_interCode(op, getTypeSize(type));
}

InterCode translate_ExtDef(TreeNode* node){
    TreeNode* child = node->children->next;
    InterCode retCode = NULL;

    if(!strcmp(child->name, "FunDec")){
        retCode = translate_FunDec(child);
        retCode = jointCode(retCode, translate_CompSt(child->next));
    }else{
        /*
        fprintf(stderr, "There has a unexpected global definition in translate_ExtDef, interCode.c\n");
        exit(-1);
        */
        // def of Struct
    }
    return retCode;
}

InterCode translate_FunDec(TreeNode* node){
    TreeNode* child = node->children;
    char* funcName = child->attr.val_str;
    Operand funcOp = new_operand(FUNCNAME, funcName);
    InterCode code1 = new_oneOp_interCode(FUNCTION, funcOp);

    FieldList p = hashCheck(funcName)->info->func->paramList;
    while(p){
        if (p->type->kind == ARRAY) {
            fprintf(stderr, "Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
            exit(-1);
        }
        code1 = jointCode(code1, new_oneOp_interCode(PARAM, new_operand(VARIABLE, p->name)));
        p = p->tail;
    }

    return code1;
}

InterCode translate_Program(TreeNode* node){
    TreeNode* child = node->children;
    if(child){
        return translate_ExtDefList(child);
    }
    return NULL;
}

InterCode translate_ExtDefList(TreeNode* node){
    TreeNode* child = node->children;
    InterCode retCode = NULL;
    if(child){
        retCode = translate_ExtDef(child);
        child = child->next;
        if(child){
            retCode = jointCode(retCode, translate_ExtDefList(child));
        }
    }
    return retCode;
}