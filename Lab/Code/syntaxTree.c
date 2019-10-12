#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"syntaxTree.h"

/*
TreeNode *createNewNode(int lineno, char *name) {
    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
    newNode->childNum = 0;
    newNode->children = NULL;
    newNode->lineno = lineno;
    newNode->name = name;
    newNode->next = NULL;
    return newNode;
}
*/

TreeNode *createNewNodeNot(char* name, int lineno){ // for type=NOTEND
    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));

    newNode->type = NOTEND;
    newNode->children = NULL;
    newNode->lineno = lineno;
    strcpy(newNode->name, name);
    newNode->next = NULL;

    return newNode;
}

TreeNode *createNewNodeID(char* name){  // for type=IDEND
    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));

    newNode->type = IDEND;
    newNode->children = NULL;
    newNode->lineno = 0;
    strcpy(newNode->name, name);
    newNode->next = NULL;

    return newNode;
}

TreeNode *createNewNodeVal(char* name, char* val){ // for type=VALEND
    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));

    newNode->type = VALEND;
    newNode->children = NULL;
    newNode->lineno = 0;
    strcpy(newNode->name, name);
    strcpy(newNode->val, val);
    newNode->next = NULL;

    return newNode;
}

void insertNode(TreeNode *parent, TreeNode *child) {
    if(!parent->children){
	parent->children = child;
    }else{
	TreeNode *temp = parent->children;
	while(temp->next){
	    temp = temp->next;
	}
	temp->next = child;
    }

/*
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
*/
}

void printTree(TreeNode *root) {
    //TODO
}