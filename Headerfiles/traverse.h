#ifndef TRAVERSE_HEADER
#define TRAVERSE_HEADER

#include "tree.h"

// int traverseAlgorithm(TreeNode *sourceNode, TreeNode *backupNode);
void traverseTrees(Tree **sourceTree, Tree **backupTree);
void recurseAlgorithm(TreeNode *sourceNode, TreeNode *backupNode);
int returnCase(TreeNode *sourceNode, TreeNode *backupNode);


#endif