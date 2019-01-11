#ifndef TREEUPDATES_HEADER
#define TREEUPDATES_HEADER

#include "list.h"
#include "functions.h"

// Functions that update the trees depending on changes on the file system
void updateTreeCreate(char *path, Tree **sourceTree, List *sourceList);
void updateTreeDelete(char *path, Tree **sourceTree, List *sourceList);
void updateTreeDeleteSelf(char *path, Tree **sourceTree, List *sourceList);
// void updateSourceTree(struct inotify_event *event, char *path,
//                       Tree **sourceTree, List *sourceList);
void readDirectory(char *filename, List **list, TreeNode *previous);

#endif