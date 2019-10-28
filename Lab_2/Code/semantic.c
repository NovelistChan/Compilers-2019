#include "semantic.h"
#include "hashTable.h"

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

Type Specifier(TreeNode *node) {
    TreeNode *child = node->children;
    Type type = NULL;
    if (strcmp(child->name, "TYPE") == 0) {
        type = (Type)malloc(sizeof(Type));
        type->kind = TypeKind.BASIC;
        if (strcmp(child->attr.val_str, "int") == 0) {
            type->u.basic = 0;
        } else type->u.basic = 1;
    } else if (strcmp(child->name, "StructSpecifier") == 0) {
        type = (Type)malloc(sizeof(Type));
        type->kind = TypeKind.STRUCTURE;
        type = StructSpecifier(child, type);
    }
    return type;
}

Type StructSpecifier(Tree *node, Type type) {
    
}

void ExtDecList(TreeNode *node, Type type) {

}