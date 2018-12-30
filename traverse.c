#include <string.h>
#include <stdio.h>
#include "Headerfiles/traverse.h"

void traverseTrees(Tree **sourceTree, Tree **backupTree) {
    int firstCase = returnCase((*sourceTree)->root->kid, (*backupTree)->root->kid);
    if (firstCase == 1) {
        addKid((*backupTree)->root, (*sourceTree)->root->kid->data);    
    }
    recurseAlgorithm((*sourceTree)->root->kid, (*backupTree)->root->kid);

    // recurseAlgorithm((*sourceTree)->root->kid, (*backupTree)->root->kid);
}

void recurseAlgorithm(TreeNode *sourceNode, TreeNode *backupNode) {
    if (sourceNode == NULL && backupNode == NULL) {
        return;
    }
    int siblingCase = returnCase(sourceNode->sibling, backupNode->sibling);
    if (siblingCase == 1) {
        TreeNode *backupSibling = addSiblingSorted(backupNode, sourceNode->sibling->data);
        recurseAlgorithm(sourceNode->sibling, backupSibling);
    } else {
        return;
    }
    int kidCase = returnCase(sourceNode->kid, backupNode->kid);
    if (kidCase == 1) {
        TreeNode *backupKid = addKid(backupNode, sourceNode->kid->data);  
        recurseAlgorithm(sourceNode->kid, backupKid);   
    } else {
        return;
    }
    
}

int returnCase(TreeNode *sourceNode, TreeNode *backupNode) {

    if (sourceNode == NULL && backupNode == NULL) {
        printf("both nodes null return 0\n");
        return 0;
    } else if (backupNode == NULL) {
        printf("backup node is null need to add node\n");
        return 1;

    }
    // if katalogos stin pigi oxi ston proorismo
    // if katalogos oxi stin pigi alla ston proorismo
    // if arxeio stin pigi oxi ston proorismo
    // if arxeio oxi stin pigi alla ston proorismo
    // if arxeio kai stin pigi kai ston proorismo

    /*
    if (sourceNode == NULL && backupNode == NULL) {
        // kai ta dio arxeia stamatane sto sosto simeio opote min kaneis tipota
    }

    // Prosthethike katalogos/arxeio sto telos tis listas me ta siblings
    else if (backupNode == NULL) {
        // if sourceNode is a directory
        if (sourceNode->kid != NULL) {
            if (strcmp(sourceNode->data.name == backupNode->data.name)) {
                // ...
            }       
        }
        // eite eimsate stin periptosi o katalogos stin pigi alla oxi ston proorismo 
        // (diladi enas katalogos prostethike kai prepei na prostethei kai ston backup)
    } 

    // Afairethike enas katalogos/arxeio apo to telos tis listas
    else if (sourceNode == NULL) {

    }

    // Enas komvos prostethike i afairethike sto endiameso
    else if (strcmp(sourceNode->data.name, backupNode->data.name)) {
        // Enas komvos prostethike
        if (sourceNode->sibling && strcmp(sourceNode->sibling->data.name, backupNode->data.name)) {
            // return 1 which means a child needs to be added
            return 1;  
        }
        // Enas komvos afairethike
        else if (strcmp(backupNode->sibling && sourceNode->data.name, backupNode->sibling->data.name)) {

        }
        
    }

    // To arxeio einai kai stin pigi kai ston proorismo
    else if (!strcmp(sourceNode->data.name, backupNode->data.name)) {
        
        // sigkrino ta inodes ...
        // (kai se if tsekaro an einai arxeio)
    }
    
    
    
    // if sourceNode is a catalog
    else {
        
    }

    // if (sourceNode != NULL) {
    //     traverseAlgorithm(node->sibling);
    //     traverseAlgorithm(node->kid);
    // }
    */
 

}