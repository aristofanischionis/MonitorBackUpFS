#ifndef FUNCTIONS_HEADER
#define FUNCTIONS_HEADER

#include "tree.h"
#include "list.h"
#include "defines.h"


#define isDIR(X) ((X) == DT_DIR ? (1) : (0))
#define isREG(X) ((X) == DT_REG ? (1) : (0))

int isDot(char *name);
void makeBackup(char *source, char *backup);
void makeDirectory(char *path, char *name);
char* backupPath(char* sourcePath, char* backupBase);
void copy(char *source, char *dest);
void fail(const char *message);

#endif
