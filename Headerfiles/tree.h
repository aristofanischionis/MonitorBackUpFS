#ifndef TREE_HEADER
#define TREE_HEADER

#include "defines.h"
#include "inode.h"

typedef struct Data {
    char name[MAX];
    char path[MAX];
    INode *inode;
} Data;

typedef struct TreeNode {
    Data data;
    struct TreeNode *kid;
    struct TreeNode *sibling;
} TreeNode;

typedef struct Tree {
    TreeNode *root;
} Tree;

// functions
Tree *initializeTree(Data data);
TreeNode *newNode(Data data);
TreeNode *addSiblingSorted(TreeNode *node, Data data);
TreeNode *addKid(TreeNode *node, Data data);
TreeNode *finderKids(TreeNode *toCheck, char *name);
TreeNode *searchByPath(TreeNode *node, char *path);
TreeNode *searchByINodeNum(TreeNode *node, int inodeNum, char *path);
void deleteKids(TreeNode *node);
TreeNode *findPrevious(TreeNode *node, Data data);
TreeNode *deleteNode(Tree *root, TreeNode *node);
void printTree(Tree *tree);
void printBranch(TreeNode *node, char *parentName);

#endif
