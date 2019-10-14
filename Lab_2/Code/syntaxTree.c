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
    newNode->next = NULL;

    //strcpy(newNode->name, name);
    //strcpy(newNode->val, val);
    if(!strcmp(name, "DEC")){
        strcpy(newNode->name, "INT");
        newNode->attr.val_int = atoi(val);
    }
    else if(!strcmp(name, "HEX")){
        strcpy(newNode->name, "INT");
        newNode->attr.val_int = (int)strtol(val, NULL, 16);
    }
    else if(!strcmp(name, "OCT")){
        strcpy(newNode->name, "INT");
        newNode->attr.val_int = (int)strtol(val, NULL, 8);
    }
    else if(!strcmp(name, "FLOAT")){
        strcpy(newNode->name, "FLOAT");
        newNode->attr.val_float = atof(val);
    }
    else{
        strcpy(newNode->name, name);
        strcpy(newNode->attr.val_str, val);
    }

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

void printTree(TreeNode *root, int layer) {
    if(!root)	return;

    for(int i=0;i<layer;i++){
	printf("  ");
    }

    switch (root->type) {
	case NOTEND:{
	    printf("%s (%d)\n", root->name, root->lineno);
	}break;
	case IDEND:{
	    printf("%s\n", root->name);
	}break;
	case VALEND:{
        printf("%s: ", root->name);
        if(!strcmp(root->name, "INT"))
	        printf("%d\n", root->attr.val_int);
        else if(!strcmp(root->name, "FLOAT"))
            printf("%f\n", root->attr.val_float);
        else
            printf("%s\n", root->attr.val_str);
        
	}break;
	default:{
	    printf("Unexpected error in printTree()");
	    exit(-1);
	}
    }

    TreeNode *temp = root->children;
    while(temp){
	printTree(temp, layer+1);
	temp = temp->next;
    }
}
