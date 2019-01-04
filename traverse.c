#include <string.h>
#include <stdio.h>
#include "Headerfiles/defines.h"
#include "Headerfiles/traverse.h"

void traverseTrees(Tree **sourceTree, Tree **backupTree) {
    while (strcmp((*sourceTree)->root->kid->data.name, (*backupTree)->root->kid->data.name)) {
        // check case for first kid of roots
        int firstCase = returnCase((*sourceTree)->root->kid, (*backupTree)->root->kid);
        if (firstCase == FILE_IN_SOURCE || firstCase == DIR_IN_SOURCE) {  
            addKid((*backupTree)->root, (*sourceTree)->root->kid->data);  
        }
        else if (firstCase == FILE_IN_BACKUP || firstCase == DIR_IN_BACKUP) {
            deleteNode(*backupTree, (*backupTree)->root->kid); 
        } 
    }
    recurseAlgorithm(*sourceTree, *backupTree, (*sourceTree)->root->kid, (*backupTree)->root->kid);   
}

void recurseAlgorithm(Tree *sourceTree, Tree *backupTree,
                      TreeNode *sourceNode, TreeNode *backupNode) {
    if (sourceNode == NULL && backupNode == NULL) {
        return;
    }

    int siblingCase = returnCase(sourceNode->sibling, backupNode->sibling);
    if (siblingCase == FILE_IN_SOURCE || siblingCase == DIR_IN_SOURCE) {
        TreeNode *backupSibling = addSiblingSorted(backupNode, sourceNode->sibling->data);
        recurseAlgorithm(sourceTree, backupTree, sourceNode->sibling, backupSibling);
    } 
    else if (siblingCase == FILE_IN_BACKUP || siblingCase == DIR_IN_BACKUP) {
        TreeNode * backupPrev = deleteNode(backupTree, backupNode->sibling);
        recurseAlgorithm(sourceTree, backupTree, sourceNode, backupPrev);
    }
    else if (siblingCase == FILE_IN_BOTH) {
        recurseAlgorithm(sourceTree, backupTree, sourceNode->sibling, backupNode->sibling);
    }  

    int kidCase = returnCase(sourceNode->kid, backupNode->kid);
    if (kidCase == FILE_IN_SOURCE || kidCase == DIR_IN_SOURCE) {
        TreeNode *backupKid = addKid(backupNode, sourceNode->kid->data);  
        recurseAlgorithm(sourceTree, backupTree, sourceNode->kid, backupKid);   
    }
    else if (kidCase == FILE_IN_BACKUP || kidCase == DIR_IN_BACKUP) {
        TreeNode * backupPrev = deleteNode(backupTree, backupNode->kid);
        recurseAlgorithm(sourceTree, backupTree, sourceNode, backupPrev);
    } 
    else if (kidCase == FILE_IN_BOTH) {
        recurseAlgorithm(sourceTree, backupTree, sourceNode->kid, backupNode->kid);
    }  
}

// Returns different integers depending on which case the branches are
int returnCase(TreeNode *sourceNode, TreeNode *backupNode) {
    printf("%s      %s       ", sourceNode->data.name, backupNode->data.name);
    if (sourceNode == NULL && backupNode == NULL) {
        printf("is null\n");
        return 0;
    }
    // if there is a node (directory/file) in source (at the end of the list of
    // siblings or kids) but not in backup
    else if (backupNode == NULL) {
        // check if missing node is a file or a directory        
        if (sourceNode->kid == NULL) {  // if it is a file
            printf("file in source\n");
            return FILE_IN_SOURCE;
        } else {                        // if it is a directory
            printf("dir in source\n");
            return DIR_IN_SOURCE;
        }     
    }
    // if there is a node (directory/file) in backup (at the end of the list of
    // siblings or kids) but not in source
    else if (sourceNode == NULL) {
        // check if missing node is a file or a directory        
        if (backupNode->kid == NULL) {  // if it is a file
            printf("file in backup\n");
            return FILE_IN_BACKUP;
        } else {                        // if it is a directory
            printf("dir in backup\n");
            return DIR_IN_BACKUP;
        }  
    }
    // if there is a node in both 
    else {
        // check if this node has the same name
        int strCompare = strcmp(sourceNode->data.name, backupNode->data.name);
        // the file is in both trees
        if (strCompare == 0) {
            printf("file/dir in both\n");
            return FILE_IN_BOTH;
            // check if both files are identical
        }
        // the file is in backup
        else if (strCompare > 0){
            printf("file/dir in backup\n");
            return FILE_IN_BACKUP;
        } 
        // the file is in source
        else if (strCompare < 0) {
            printf("file/dir in source\n");
            return FILE_IN_SOURCE;
        }
    }
}