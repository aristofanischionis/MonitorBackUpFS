#ifndef FUNCTIONS_HEADER
#define FUNCTIONS_HEADER

#include "tree.h"
#include "list.h"
#include "defines.h"

int isDot(char *name);
void readDirectory(char *filename, List **list, TreeNode *previous);
void makeBackup(char *source, char *backup);

#endif
