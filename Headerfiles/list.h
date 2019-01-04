#ifndef LIST_HEADER
#define LIST_HEADER

#include "inode.h"
#include <time.h>

typedef struct List {
    INode *head;
} List;

List * initializeList(void);
INode * addINode(List **list, char *path);
int pointToCopy(List *list, INode *inode, int inodeNumCopy);
INode * searchForINode(List *list, int inodeNum);
void printINodes(List *list);
int deleteINode(List **list, int inodeNum, char *name);
INode * normalDelete(List **list, int inodeNum);
void deleteList(List **list);

#endif
