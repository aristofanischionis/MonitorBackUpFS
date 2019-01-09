#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Headerfiles/defines.h"
#include "Headerfiles/traverse.h"
#include "Headerfiles/functions.h"

// Traverse both trees at the same time (node by node)
// Look at defines.h for info about defined macros
void traverseTrees(Tree *backupTree, List **sourceINodes,
                      List **backupINodes, TreeNode *sourceNode,
                      TreeNode *backupNode) {
    if (sourceNode == NULL && backupNode == NULL) {
        return;
    }

    int kidCase = returnCase(sourceNode->kid, backupNode->kid);

    if (kidCase == FILE_IN_SOURCE ) {
        TreeNode *backupKid = addKid(backupNode, sourceNode->kid->data); 
        // make backup path same as source but with different root folder
        char *path = backupPath(sourceNode->kid->data.path, backupTree->root->data.name); 
        // link it in the list of inodes after creating it
        backupKid->data.inode = addINode(backupINodes, path); 
        strcpy(backupKid->data.path, path); 
        // make copy pointer of source node point to backup
        sourceNode->kid->data.inode->copy = backupKid->data.inode;
        traverseTrees(backupTree, sourceINodes, backupINodes, sourceNode->kid, backupKid);   
    }
    else if (kidCase == DIR_IN_SOURCE) {
        TreeNode *backupKid = addKid(backupNode, sourceNode->kid->data);  
        traverseTrees(backupTree, sourceINodes, backupINodes, sourceNode->kid, backupKid); 
    }
    else if (kidCase == FILE_IN_BACKUP) {
        // unlink it in the list of inodes before deleting it from the tree
        deleteINode(backupINodes, backupNode->kid->data.inode->inodeNum, backupNode->kid->data.name);
        TreeNode * backupPrev = deleteNode(backupTree, backupNode->kid);
        traverseTrees(backupTree, sourceINodes, backupINodes, sourceNode, backupPrev);
    } 
    else if (kidCase == DIR_IN_BACKUP) {
        TreeNode * backupPrev = deleteNode(backupTree, backupNode->kid);
        traverseTrees(backupTree, sourceINodes, backupINodes, sourceNode, backupPrev);
    }
    else if (kidCase == FILE_IN_BOTH) {
        // if file has been modified update backup inode struct info
        if (sourceNode->kid->data.inode->modDate != backupNode->kid->data.inode->modDate 
        || sourceNode->kid->data.inode->size != backupNode->kid->data.inode->size) {
            backupNode->kid->data.inode->modDate = sourceNode->kid->data.inode->modDate;
            backupNode->kid->data.inode->size = sourceNode->kid->data.inode->size;
            backupNode->kid->data.inode->modified = 1;
        }
        traverseTrees(backupTree, sourceINodes, backupINodes, sourceNode->kid, backupNode->kid);
    }  
    else if (kidCase == DIR_IN_BOTH) {
        traverseTrees(backupTree, sourceINodes, backupINodes, sourceNode->kid, backupNode->kid);
    } 

    int siblingCase = returnCase(sourceNode->sibling, backupNode->sibling);
    
    if (siblingCase == FILE_IN_SOURCE) {
        TreeNode *backupSibling = addSiblingSorted(backupNode, sourceNode->sibling->data);
        // make backup path same as source but with different root folder
        char *path = backupPath(sourceNode->sibling->data.path, backupTree->root->data.name); 
        // link it in the list of inodes after creating it
        backupSibling->data.inode = addINode(backupINodes, path);  
        strcpy(backupSibling->data.path, path); 
        // make copy pointer of source node point to backup
        sourceNode->sibling->data.inode->copy = backupSibling->data.inode;
        traverseTrees(backupTree, sourceINodes, backupINodes, sourceNode->sibling, backupSibling);
    } 
    else if (siblingCase == DIR_IN_SOURCE){
        TreeNode *backupSibling = addSiblingSorted(backupNode, sourceNode->sibling->data);
        traverseTrees(backupTree, sourceINodes, backupINodes, sourceNode->sibling, backupSibling);
    }
    else if (siblingCase == FILE_IN_BACKUP) {
        TreeNode * backupPrev = deleteNode(backupTree, backupNode->sibling);
        // unlink it in the list of inodes before deleting it from the tree
        deleteINode(backupINodes, backupNode->sibling->data.inode->inodeNum, backupNode->sibling->data.name);
        traverseTrees(backupTree, sourceINodes, backupINodes, sourceNode, backupPrev);
    }
    else if (siblingCase == DIR_IN_BACKUP) {
        TreeNode * backupPrev = deleteNode(backupTree, backupNode->sibling);
        traverseTrees(backupTree, sourceINodes, backupINodes, sourceNode, backupPrev);
    }
    else if (siblingCase == FILE_IN_BOTH) {
        // if file has been modified update backup inode struct info
        if (sourceNode->sibling->data.inode->modDate != backupNode->sibling->data.inode->modDate 
        || sourceNode->sibling->data.inode->size != backupNode->sibling->data.inode->size) {
            backupNode->sibling->data.inode->modDate = sourceNode->sibling->data.inode->modDate;
            backupNode->sibling->data.inode->size = sourceNode->sibling->data.inode->size;
            backupNode->sibling->data.inode->modified = 1;
        }
        traverseTrees(backupTree, sourceINodes, backupINodes, sourceNode->sibling, backupNode->sibling);
    } 
    else if (siblingCase == DIR_IN_BOTH) {
        traverseTrees(backupTree, sourceINodes, backupINodes, sourceNode->sibling, backupNode->sibling);
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
        // check if both are files
        if (sourceNode->kid == NULL && backupNode->kid == NULL) {
            // the file is in both trees
            if (strCompare == 0) {
                printf("file in both\n");
                return FILE_IN_BOTH;
            }
            // the file is in backup
            else if (strCompare > 0){
                printf("file in backup\n");
                return FILE_IN_BACKUP;
            } 
            // the file is in source
            else if (strCompare < 0) {
                printf("file in source\n");
                return FILE_IN_SOURCE;
            }
        } else {
            // the directory is in both trees
            if (strCompare == 0) {
                printf("dir in both\n");
                return DIR_IN_BOTH;
            }
            // the file is in backup
            else if (strCompare > 0){
                printf("dir in backup\n");
                return DIR_IN_BACKUP;
            } 
            // the file is in source
            else if (strCompare < 0) {
                printf("dir in source\n");
                return DIR_IN_SOURCE;
            }
        }
    }
}