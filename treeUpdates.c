#include <sys/inotify.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <libgen.h>
#include "Headerfiles/treeUpdates.h"

void updateTreeCreate(char *path, Tree **sourceTree, List *sourceList) {
    // make a copy of path to pass so that it doesn't change when passed to
    // functions
    char *pathCopy = malloc(strlen(path) + 1);
    strcpy(pathCopy, path);
    TreeNode *previous = searchByPath((*sourceTree)->root, dirname(pathCopy));
    strcpy(pathCopy, path);
    Data data;
    strcpy(data.path, path);
    strcpy(data.name, basename(pathCopy));
    data.inode = NULL;
    // if a file was added, add an inode
    if (!isDirectory(path)) {
        INode *node = addINode(&sourceList, path);
        data.inode = node;
    }
    addKid(previous, data);
    return;
}

void updateTreeDelete(char *path, Tree **sourceTree, List *sourceList) {
    // if a file will be deleted, unlink
    // I can't call isDirectory because the real path doesn't exist in the source file
    // so I check if the inode of tis tree node is null (directories don't have an inode)
    TreeNode *sourceNode = searchByPath((*sourceTree)->root, path);
    if (sourceNode != NULL && sourceNode->data.inode != NULL) {
        deleteINode(&sourceList, sourceNode->data.inode->inodeNum,
                    sourceNode->data.name);
        deleteNode(*sourceTree, sourceNode);
    }
    
    return;
}

void updateTreeDeleteSelf(char *path, Tree **sourceTree, List *sourceList) {   
    // it concerns only a catalog
    printf("file delete\n");
    // I can't call isDirectory because the real path doesn't exist in the source file
    // so I check if the inode of tis tree node is null (directories don't have an inode)
    TreeNode *sourceNode = searchByPath((*sourceTree)->root, path);
    if (sourceNode != NULL && sourceNode->data.inode == NULL) {
        printf("dic delete\n");
        TreeNode *nodeFound = searchByPath((*sourceTree)->root, path);
        deleteNode(*sourceTree, nodeFound);
    }
    return;
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
            // remove '/' character if it exists at the end of the filename
            if (filename[strlen(filename)-1] == '/') {
                filename[strlen(filename)-1] = 0;
            }
            sprintf(path, "%s/%s", filename, direntp->d_name);
            strcpy(data.name, direntp->d_name);
            strcpy(data.path, path);
            // if this is a file (not a pipe, socket, etc) add an inode
            if (isREG(direntp->d_type)) {
                INode *node = addINode(list, path);
                data.inode = node;
                addKid(previous, data);
            }
            // check if direntp is a directory
            if (isDIR(direntp->d_type)) {
                // check if it a new directory (not a . or ..)
                if (!isDot(direntp->d_name)) {
                    // if it is a new directory add it only to the tree
                    TreeNode *treenode = addKid(previous, data);
                    readDirectory(path, list, treenode);
                }
            }
        }
        closedir(file_ptr);
    }
}