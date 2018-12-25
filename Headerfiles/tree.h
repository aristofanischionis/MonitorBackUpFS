
typedef struct  iNodeData{
    int num;
} iNodeData;

typedef struct Data {
    char name[30];
    iNodeData *myData;
} Data;

typedef struct  TreeNode {
    Data data;
    struct TreeNode *kid;
    struct TreeNode *sibling;
} TreeNode;

typedef struct Tree {
    TreeNode *root;
} Tree;

// functions
TreeNode* newNode(Data data);
TreeNode* addSibling(TreeNode *node, Data data);
TreeNode* addKid(TreeNode* node, Data data);
TreeNode* finderKids(TreeNode *toCheck, char *name);
TreeNode* search(TreeNode* root, Data data);
TreeNode* removeNode(TreeNode* root, TreeNode* node);
void printTree(TreeNode* root, int space);
