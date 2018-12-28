#ifndef FUNCTIONS_HEADER
#define FUNCTIONS_HEADER

#include "list.h"

int isDirectory(const char *path);
int isDot(char *name);
void readDirectory(char *filename, List **list);

#endif
