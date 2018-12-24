#ifndef LIST_HEADER
#define LIST_HEADER

#include "inode.h"
#include <time.h>

typedef struct List {
    INode *head;
} List;

List * initializeList(void);
int addSourceNode(List **list, char *path);
int addCopyNode(List **list, int inodeNum, char *path, int inodeNumOriginal);
INode * searchForNode(List *list, int inodeNum);
void printNodes(List *list);
int deleteNode(List **list, int inodeNum);

#endif
