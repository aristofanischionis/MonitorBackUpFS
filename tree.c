#include "Headerfiles/tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initiaize tree with the given data as root node
Tree *initializeTree(Data data) {
    Tree *tree = NULL;
    tree = malloc(sizeof(Tree));
    tree->root = newNode(data);

    return tree;
}

// Initialize a new tree node with the given data
TreeNode *newNode(Data data) {
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
TreeNode *addSiblingSorted(TreeNode *node, Data data) {
    // we have to find the node that will be next of the given one
    TreeNode *next = node;
    if (node == NULL) {
        return NULL;
    }
    TreeNode *prev;
    // sort from a->z
    while (next != NULL) {
        if (strcmp(next->data.name, data.name) >= 0) break;
        prev = next;
        next = next->sibling;
    }
    TreeNode *new = newNode(data);
    prev->sibling = new;
    new->sibling = next;
    return new;
}

// Add a kid to the given node
TreeNode *addKid(TreeNode *node, Data data) {
    if (node == NULL) {
        printf("addKid: Node is null\n");
        return NULL;
    }
    // if there is already a kid, add it as a sibling to it
    // else add it as the first kid
    if (node->kid != NULL) {
        // if the new node's must be inserted as the first child (at the start
        // of the list)
        if (strcmp(node->kid->data.name, data.name) >= 0) {
            TreeNode *new = newNode(data);
            new->sibling = node->kid;
            node->kid = new;
            return new;
        } else {
            return addSiblingSorted(node->kid, data);
        }
    } else {
        return (node->kid = newNode(data));
    }
}

TreeNode *searchByPath(TreeNode *node, char *path) {
    if (node == NULL || !strcmp(path, "")) {
        return NULL;
    }
    if (!strcmp(node->data.path, path)) {
        return node;
    }

    TreeNode *nodeKid = NULL;
    TreeNode *nodeSibling = NULL;
    if (node->kid != NULL) {
        nodeKid = searchByPath(node->kid, path);
    }
    if (node->sibling != NULL) {
        nodeSibling = searchByPath(node->sibling, path);
    }

    if (nodeKid != NULL) {
        return nodeKid;
    } else {
        return nodeSibling;
    }
}

void deleteKids(TreeNode *node) {
    // if the node is null (e.g. if delete was called for kids but there are no
    // kids) then return
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

TreeNode *findPrevious(TreeNode *node, Data data) {
    if ((node == NULL) || (!strcmp(data.name, ""))) {
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
    findPrevious(node->kid, data);
}

TreeNode *deleteNode(Tree *tree, TreeNode *node) {
    if (node == NULL) {
        printf("Couldn't find the node to remove\n");
        return NULL;
    }
    // delete all of its kids, so all the siblings of its kid
    deleteKids(node->kid);
    node->kid = NULL;

    // delete the node under review
    // find the previous node
    TreeNode *prev = findPrevious(tree->root, node->data);
    if (prev == NULL) {
        return NULL;
    }
    if ((prev->kid->data.name != NULL) &&
        (!strcmp(prev->kid->data.name, node->data.name))) {
        prev->kid = node->sibling;
    } else if ((prev->sibling->data.name != NULL) &&
               (!strcmp(prev->sibling->data.name, node->data.name))) {
        prev->sibling = node->sibling;
    }
    free(node);

    return prev;
}

void printTree(Tree *tree) {
    printBranch(tree->root->kid, tree->root->data.name);
}

void printBranch(TreeNode *node, char *parentName) {
    if (node != NULL) {
        printBranch(node->kid, node->data.name);
        printBranch(node->sibling, parentName);
        printf("%s in folder %s \n", node->data.name, parentName);
    }
}
