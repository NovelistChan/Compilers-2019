#include "semantic.h"

void semanticAnalysis(){
    Program(root);
}

void Program(TreeNode *node){
    ExtDefList(node);
}

void ExtDefList(TreeNode *node){
    TreeNode *child = node->children;
    if(child){
        ExtDef(child);
        ExtDefList(child->next);
    }
}

void ExtDef(TreeNode *node){
    TreeNode *child = node->children;
    Type type = Specifier(child);

    child = child->next;
    if(!strcmp(child->name, "ExtDecList")){
        // TODO
    }else if(!strcmp(child->name, "SEMI")){
        // TODO
    }else{
        // TODO
    }
}