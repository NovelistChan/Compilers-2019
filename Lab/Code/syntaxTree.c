#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"syntaxTree.h"

TreeNode *createNewNode(char *name, int lineno) {
    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
    newNode->childNum = 0;
    newNode->children = NULL;
    newNode->lineno = lineno;
    newNode->name = name;
    newNode->next = NULL;
}

void insertNode(TreeNode *parent, TreeNode *child) {
    if (parent->childNum == 0) {
        parent->children = child;
        parent->childNum++;
    } else {
        TreeNode *p = parent->children;
        for (int i = 0; i < parent->childNum; i++) 
            p = p->next;
        p->next = child;
        parent->childNum++;
    }
}

void printTree(TreeNode *root) {
    //TODO
}
