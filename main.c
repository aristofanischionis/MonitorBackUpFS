#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/functions.h"
#include "Headerfiles/inotifyFunctions.h"
#include "Headerfiles/list.h"
#include "Headerfiles/traverse.h"
#include "Headerfiles/tree.h"
#include "Headerfiles/treeUpdates.h"

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        perror("Not enough arguments provided\n");
        exit(EXIT_FAILURE);
    }

    char *sourceFilename = malloc(strlen(argv[1]) + 1);
    strcpy(sourceFilename, argv[1]);
    char *backupFilename = malloc(strlen(argv[2]) + 1);
    strcpy(backupFilename, argv[2]);

    Data sourceData, backupData;
    // remove '\' character if it exists at the end of the filenames
    char *sourceFileCopy = malloc(strlen(sourceFilename) + 1);
    strcpy(sourceFileCopy, sourceFilename);
    char *backupFileCopy = malloc(strlen(backupFilename) + 1);
    strcpy(backupFileCopy, backupFilename);
    if (sourceFileCopy[strlen(sourceFileCopy) - 1] == '/') {
        sourceFileCopy[strlen(sourceFileCopy) - 1] = 0;
    }
    if (backupFileCopy[strlen(backupFileCopy) - 1] == '/') {
        backupFileCopy[strlen(backupFileCopy) - 1] = 0;
    }
    
    strcpy(sourceData.name, sourceFileCopy);
    strcpy(backupData.name, backupFileCopy);
    strcpy(sourceData.path, sourceFileCopy);
    strcpy(backupData.path, backupFileCopy);

    List *sourceINodes = initializeList();
    List *backupINodes = initializeList();

    Tree *sourceTree = initializeTree(sourceData);
    Tree *backupTree = initializeTree(backupData);
    
    // make backup folder if it doesn't exist
    makeBackup(sourceFilename, backupFilename);

    strcpy(sourceFileCopy, sourceFilename);
    strcpy(backupFileCopy, backupFilename);
    // read both directories to initialize data in tree structures
    readDirectory(sourceFileCopy, &sourceINodes, sourceTree->root);
    readDirectory(backupFileCopy, &backupINodes, backupTree->root);

    traverseTrees(sourceTree->root->data.path, backupTree, &sourceINodes,
                  &backupINodes, sourceTree->root, backupTree->root);
    printStructures(sourceTree, backupTree, sourceINodes, backupINodes);
    inotifyCode(sourceFileCopy, backupFilename, sourceINodes, backupINodes,
                &sourceTree, &backupTree);

    // Free allocated memory
    deleteNode(sourceTree, sourceTree->root);
    deleteNode(backupTree, backupTree->root);
    deleteList(&sourceINodes);
    deleteList(&backupINodes);
    free(sourceFilename);
    free(backupFilename);
    free(sourceTree->root);
    free(backupTree->root);
    free(sourceTree);
    free(backupTree);

    return 0;
}
