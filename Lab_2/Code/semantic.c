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
    } else if (strcmp(child->name, "StructSpecifier") == 0) {
        type = StructSpecifier(child);
    }
    return type;
}

Type StructSpecifier(TreeNode *node) {
    TreeNode *child = node->children->next; // skip STRUCT
    if(!strcmp(child->name, "OptTag")){ // defined
        HashNode hashNode = NULL;
        hashNode = (HashNode)malloc(sizeof(struct HashNode_));

        hashNode->kind = VARI;
        strcpy(hashNode->name, OptTag(child));

        child = child->next->next; // skip LC("{")
        Type type = (Type)malloc(sizeof(struct Type_));
        type->kind = STRUCTURE;
        DefList(child, type);
        hashNode->type = type;
        // skip RC("}")
        if(strcmp(hashNode->name, "")&&hashCheck(hashNode->name)){
            printf("Error type 16 at Line %d: Duplicated name \"%s\"", node->lineno, hashNode->name);
        }else{
            insertHashtNode(hashNode);
            return type;
        }
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

void DefList(TreeNode *node, Type type){

}