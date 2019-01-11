#ifndef NAMELIST_HEADER
#define NAMELIST_HEADER

#include "defines.h"

typedef struct NameListNode {
    char name[MAX];
    struct NameListNode *next;
} NameListNode;

typedef struct NameList {
    NameListNode *head;
} NameList;

NameList *initializeNameList(void);
int addName(NameList **list, char *name);
NameListNode *searchForName(NameList *list, char *name);
void printNames(NameList *list);
int deleteName(NameList **list, char *name);
void deleteNameList(NameList **list);

#endif
