#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/tree.h"
#include "Headerfiles/functions.h"
#include "Headerfiles/list.h"

int main(int argc, char const *argv[]) {

    if(argc < 3){
        perror("Not enough arguments provided\n");
    	exit(EXIT_FAILURE);
    }

    char *sourceFilename = malloc(strlen(argv[1])+1);
    strcpy(sourceFilename, argv[1]);
    char *backupFilename = malloc(strlen(argv[2])+1);
    strcpy(backupFilename, argv[2]);

    Data sourceData, backupData;
    strcpy(sourceData.name, sourceFilename);
    strcpy(backupData.name, backupFilename);

    List *sourceINodes = initializeList();
    List *backupINodes = initializeList();

    INode *sourceNode = addINode(&sourceINodes, sourceFilename);
    INode *backupNode = addINode(&backupINodes, backupFilename);
    sourceData.inode = sourceNode;
    backupData.inode = backupNode;

    Tree *sourceTree = initializeTree(sourceData);
    Tree *backupTree = initializeTree(backupData);

    readDirectory(sourceFilename, &sourceINodes, sourceTree->root);
    readDirectory(backupFilename, &backupINodes, backupTree->root);

    // Print structures
    printf("Source Tree:\n");
    printTree(sourceTree);
    printf("Backup Tree:\n");
    printTree(backupTree);
    printf("Source iNodes:\n");
    printINodes(sourceINodes);
    printf("Backup iNodes:\n");
    printINodes(backupINodes);

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
