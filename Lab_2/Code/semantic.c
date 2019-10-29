#include "semantic.h"

void semanticAnalysis() {
    Program(root);
}

void Program(TreeNode *node) {
    ExtDefList(node);
}

void ExtDefList(TreeNode *node) {
    TreeNode *child = node->children;
    if (child) {
        ExtDef(child);
        ExtDefList(child->next);
    }
}

void ExtDef(TreeNode *node) {
    TreeNode *child = node->children;
    Type type = Specifier(child);

    child = child->next;
    if (!strcmp(child->name, "ExtDecList")) {
        // TODO
        ExtDecList(child->next, type);
    } else if(!strcmp(child->name, "SEMI")) {
        // TODO
    } else {
        // TODO
    }
}

void ExtDecList(TreeNode *node, Type type) {

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
    if(!strcmp(child->name, "OptTag")){ /* ******* defined ******* */
        // HashNode hashNode = createHashNode(OptTag(child), VARI);
        char* hashName = OptTag(child);

        child = child->next->next; // skip LC("{")
        Type type = (Type)malloc(sizeof(struct Type_));
        type->kind = STRUCTURE;
        type->u.structure = NULL;
        DefList(child, true, type->u.structure);
        // skip RC("}")
        if(strcmp(hashName, "")){
            Info info = (Info)malloc(sizeof(struct Info_));
            info->kind = VARI;
            info->type = type;
            info->next = NULL;
            HashNode temp = hashCheck(hashName);
            if(temp){
                temp->info->next = info;
                printf("Error type 16 at Line %d: Duplicated name \"%s\"", node->children->next->lineno, hashName);
            }else{
                HashNode hashNode = (HashNode)malloc(sizeof(struct HashNode_));
                hashNode->name = hashName;
                hashNode->info = info;
                insertHashtNode(hashNode);
            }
        }
        return type;
    }else{  /* ******* use ******* */
        char* sname = OptTag(child);
        HashNode hashNode = hashCheck(sname);
        if(hashNode && hashNode->info->kind==VARI && hashNode->info->type->kind==STRUCTURE){
            return hashNode->info->type;
        }else{
            Type type = (Type)malloc(sizeof(struct Type_));
            type->kind = STRUCTURE;
            type->u.structure = NULL;
            return type;
            printf("Error type 17 at Line %d: Undefined structure \"%s\"", child->lineno, sname);
        }
    }
}

char* OptTag(TreeNode *node){
    TreeNode *child = node->children;
    if(child){
        return child->attr.val_str;
    }
    return "";
}

char* Tag(TreeNode *node){
    return node->children->attr.val_str;
}

char* VarDec(TreeNode *node, Type type){

}

void DefList(TreeNode *node, bool isStruct, FieldList fieldList){
    TreeNode *child = node->children;
    if(child){
        Def(child, isStruct, fieldList);
        child = child->next;
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
            bool isFirstError = true;
            while(temp->tail){
                if(isFirstError && !strcmp(temp->name, newFieldList->name)){
                    isFirstError = false;
                    printf("Error type 15 at Line %d: Redefined field \"%s\"", child->lineno, varName);
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
        printf("Error type 3 at Line %d: Redefined variable \"%s\"", child->lineno, varName);
    }else{
        HashNode hashNode = (HashNode)malloc(sizeof(struct HashNode_));
        hashNode->name = varName;
        hashNode->info = info;
        insertHashtNode(hashNode);
    }

    child = child->next;
    if(child){
        if(isStruct){
            printf("Error type 15 at Line %d: Field \"%s\" should not be initialized.", node->children->lineno, varName);
        }
        else{
            // TODO check ASSIGNOP("=")
            child = child->next;
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
        if (p1->type != p2->type) {
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
    if (!strcmp(child->name, "ID") && child->next == NULL) {
       // printf("Exp->ID\n");
       //  type = (Type)malloc(sizeof(struct Type_));
        HashNode checkNode = NULL;
        checkNode = hashCheck(child->attr.val_str);
        if (checkNode == NULL) {
            printf("Error Type 1 at Line %d: Undefined variable \"%s\"\n", child->lineno, child->attr.val_str);
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
    } else if (!strcmp(child->name, "INT") && child->next == NULL) {
        type = (Type)malloc(sizeof(struct Type_));
        type->kind = BASIC;
        type->u.basic = 0;
        return type;
    } else if (!strcmp(child->name, "FLOAT") && child->next == NULL) {
        type = (Type)malloc(sizeof(struct Type_));
        type->kind = BASIC;
        type->u.basic = 1;
        return type;
    } 
    // Error Type 2: FUNC undefined
    // Function without Params
    else if (!strcmp(child->name, "ID") && !strcmp(child->next->name, "LP") && !strcmp(child->next->next->name, "RP")) {
        HashNode checkNode = hashCheck(child->attr.val_str);
        if (checkNode == NULL) {
            // 不在符号表中
            printf("Error Type 2 at Line %d: Undefined function \"%s\"\n", child->lineno, child->attr.val_str);
            return NULL;
        } else if (checkInfo(checkNode, FUNC) == NULL) {
            // 在符号表中但是变量
            printf("Error Type 11 at Line %d: \"%s\" is not a function\n", child->lineno, child->attr.val_str);
        } else {
            Info info = checkInfo(checkNode, FUNC);
            if (info->func->paramList == NULL) {
                return info->func->ret;
            } else {
                // 引用了带参函数
                printf("Error Type 9 at Line %d: Arguments not applicable for Function \"%s\"\n ", child->lineno, child->attr.val_str);
                return NULL;
            }
        }
    } else if (!strcmp(child->name, "ID") && !strcmp(child->next->next->name, "Args")) {
        HashNode checkNode = hashCheck(child->attr.val_str);
        if (checkNode == NULL) {
            // 不在符号表中
            printf("Error Type 2 at Line %d: Undefined function \"%s\"\n", child->lineno, child->attr.val_str);
            return NULL;
        } else if (checkInfo(checkNode, FUNC) == NULL){
            // 在符号表中但是变量
            printf("Error Type 11 at Line %d: \"%s\" is not a function\n", child->lineno, child->attr.val_str);
        } else {
            Info info = checkInfo(checkNode, FUNC);
            FieldList childParamList = Args(child->next->next);
            bool matchArgs = checkArgs(childParamList, info->func->paramList);
            if (matchArgs) {
                return info->func->ret;
            } else {
                // 参数列表不匹配
                printf("Error Type 9 at Line %d: Arguments not applicable for Function \"%s\"\n ", child->lineno, child->attr.val_str);
                return NULL;
            }
        }
    }
}