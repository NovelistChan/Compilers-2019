typedef enum {
    END,
    ID,
    VAL
} NodeType;

typedef struct treeNode{
    TreeNode *children;//CHILD NODES
    int childNum;//NUMBER OF CHILDREN
    int lineno;//LOCATION
    char val[32];//VALUE
    /*union 
    {
        /* data *//*
        int val_int;
        float val_float;
        char val_string[32];
    } attr;//DATA VALUE ATTRIBUTION
    */
    char *name;//NAME(TYPE)
    TreeNode *next;//WHEN REFERRED AS A CHILD, THIS REPRESENTS ITS SIBLING
} TreeNode;

TreeNode *createNewNode(char* name, int lineno);
void insertNode(TreeNode *parent, TreeNode *child);
void printTree(TreeNode *root);

