#ifndef TRAVERSE_HEADER
#define TRAVERSE_HEADER

#include "list.h"
#include "tree.h"

void traverseTrees(char *sourceBase, Tree *backupTree, List **sourceINodes,
                   List **backupINodes, TreeNode *sourceNode,
                   TreeNode *backupNode);
int returnCase(TreeNode *sourceNode, TreeNode *backupNode);

#endif