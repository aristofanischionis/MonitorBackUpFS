
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
Tree* initializeTree(Data data);
TreeNode* newNode(Data data);
TreeNode* addSiblingSorted(TreeNode *node, Data data);
TreeNode* addKid(TreeNode* node, Data data);
TreeNode* finderKids(TreeNode *toCheck, char *name);
TreeNode* search(Tree *root, Data data);
int removeNode(Tree* root, TreeNode* node);
void printTree(Tree *tree);
void printBranch(TreeNode *node, char *parentName);
TreeNode* checker(TreeNode* node, Data data);