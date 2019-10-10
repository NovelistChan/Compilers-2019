
typedef struct treeNode{
    TreeNode *children;//CHILD NODES
    int childNum;//NUMBER OF CHILDREN
    int lineno;//LOCATION
//    char *val;//VALUE
    union 
    {
        /* data */
        int val_int;
        float val_float;
        char val_string[32];
    } attr;//DATA VALUE ATTRIBUTION
    char *name;//NAME
    TreeNode *next;//WHEN REFERRED AS A CHILD, THIS REPRESENTS ITS SIBLING
} TreeNode;

TreeNode *createNewNode(int lineno, char* name);
void insertNode(TreeNode *parent, TreeNode *child);
void printTree(TreeNode *root);

