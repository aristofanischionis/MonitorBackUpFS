#ifndef INODE_HEADER
#define INODE_HEADER

#include <sys/types.h>
#include <time.h>
#include "namelist.h"

typedef struct INode {
    int inodeNum;
    time_t modDate;
    off_t size;
    NameList *names;
    int nameCount;
    int modified;
    struct INode *copy;  // only for files in source
    struct INode *next;
} INode;

#endif
