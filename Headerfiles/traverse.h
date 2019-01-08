#ifndef TRAVERSE_HEADER
#define TRAVERSE_HEADER

#include "tree.h"
#include "list.h"

void traverseTrees(Tree **sourceTree, Tree **backupTree, List **sourceINodes,
                   List **backupINodes);
void recurseAlgorithm(Tree *backupTree, List **sourceINodes,
                      List **backupINodes, TreeNode *sourceNode,
                      TreeNode *backupNode);
int returnCase(TreeNode *sourceNode, TreeNode *backupNode);

#endif