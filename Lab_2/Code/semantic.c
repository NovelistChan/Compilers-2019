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
        type->next = NULL;
        type->kind = BASIC;
        if (strcmp(child->attr.val_str, "int") == 0) {
            type->u.basic = 0;
        } else type->u.basic = 1;
    } else if (strcmp(child->name, "StructSpecifier") == 0) {
        type = StructSpecifier(child);
    }
    return type;
}

Type StructSpecifier(TreeNode *node) {
    TreeNode *child = node->children->next; // skip STRUCT
    if(!strcmp(child->name, "OptTag")){ /* ******* defined ******* */
        HashNode hashNode = createHashNode(OptTag(child), VARI);

        child = child->next->next; // skip LC("{")
        Type type = (Type)malloc(sizeof(struct Type_));
        type->next = NULL;
        type->kind = STRUCTURE;
        type->u.structure = NULL;
        DefList(child, true, type->u.structure);
        hashNode->info->type = type;
        // skip RC("}")
        if(strcmp(hashNode->name, "")){
            HashNode temp = hashCheck(hashNode->name);
            if(temp){
                temp->type->next = type;
                printf("Error type 16 at Line %d: Duplicated name \"%s\"", node->lineno, hashNode->name);
                return type;
            }
        }
        insertHashtNode(hashNode);
        return type;
    }else{
        char* sname = OptTag(child);
        HashNode hashNode = hashCheck(sname);
        if(hashNode && hashNode->kind==VARI && hashNode->type->kind==STRUCTURE){
            return hashNode->type;
        }else{
            printf("Error type 17 at Line %d: Undefined structure \"%s\"", node->lineno, sname);
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
    FieldList newFieldList = (FieldList)malloc(sizeof(struct FieldList_));
    newFieldList->tail=NULL;
    Type newType = (Type)malloc(sizeof(struct Type_));
    memcpy(newType, type, sizeof(struct Type_));
    strcpy(newFieldList->name, VarDec(child, newType));
    newFieldList->type = newType;
    /* add newFieldList to the tail of fieldList and check the duplication of name in structure,
    then check the duplication of name in hashTable
    */
}

FieldList Args(TreeNode* node) {

}

int checkArgs(FieldList paramList1, FieldList paramList2) {

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
        type->next = NULL;
        type->u.basic = 0;
        return type;
    } else if (!strcmp(child->name, "FLOAT") && child->next == NULL) {
        type = (Type)malloc(sizeof(struct Type_));
        type->kind = BASIC;
        type->next = NULL;
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