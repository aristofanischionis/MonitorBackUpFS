#ifndef INODE_HEADER
#define INODE_HEADER

#include <time.h>
#include <sys/types.h>

typedef struct INode {
    int inodeNum;
    time_t modDate;
    off_t size;
    // list with names
    int nameCount;
    struct INode *copy;    // only for files in source
    struct INode *next;
} INode;

#endif
