#ifndef __SYNTAXTREE_H__
#define __SYNTAXTREE_H__

typedef enum {
  NOTEND,  // non-terminal
  IDEND,   // terminal without value
  VALEND   // terminal with value
} NODE_TYPE;

typedef struct treeNode {
    struct treeNode *children;//CHILD NODES
//    int childNum; //NUMBER OF CHILDREN
    int lineno; //LOCATION

    NODE_TYPE type; //TYPE

    // char val[32]; //VALUE
    union {
        int val_int;
        float val_float;
        char val_str[32];
    } attr;

    char name[32]; //NAME

    struct treeNode *next; //WHEN REFERRED AS A CHILD, THIS REPRESENTS ITS SIBLING
} TreeNode;

TreeNode *createNewNodeNot(char* name, int lineno); // for type=NOTEND
TreeNode *createNewNodeID(char* name, int lineno);  // for type=IDEND
TreeNode *createNewNodeVal(char* name, char* val, int lineno); // for type=VALEND
void insertNode(TreeNode *parent, TreeNode *child);
void printTree(TreeNode *root, int layer);

#endif
