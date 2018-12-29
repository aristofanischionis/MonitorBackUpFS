#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "Headerfiles/functions.h"
#include "Headerfiles/defines.h"


#define isDIR(X) ((X) == DT_DIR ? (1) : (0))
#define isREG(X) ((X) == DT_REG ? (1) : (0))

int isDot(char *name) {
    return (((!strcmp(name, ".")) || (!strcmp(name, ".."))) ? (1) : (0) );
}

void readDirectory(char *filename, List **list, TreeNode *previous) {
    DIR *file_ptr;
    Data data;
    struct dirent *direntp;
    char path[MAX];
    if ((file_ptr = opendir(filename)) == NULL) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    } else {
        while ((direntp = readdir(file_ptr)) != NULL) {
            sprintf(path, "%s/%s",filename, direntp->d_name);
            strcpy(data.name, path);
            // printf("inode %d of the entry %s\n" , \
            // (int)direntp->d_ino, direntp->d_name);
            // if this is a file (not a pipe, socket, etc) add an inode
            if(isREG(direntp->d_type)) {
                INode *node = addINode(list, path);               
                data.inode = node;
                addKid(previous, data);
            }
            // check if direntp is a directory 
            if (isDIR(direntp->d_type)) {
                // check if it a new directory (not a . or ..)
                if (!isDot(direntp->d_name)) {
                    // if it is a new directory add 
                    INode *node = addINode(list, path);
                    data.inode = node;
                    TreeNode *treenode = addKid(previous, data);
                    readDirectory(path, list, treenode);
                }
            }
        }
        closedir(file_ptr);
    }
}
