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