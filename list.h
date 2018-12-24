#ifndef LIST_HEADER
#define LIST_HEADER

#include "inode.h"
#include <time.h>

typedef struct List {
    INode *head;
} List;

List * initializeList(void);
int addSourceNode(List **list, char *path);
int addCopyNode(List **list, char *path, INode *originalNode);
INode * searchForNode(List *list, char *path);
void printNodes(List *list);
int delete_vertex(List **list, char *path);

#endif
