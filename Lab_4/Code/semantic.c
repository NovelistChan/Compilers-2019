#include "semantic.h"
extern bool is_pass;

void semanticAnalysis() {
    Program(root);
    checkFunction();
}

void Program(TreeNode *node) {
    TreeNode *child = node->children;
    if(child){
        ExtDefList(child);
    }
}

void ExtDefList(TreeNode *node) {
    TreeNode *child = node->children;
    if(!strcmp(child->name, "ExtDef")){
        ExtDef(child);
    }else{
        FunDeclaration(child);
    }
    child = child->next;
    if(child){
        ExtDefList(child);
    }
}

void ExtDef(TreeNode *node) {
    TreeNode *child = node->children;
    Type type = Specifier(child);

    child = child->next;
    if (!strcmp(child->name, "ExtDecList")) {
        ExtDecList(child, type);
        // skip SEMI
    } else if(!strcmp(child->name, "SEMI")) {
        // Nothing
    } else {
        FunDec(child, type, true);
        child = child->next;
        CompSt(child, type);
    }
}

void ExtDecList(TreeNode *node, Type type) {
    TreeNode *child = node->children;
    Type copyType = (Type)malloc(sizeof(struct Type_));
    memcpy(copyType, type, sizeof(struct Type_));

    char* varName = VarDec(child, copyType);

    HashNode temp = hashCheck(varName);
    Info info = (Info)malloc(sizeof(struct Info_));
    info->kind = VARI;
    info->type = copyType;
    info->next = NULL;
    if(temp){
        temp->info->next = info;
        fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n", child->lineno, varName);
        is_pass = false;
    }else{
        HashNode hashNode = (HashNode)malloc(sizeof(struct HashNode_));
        hashNode->name = varName;
        hashNode->info = info;
        insertHashNode(hashNode);
    }

    child = child->next;
    if(child){
        child = child->next;  // skip COMMA(",")
        ExtDecList(child, type);
    }
}

void FunDeclaration(TreeNode *node){
    TreeNode *child = node->children;
    Type type = Specifier(child);
    child = child->next;
    FunDec(child, type, false);
    // skip SEMI
}

Type Specifier(TreeNode *node) {
    TreeNode *child = node->children;
    Type type = NULL;
    if (strcmp(child->name, "TYPE") == 0) {
        type = (Type)malloc(sizeof(struct Type_));
        type->kind = BASIC;
        if (strcmp(child->attr.val_str, "int") == 0) {
            type->u.basic = 0;
        } else type->u.basic = 1;
    } else {
        type = StructSpecifier(child);
    }
    return type;
}

Type StructSpecifier(TreeNode *node) {
    TreeNode *child = node->children->next; // skip STRUCT
    if(!strcmp(child->name, "Tag")){ /* ******* use ******* */
        char* sname = Tag(child);
        HashNode hashNode = hashCheck(sname);
        if(hashNode && hashNode->info->kind==VARI && hashNode->info->type->kind==STRUCTURE){
            return hashNode->info->type;
        }else{
            fprintf(stderr, "Error type 17 at Line %d: Undefined structure \"%s\".\n", child->lineno, sname);
            is_pass = false;
            Type type = (Type)malloc(sizeof(struct Type_));
            type->kind = STRUCTURE;
            type->u.structure = NULL;
            return type;
        }
    }else{  /* ******* define ******* */
        char* hashName = "";
        if(!strcmp(child->name, "OptTag")){
            hashName = OptTag(child);
            child = child->next;
        }

        child = child->next; // skip LC("{")
        Type type = (Type)malloc(sizeof(struct Type_));
        type->kind = STRUCTURE;
        type->u.structure = NULL;
        FieldList tempFieldList = (FieldList)malloc(sizeof(struct FieldList_));
        tempFieldList->tail = NULL;
        if(!strcmp(child->name, "DefList")){
            DefList(child, true, tempFieldList);
            type->u.structure = tempFieldList->tail;
        }
        // skip RC("}")
        if(strcmp(hashName, "")){
            Info info = (Info)malloc(sizeof(struct Info_));
            info->kind = VARI;
            info->type = type;
            info->next = NULL;
            HashNode temp = hashCheck(hashName);
            if(temp){
                temp->info->next = info;
                is_pass = false;
                fprintf(stderr, "Error type 16 at Line %d: Duplicated name \"%s\".\n", node->children->next->lineno, hashName);
            }else{
                HashNode hashNode = (HashNode)malloc(sizeof(struct HashNode_));
                hashNode->name = hashName;
                hashNode->info = info;
                insertHashNode(hashNode);
            }
        }
        return type;
    }
}

char* OptTag(TreeNode *node){
    TreeNode *child = node->children;
    return child->attr.val_str;;
}

char* Tag(TreeNode *node){
    return node->children->attr.val_str;
}

char* VarDec(TreeNode *node, Type type){
    TreeNode *child = node->children;
    if(!strcmp(child->name, "ID")){
        return child->attr.val_str;
    }

    Type copyType = (Type)malloc(sizeof(struct Type_));
    memcpy(copyType, type, sizeof(struct Type_));
    type->kind = ARRAY;
    type->u.array.elem = copyType;
    child = child->next->next;  // skip LB("[")
    type->u.array.size = child->attr.val_int;
    return VarDec(node->children, type);
    // skip RB("]")
}

void FunDec(TreeNode *node, Type type, bool isDef){
    TreeNode *child = node->children;
    char* funcName = child->attr.val_str;
    Func func = (Func)malloc(sizeof(struct Func_));
    func->ret = type;
    func->paramNum = 0;
    func->paramList = NULL;
    func->ifDef = !isDef;
    func->ifReal = isDef;
    func->decLines = NULL;

    HashNode checkNode = hashCheck(funcName);

    child = child->next->next;  // skip LP("(")
    if(!strcmp(child->name, "VarList")){
        func->paramList = VarList(child);
        FieldList p = func->paramList;
        while(p){
            p = p->tail;
            func->paramNum ++;
        }

        bool hasInsert = true;
        if(checkNode){
            Info temp = checkNode->info;
            while(temp){
                if(temp->kind == FUNC && !funcCmp(temp->func, func)){
                    hasInsert = false;
                    break;
                }
                temp = temp->next;
            }
        }
        if(hasInsert){
            p = func->paramList;
            while(p){
                HashNode checkNode = hashCheck(p->name);
                Info info = (Info)malloc(sizeof(struct Info_));
                info->kind = VARI;
                info->type = p->type;
                info->next = NULL;
                if(checkNode){
                    Info temp = checkNode->info;
                    while(temp->next){
                        temp = temp->next;
                    }
                    temp->next = info;
                    is_pass = false;
                    fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n", child->lineno, p->name);
                }else{
                    HashNode newNode = (HashNode)malloc(sizeof(struct HashNode_));
                    newNode->name = p->name;
                    newNode->info = info;
                    insertHashNode(newNode);
                }
                p = p->tail;
            }
        }
    }

    // skip RP(")")

    if(!isDef){
        DecLine decLine = (DecLine)malloc(sizeof(struct DecLine_));
        decLine->lineno = node->children->lineno;
        decLine->next = NULL;
        func->decLines = decLine;
    }
    // check hashTable
    Info newInfo = (Info)malloc(sizeof(struct Info_));
    newInfo->kind = FUNC;
    newInfo->func = func;
    newInfo->next = NULL;

    if(!checkNode){
        HashNode newNode = (HashNode)malloc(sizeof(struct HashNode_));
        newNode->name = funcName;
        newNode->info = newInfo;
        insertHashNode(newNode);
    }else{
        Info p = checkNode->info;
        while(p){
            if(p->kind==FUNC && !funcCmp(p->func, func)){
                if(isDef){
                    // Redefined function
                    if(p->func->ifReal){
                        is_pass = false;
                        fprintf(stderr, "Error type 4 at Line %d: Redefined function \"%s\".\n", child->lineno, funcName);
                    }else{
                        p->func->ifReal = true;
                    }
                }else{
                    if(!p->func->ifDef){
                        p->func->ifDef = true;
                        p->func->decLines = func->decLines;
                    }else{
                        DecLine q = p->func->decLines;
                        while(q->next){
                            q = q->next;
                        }
                        q->next = func->decLines;
                    }
                }
                break;
            }
            p = p->next;
        }
        if(!p){
            p = checkNode->info;
            bool decConflict = true;
            while(p->next){
                if(p->kind == FUNC && p->func->ifReal){
                    decConflict = false;
                }
                p = p->next;
            }
            if(p->kind == FUNC && p->func->ifReal){
                decConflict = false;
            }
            p->next = newInfo;
            if(isDef){
                if(decConflict){
                    fprintf(stderr, "Error type 19 at Line %d: Conflicting declaration and definition of function \"%s\".\n", child->lineno, funcName);
                }else{
                    fprintf(stderr, "Error type 4 at Line %d: Redefined function \"%s\".\n", child->lineno, funcName);
                }
            }else{
                if(decConflict){
                    fprintf(stderr, "Error type 19 at Line %d: Inconsistent declaration of function \"%s\".\n", child->lineno, funcName);
                }else{
                    fprintf(stderr, "Error type 19 at Line %d: Conflicting declaration and definition of function \"%s\".\n", child->lineno, funcName);
                }
            }
            is_pass = false;
        }
    }
}

FieldList VarList(TreeNode *node){
    TreeNode *child = node->children;
    FieldList ret = ParamDec(child);
    child = child->next;
    if(child){
        child = child->next;    // skip COMMA(",")
        ret->tail = VarList(child);
    }
    return ret;
}

FieldList ParamDec(TreeNode *node){
    TreeNode *child = node->children;
    Type type = Specifier(child);
    child = child->next;
    FieldList ret = (FieldList)malloc(sizeof(struct FieldList_));
    ret->name = VarDec(child, type);
    ret->type = type;
    ret->tail = NULL;
    return ret;
}

void CompSt(TreeNode *node, Type retType){
    TreeNode *child = node->children->next;  // skip LC("{")
    if(!strcmp(child->name, "DefList")){
        DefList(child, false, NULL);
        child = child->next;
    }
    if(!strcmp(child->name, "StmtList")){
        StmtList(child, retType);
    }
    // skip RC("}")
}

void StmtList(TreeNode *node, Type retType){
    TreeNode *child = node->children;
    Stmt(child, retType);
    child = child->next;
    if(child){
        StmtList(child, retType);
    }
}

void Stmt(TreeNode *node, Type retType){
    TreeNode *child = node->children;
    if(!strcmp(child->name, "Exp")){
        Exp(child);
        // skip SEMI(";")
    }else if(!strcmp(child->name, "CompSt")){
        CompSt(child, retType);
    }else if(!strcmp(child->name, "RETURN")){
        child = child->next;    // skip RETURN
        if(typeCmp(retType, Exp(child))){
            fprintf(stderr, "Error type 8 at Line %d: Type mismatched for return.\n", child->lineno);
            is_pass = false;
            // skip SEMI(";")
        }
    }else if(!strcmp(child->name, "IF")){
        child = child->next->next;  // skip IF and LP
        Type expType = Exp(child);
        if(!expType || expType->kind != BASIC || expType->u.basic != 0){
            is_pass = false;
            fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operamds(The condition in \"if\" must be \"int\").\n", child->lineno);
        }
        child = child->next->next;  // skip RP
        Stmt(child, retType);
        child = child->next;
        if(child){
            child = child->next;    // skip ELSE
            Stmt(child, retType);
        }
    }else{
        child = child->next->next;  // skip WHILE and LP
        Type expType = Exp(child);
        if(!expType || expType->kind != BASIC || expType->u.basic != 0){
            is_pass = false;
            fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operamds(The condition in \"while\" must be \"int\").\n", child->lineno);
        }
        child = child->next->next;  // skip RP
        Stmt(child, retType);
    }
}

void DefList(TreeNode *node, bool isStruct, FieldList fieldList){
    TreeNode *child = node->children;
    Def(child, isStruct, fieldList);
    child = child->next;
    if(child){
        DefList(child, isStruct, fieldList);
    }
}

void Def(TreeNode *node, bool isStruct, FieldList fieldList){  /* ******* defined ******* */
    TreeNode *child = node->children;
    Type type = Specifier(child);

    child = child->next;
    DecList(child, isStruct, fieldList, type);
    // skip SEMI
}

void DecList(TreeNode *node, bool isStruct, FieldList fieldList, Type type){
    TreeNode *child = node->children;
    Dec(child, isStruct, fieldList, type);
    child = child->next;
    if(child){
        child = child->next;    // skip COMMA
        DecList(child, isStruct, fieldList, type);
    }
}

void Dec(TreeNode *node, bool isStruct, FieldList fieldList, Type type){
    TreeNode *child = node->children;
    Type newType = (Type)malloc(sizeof(struct Type_));
    memcpy(newType, type, sizeof(struct Type_));
    char* varName = VarDec(child, newType);

    bool isFirstError = true;
    if(isStruct){
        FieldList newFieldList = (FieldList)malloc(sizeof(struct FieldList_));
        newFieldList->name = varName;
        newFieldList->type = newType;
        newFieldList->tail = NULL;
        /* add newFieldList to the tail of fieldList and check the duplication of name in structure,
        then check the duplication of name in hashTable
        */
        if(fieldList==NULL){
            fieldList = newFieldList;
        }else{
            FieldList temp = fieldList;
            while(temp->tail){
                if(isFirstError && !strcmp(temp->tail->name, newFieldList->name)){
                    isFirstError = false;
                    is_pass = false;
                    fprintf(stderr, "Error type 15 at Line %d: Redefined field \"%s\".\n", child->lineno, varName);
                }
                temp = temp->tail;
            }
            temp->tail = newFieldList;
        }
    }

    HashNode temp = hashCheck(varName);
    Info info = (Info)malloc(sizeof(struct Info_));
    info->kind = VARI;
    info->type = newType;
    info->next = NULL;
    if(temp){
        temp->info->next = info;
        if(isFirstError){
            isFirstError = false;
            is_pass = false;
            fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n", child->lineno, varName);
        }
    }else{
        HashNode hashNode = (HashNode)malloc(sizeof(struct HashNode_));
        hashNode->name = varName;
        hashNode->info = info;
        insertHashNode(hashNode);
    }

    child = child->next;
    if(child){
        if(isStruct){
            is_pass = false;
            fprintf(stderr, "Error type 15 at Line %d: Field \"%s\" should not be initialized.\n", node->children->lineno, varName);
        }
        else{
            AssignOp(newType, Exp(child->next), child->lineno);
        }
    }
}

FieldList Args(TreeNode* node) {
    FieldList paramList = NULL;
    TreeNode *child = node->children;
    Type p = Exp(child);
    if (p == NULL) {
        return paramList;
    }
    paramList = (FieldList)malloc(sizeof(struct FieldList_));
    paramList->name = NULL;
    paramList->type = p;
    paramList->tail = NULL;
    // Args -> Exp COMMA Args
    if (child->next != NULL) {
        paramList->tail = Args(child->next->next);
    }
    return paramList;
}

// 0 = 匹配 1 = 不匹配
int checkArgs(FieldList paramList1, FieldList paramList2) {
   FieldList p1 = paramList1;
   FieldList p2 = paramList2;
    while (p1 != NULL && p2 != NULL) {
        if (typeCmp(p1->type, p2->type)) {
            return 1;
        }
        p1 = p1->tail; p2 = p2->tail;
    }
    if (p1 != NULL || p2 != NULL) {
        return 1;
    }
    return 0;
}

Type Exp (TreeNode* node) {
    TreeNode *child = node->children;
    Type type = NULL;
    // Error Type 1 : VARI undefined
    // Exp -> ID
    if (!strcmp(child->name, "ID") && child->next == NULL) {
        HashNode checkNode = NULL;
        checkNode = hashCheck(child->attr.val_str);
        if (checkNode == NULL) {
            is_pass = false;
            fprintf(stderr, "Error type 1 at Line %d: Undefined variable \"%s\".\n", child->lineno, child->attr.val_str);
            return NULL;
        } 
        else {
            Info info = checkInfo(checkNode, VARI);
            if (info == NULL) return NULL;
            else return info->type;
        }
        // else if (checkNode->kind == VARI) {
        //     return checkNode->type;
        // } else return NULL;
    } 
    // Exp -> INT
    else if (!strcmp(child->name, "INT") && child->next == NULL) {
        type = (Type)malloc(sizeof(struct Type_));
        type->kind = BASIC;
        type->u.basic = 0;
        return type;
    } 
    // Exp -> FLOAT
    else if (!strcmp(child->name, "FLOAT") && child->next == NULL) {
        type = (Type)malloc(sizeof(struct Type_));
        type->kind = BASIC;
        type->u.basic = 1;
        return type;
    } 
    // Error Type 2: FUNC undefined
    // Function without Params
    // Exp -> ID LP RP
    else if (!strcmp(child->name, "ID") && !strcmp(child->next->name, "LP") && !strcmp(child->next->next->name, "RP")) {
        HashNode checkNode = hashCheck(child->attr.val_str);
        if (checkNode == NULL) {
            // 不在符号表中
            is_pass = false;
            fprintf(stderr, "Error type 2 at Line %d: Undefined function \"%s\".\n", child->lineno, child->attr.val_str);
            return NULL;
        } else if (checkInfo(checkNode, FUNC) == NULL) {
            // 在符号表中但是变量
            is_pass = false;
            fprintf(stderr, "Error type 11 at Line %d: \"%s\" is not a function.\n", child->lineno, child->attr.val_str);
        } else {
            Info info = checkInfo(checkNode, FUNC);
            if (info->func->paramList == NULL) {
                return info->func->ret;
            } else {
                // 引用了带参函数
                is_pass = false;
                fprintf(stderr, "Error type 9 at Line %d: Arguments are not applicable for Function \"%s\".\n ", child->lineno, child->attr.val_str);
                return NULL;
            }
        }
    } 
    // Exp -> ID LP Args RP
    else if (!strcmp(child->name, "ID") && !strcmp(child->next->next->name, "Args")) {
        HashNode checkNode = hashCheck(child->attr.val_str);
        if (checkNode == NULL) {
            // 不在符号表中
            is_pass = false;
            fprintf(stderr, "Error type 2 at Line %d: Undefined function \"%s\".\n", child->lineno, child->attr.val_str);
            return NULL;
        } else if (checkInfo(checkNode, FUNC) == NULL){
            // 在符号表中但是变量
            is_pass = false;
            fprintf(stderr, "Error type 11 at Line %d: \"%s\" is not a function.\n", child->lineno, child->attr.val_str);
        } else {
            Info info = checkInfo(checkNode, FUNC);
            FieldList childParamList = Args(child->next->next);
            int matchArgs = checkArgs(childParamList, info->func->paramList);
            if (matchArgs == 0) {
                return info->func->ret;
            } else {
                // 参数列表不匹配
                is_pass = false;
                fprintf(stderr, "Error type 9 at Line %d: Arguments are not applicable for Function \"%s\".\n ", child->lineno, child->attr.val_str);
                return NULL;
            }
        }
    } 
    else if (!strcmp(child->name, "Exp")) {
        // Exp -> Exp ASSIGNOP Exp
        if (!strcmp(child->next->name, "ASSIGNOP")) {
            Type left = Exp(child);
            Type right = Exp(child->next->next);
            if (left == NULL) return NULL;
            TreeNode *grandChild = child->children;
            // 赋值号左边只能是ID、Exp LB Exp RB、Exp DOT ID
            if (!((!strcmp(grandChild->name, "ID") && grandChild->next == NULL) 
            || (!strcmp(grandChild->name, "Exp") && !strcmp(grandChild->next->name, "LB") && !strcmp(grandChild->next->next->name, "Exp") && !strcmp(grandChild->next->next->next->name, "RB") && grandChild->next->next->next->next == NULL)
            || (!strcmp(grandChild->name, "Exp") && !strcmp(grandChild->next->name, "DOT") && !strcmp(grandChild->next->next->name, "ID") && grandChild->next->next->next == NULL))) {
                fprintf(stderr, "Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", child->lineno);
                is_pass = false;
                return NULL;
            }
            return AssignOp(left, right, child->lineno);
        }
        // Exp -> Exp AND Exp
        else if (!strcmp(child->next->name, "AND")) {
            Type left = Exp(child);
            Type right = Exp(child->next->next);
            if (left == NULL) return NULL;
            return LogicCheck(left, right, child->lineno);
        }
        // Exp -> Exp OR Exp
        else if (!strcmp(child->next->name, "OR")) {
            Type left = Exp(child);
            Type right = Exp(child->next->next);
            if (left == NULL) return NULL;
            return LogicCheck(left, right, child->lineno);
        }
        // Exp -> Exp RELOP Exp
        else if (!strcmp(child->next->name, "RELOP")) {
            Type left = Exp(child);
            Type right = Exp(child->next->next);
            if(ArithmeticCheck(left, right, child->lineno)){
                Type retType = (Type)malloc(sizeof(struct Type_));
                retType->kind = BASIC;
                retType->u.basic = 0;
                return retType;
            }
            return NULL;
        }
        // Exp -> Exp PLUS Exp
        else if (!strcmp(child->next->name, "PLUS")) {
            Type left = Exp(child);
            Type right = Exp(child->next->next);
            if (left == NULL) return NULL;
            return ArithmeticCheck(left, right, child->lineno);
        }
        // Exp -> Exp MINUS Exp
        else if (!strcmp(child->next->name, "MINUS")) {
            Type left = Exp(child);
            Type right = Exp(child->next->next);
            if (left == NULL) return NULL;
            return ArithmeticCheck(left, right, child->lineno);
        }
        // Exp -> Exp STAR Exp
        else if (!strcmp(child->next->name, "STAR")) {
            Type left = Exp(child);
            Type right = Exp(child->next->next);
            if (left == NULL) return NULL;
            return ArithmeticCheck(left, right, child->lineno);
        }
        // Exp -> Exp DIV Exp
        else if (!strcmp(child->next->name, "DIV")) {
            Type left = Exp(child);
            Type right = Exp(child->next->next);
            if (left == NULL) return NULL;
            return ArithmeticCheck(left, right, child->lineno);
        }
        // Exp -> Exp LB Exp RB
        else if (!strcmp(child->next->name, "LB")) {
            // printf("child->attr.val_str: %s 11\n", child->attr.val_str);
            Type body = Exp(child);
            Type index = Exp(child->next->next);
            if (body == NULL || index == NULL) return NULL;
            if (body->kind != ARRAY) {
                // printf("child->attr.val_str: %s\n", child->attr.val_str);
                is_pass = false;
                fprintf(stderr, "Error type 10 at line %d: The variable before [] is not an array.\n", child->lineno);
                return NULL;
            }
            if (!(index->kind == BASIC && index->u.basic == 0)) {
                is_pass = false;
                fprintf(stderr, "Error type 12 at line %d: The number in [] is not an integer.\n", child->lineno);
            }
            return body->u.array.elem;
        }
        // Exp -> Exp DOT ID
        else if (!strcmp(child->next->name, "DOT")) {
            Type body = Exp(child);
            // Type id = Exp(child->next->next);
            if (body->kind != STRUCTURE) {
                fprintf(stderr, "Error type 13 at line %d: Illegal use of \".\".\n", child->lineno);
                is_pass = false;
                return NULL;
            }
            FieldList fieldList = body->u.structure;
            while (fieldList) {
                if (!strcmp(fieldList->name, child->next->next->attr.val_str)) {
                    return fieldList->type;
                }
                fieldList = fieldList->tail;
            }
            is_pass = false;
            fprintf(stderr, "Error type 14 at line %d: Non-existent filed \"%s\".\n", child->lineno, child->next->next->attr.val_str);
            return NULL;
        }
    }
    // Exp -> MINUS Exp
    else if (!strcmp(child->name, "MINUS") && !strcmp(child->next->name, "Exp") && child->next->next == NULL) {
        Type body = Exp(child->next);
        if (body->kind != BASIC) {
            is_pass = false;
            fprintf(stderr, "Error type 7 at line %d: Type mismatched for operands (对非数字变量取负).\n", child->lineno);
            return NULL;
        } else {
            return body;
        }
    }
    // Exp -> NOT Exp
    else if (!strcmp(child->name, "NOT") && !strcmp(child->next->name, "Exp") && child->next->next == NULL) {
        Type body = Exp(child->next);
        if (body->kind != BASIC || body->u.basic != 0) {
            is_pass = false;
            fprintf(stderr, "Error type 7 at line %d: Type mismatched for operands (对非整型变量取非).\n", child->lineno);
            return NULL;
        } else {
            return body;
        }
    }
    // Exp -> LP Exp RP
    else if (!strcmp(child->name, "LP")) {
        Type body = Exp(child->next);
        return body;
    }
}

Type ArithmeticCheck(Type left, Type right, int line) {
    if (left == NULL || right == NULL){
        fprintf(stderr, "Error type 7 at line %d: Type mismatched for operands.\n", line);
        is_pass = false;
        return NULL;
    }
    if (left->kind != right->kind) {
        fprintf(stderr, "Error type 7 at line %d: Type mismatched for operands (different type).\n", line);
        is_pass = false;
        return NULL;
    } else if (left->kind == BASIC) {
        if (left->u.basic == right->u.basic) return left;
        else {
            fprintf(stderr, "Error type 7 at line %d: Type mismatched for operands.\n", line);
            is_pass = false;
            return NULL;
        } 
    } else {
        fprintf(stderr, "Error type 7 at line %d: Type mismatched for operands (struct or array).\n", line);
        is_pass = false;
        return NULL;
    }
}

Type LogicCheck(Type left, Type right, int line) {
    Type retType = (Type)malloc(sizeof(struct Type_));
    retType->kind = BASIC;
    retType->u.basic = 0;
    if (left == NULL || right == NULL){
        fprintf(stderr, "Error type 7 at line %d: Type mismatched for operands.\n", line);
        is_pass = false;
        return NULL;
    }
    if (left->kind != right->kind) {
        fprintf(stderr, "Error type 7 at line %d: Type mismatched for operands (different type).\n", line);
        is_pass = false;
        return NULL;
    }
    else if (left->kind == BASIC) {
        if (left->u.basic == 0 && right->u.basic == 0) {
            return retType;
        } else {
            fprintf(stderr, "Error type 7 at line %d: Type mismatched for operands (float).\n", line);
            is_pass = false;
            return NULL;
        }
    } else {
        fprintf(stderr, "Error type 7 at line %d: Type mismatched for operands (struct or array).\n", line);
        is_pass = false;
        return NULL;
    }
}

Type AssignOp(Type left, Type right, int line) {
    if (left == NULL || right == NULL) {
        fprintf(stderr, "Error type 5 at line %d: Type mismatched for assignment.\n", line);
        is_pass = false;
        return NULL;
    }
    if (!(typeCmp(left, right))) {
        return left;
    } else {
        fprintf(stderr, "Error type 5 at line %d: Type mismatched for assignment.\n", line);
        is_pass = false;
        return NULL;
    }
}