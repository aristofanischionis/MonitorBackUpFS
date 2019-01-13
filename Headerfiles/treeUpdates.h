#ifndef TREEUPDATES_HEADER
#define TREEUPDATES_HEADER

#include "functions.h"
#include "list.h"

// Functions that update the trees depending on changes on the file system
void updateTreeCreate(char *path, Tree **sourceTree, List *sourceList);
void updateTreeDelete(char *path, Tree **sourceTree, List *sourceList);
void updateTreeDeleteSelf(char *path, Tree **sourceTree, List *sourceList);
void updateTreeModify(char *path, Tree **sourceTree, List *sourceList);
void updateTreeMoveToInsideHierarchy(char *path, Tree **sourceTree,
                                     List *sourceList, INode *inode);
void updateTreeMoveFrom(char *path, Tree **sourceTree, List *sourceList);
void readDirectory(char *filename, List **list, TreeNode *previous);

#endif