#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/tree.h"

// Initiaize tree with the given data as root node
Tree* initializeTree(Data data) {
    Tree *tree = NULL;
    tree = malloc(sizeof(Tree));
    tree->root = newNode(data);

    return tree;
}

// Initialize a new tree node with the given data
TreeNode* newNode(Data data) {
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));

    if (node) {
        node->data = data;
        node->sibling = NULL;
        node->kid = NULL;
        return node;
    }
    perror("Something went wrong while allocating newNode\n");
    exit(EXIT_FAILURE);
}

// Add a sibling to the tree level of the given node
TreeNode* addSiblingSorted(TreeNode *node, Data data) {
    // node should be the first node in this level
    TreeNode *prev = node;
    if (node == NULL) {
        return NULL;
    }
    // prev = node;
    // sort from a->z
    while (prev->sibling != NULL) {
        if(strcmp(prev->data.name, data.name) >= 0) break;
        prev = prev->sibling;
    }
    // at prev we now have the prev node
    TreeNode *new = newNode(data);
    // the previous' node sibling will be the new ones sibl
    new->sibling = prev->sibling;
    // previous' sibling becomes the new node
    prev->sibling = new;
    return new;

}

// Add a kid to the given node
TreeNode* addKid(TreeNode *node, Data data) {
    if (node == NULL) {
        return NULL;
    }
    // if there is already a kid, add it as a sibling to it
    // else add it as the first kid
    if (node->kid != NULL) {
        return addSiblingSorted(node->kid, data);
    } else {
        return (node->kid = newNode(data));
    }
}

TreeNode* checker(TreeNode* node, Data data){

    if((node == NULL) || (!strcmp(data.name, ""))) return NULL;
    if(!strcmp(node->data.name, data.name)) return node;

    if(node->kid != NULL) return checker(node->kid, data);
    if(node->sibling != NULL) return checker(node->sibling, data);
}

TreeNode* search(Tree *tree, Data data){
    // base case
    if ((tree == NULL) || (tree->root == NULL) || (!strcmp(tree->root->data.name, data.name))) {
        return tree->root;
    }
    TreeNode *node, *res;
    node = tree->root;
    res = checker(node, data);
    if(res == NULL){
        printf("Couldn't find the desired name : %s\n", data.name);
        return NULL;
    }
    return res;
}

void deleteKids(TreeNode* node) {
    // if the node is null (e.g. if delete was called for kids but there are no kids)
    // then return
    if (node == NULL) {
        return;
    }

    // if there are no siblings, call delete for the kids, else call delete for
    // the siblings
    if (node->sibling == NULL) {
        deleteKids(node->kid);
    } else {
        deleteKids(node->sibling);
        deleteKids(node->kid);
    }
    // if there are no more kids or siblings, delete this node
    free(node);
}

TreeNode *findPrevious(TreeNode *node, Data data){
    if((node == NULL) || (!strcmp(data.name, ""))) {
        return NULL;
    }

    if (node->kid != NULL) {
        if (!strcmp(node->kid->data.name, data.name)) {
            return node;
        }
    }
    if (node->sibling != NULL) {
        if (!strcmp(node->sibling->data.name, data.name)) {
            return node;
        }
    }

    TreeNode *res = findPrevious(node->sibling, data);
    // if node was found dont continue with recursion
    if (res != NULL) {
        return res;
    }
    // findPrevious(node->kid, data);
}

int deleteNode(Tree *tree, TreeNode *node) {

    if (node == NULL) {
        printf("Couldn't find the node to remove\n");
        return 1;
    }
    // delete all of its kids, so all the siblings of its kid
    deleteKids(node->kid);
    node->kid = NULL;

    // delete the node under review
    // find the previous node
    TreeNode *prev = findPrevious(tree->root, node->data);
    if (prev == NULL) {
        return 2;
    }
    if ((prev->kid->data.name != NULL) && (!strcmp(prev->kid->data.name, node->data.name))) {
        prev->kid = node->sibling;
    } else if ((prev->sibling->data.name != NULL) && (!strcmp(prev->sibling->data.name, node->data.name))) {
        prev->sibling = node->sibling;
    }
    free(node);

    return 0;
}

void printTree(Tree *tree) {
    printBranch(tree->root->kid, tree->root->data.name);
}

void printBranch(TreeNode *node, char *parentName){
    if (node != NULL) {
        printBranch(node->sibling, parentName);
        printBranch(node->kid, node->data.name);
        printf("%s in folder %s\n", node->data.name, parentName);
    }
}
