#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/tree.h"
#define COUNT 10

Tree* initializeTree(void) {
    Tree *tree = NULL;
    tree = malloc(sizeof(Tree));
    tree->root = NULL;

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
TreeNode* addSibling(TreeNode *node, Data data) {
    if (node == NULL) {
        return NULL;
    }
    while (node->sibling != NULL) {
        node = node->sibling;
    }

    return (node->sibling = newNode(data));
}

// Add a kid to the given node
TreeNode* addKid(TreeNode *node, Data data) {
    if (node == NULL) {
        return NULL;
    }
    // if there is already a kid, add it as a sibling to it
    // else add it as the first kid
    if (node->kid != NULL) {
        return addSibling(node->kid, data);
    } else {
        return (node->kid = newNode(data));
    }
}

TreeNode* finderKids(TreeNode *toCheck, char *name) {
    // DFS
    if ((toCheck == NULL) || (!strcmp(toCheck->data.name, name))) {
        printf("toCheck is null or name is found");
        return toCheck;
    }
    toCheck = toCheck->kid;
    while (toCheck != NULL) {
        if (!strcmp(toCheck->data.name, name)) {
            //found it
            return toCheck;
        }
        toCheck = toCheck->kid;
    }
    return NULL;
}

TreeNode* search(TreeNode *root, Data data) {
    // base case
    if ((root == NULL) || (!strcmp(root->data.name, data.name))) {
        printf("root is null or name is found");
        return root;
    }
    TreeNode *res = (TreeNode*) malloc(sizeof(TreeNode));

    while(root != NULL) {
        // search its children
        res = finderKids(root, data.name);
        if (res != NULL) {
            return res;
        }
        root = root->next;
    }

    printf("I couldn't find %s\n", root->data.name);
    free(res);
    return NULL;
}

// Delete kids of a given node (and their kids)
// exei provlima mallon !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void deleteNodeKids(TreeNode* node) {

    if (node->kid != NULL) {
        deleteNodeKids(node->kid);
    }
    free(node);
}

TreeNode* removeNode(TreeNode* root, TreeNode* node) {
    TreeNode* res;
    res = search(root, node->data);
    if (res == NULL) {
        printf("Couldn't find the node to remove\n");
        return NULL;
    }
    // delete its kids
    deleteNodeKids(res);
    // delete the node under review
    // find the previous node
    TreeNode *prev = root;
    while (prev->next != NULL) {
        if (!strcmp(prev->next->data.name, node->data.name) ) {
            break;
        }
        prev = prev->next;
    }

    res = prev->next;        //the node to be deleted
    //check if node really exists in list (because prev pointer might have reached end of list)
    if (res == NULL) {
        return NULL;
    }

    //remove node from list and make previous node point to the next one of the deleted
    prev->next = res->next;


    // free(res);
}

TreeNode* ordering(TreeNode* node) {
    // order nodes according to their names
}

void printTree(TreeNode* root, int space) {
    // Base case
    if (root == NULL)
        return;

    // Increase distance between levels
    space += COUNT;

    if(root->kid) printTree(root->kid, space);

    // Print current node after space
    // count
    printf("\n");
    for (int i = COUNT; i < space; i++)
        printf("-");
    printf("%d\n", root->data.myData->num);

    if(root->next) printTree(root->next, space);
}
