#ifndef NAMELIST_HEADER
#define NAMELIST_HEADER

typedef struct NameListNode {
    char name[20];
    struct NameListNode *next;
} NameListNode;

typedef struct NameList {
    NameListNode *head;
} NameList;

NameList * initializeNameList(void);
int addName(NameList **list, char *name);
NameListNode * searchForName(NameList *list, char *name);
void printNames(NameList *list);
int deleteName(NameList **list, char *name);
void deleteNameList(NameList **list);

#endif
