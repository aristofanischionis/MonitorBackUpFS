#include <string.h>
#include <stdio.h>
#include "Headerfiles/defines.h"
#include "Headerfiles/traverse.h"

void traverseTrees(Tree **sourceTree, Tree **backupTree) {
    // check case for first kid of roots
    int firstCase = returnCase((*sourceTree)->root->kid, (*backupTree)->root->kid);
    if (firstCase == 1) {
        addKid((*backupTree)->root, (*sourceTree)->root->kid->data);    
    }
    // call recurseAlgorithm for first kid of roots
    recurseAlgorithm((*sourceTree)->root->kid, (*backupTree)->root->kid);
}

void recurseAlgorithm(TreeNode *sourceNode, TreeNode *backupNode) {
    if (sourceNode == NULL && backupNode == NULL) { 
        return;
    }
    int siblingCase = returnCase(sourceNode->sibling, backupNode->sibling);
    if (siblingCase == FILE_IN_SOURCE || siblingCase == DIR_IN_SOURCE) {
        TreeNode *backupSibling = addSiblingSorted(backupNode, sourceNode->sibling->data);
        recurseAlgorithm(sourceNode->sibling, backupSibling);
    } 
    int kidCase = returnCase(sourceNode->kid, backupNode->kid);
    if (kidCase == FILE_IN_SOURCE || kidCase == DIR_IN_SOURCE) {
        TreeNode *backupKid = addKid(backupNode, sourceNode->kid->data);  
        recurseAlgorithm(sourceNode->kid, backupKid);   
    }   
}

// Returns different integers depending on which case the branches are
int returnCase(TreeNode *sourceNode, TreeNode *backupNode) {
    if (sourceNode == NULL && backupNode == NULL) {
        return 0;
    } 
    // if there is a node (directory/file) in source but not in backup
    else if (backupNode == NULL) {
        // check if missing node is a file or a directory        
        if (sourceNode->kid == NULL) {  // if it is a file
            return FILE_IN_SOURCE;
        } else {                        // if it is a directory
            return DIR_IN_SOURCE;
        }     
    }
    // if there is a node (directory/file) in backup but not in source
    else if (sourceNode == NULL) {
        // check if missing node is a file or a directory        
        if (sourceNode->kid == NULL) {  // if it is a file
            return FILE_IN_BACKUP;
        } else {                        // if it is a directory
            return DIR_IN_BACKUP;
        }  
    }
    // if there is a node in both 
    else {
        // check if this node has the same name
        if (!strcmp(sourceNode->data.name, backupNode->data.name)) {
            // check if both files are identical
        } else {
            // check if a node has been added or deleted
        }
    }
}