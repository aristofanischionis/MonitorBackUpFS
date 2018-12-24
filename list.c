#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

// Allocate memory for list of inodes struct
List * initializeList(void) {
    List *list = NULL;
    list = malloc(sizeof(List));
    list->head = NULL;

    return list;
}

// Add an original inode to the start of list
int addSourceNode(List **list, char *path) {
    // check if given vertex exists
    if (searchForNode(*list, path) != NULL) {
        return 1;
    }

    struct stat buf;
    if (stat(path, &buf) == -1) {
        perror("Error using stat");
        exit(1);
    }

    // Allocate memory for node
    INode *newNode = (INode *)malloc(sizeof(INode));

    newNode->path = malloc(strlen(path)+1);
    strcpy(newNode->path, path);
    newNode->changeDate = buf.st_mtim;
    newNode->size = buf.st_size;
    newNode->nameCount = (int) buf.st_nlink;
    newNode->next = (*list)->head;
    // Change head pointer as new node is added at the beginning
    (*list)->head = newNode;

    return 0;
}

// Add a copied inode to the start of list
int addCopyNode(List **list, char *path, INode *originalNode) {
    // check if given vertex exists
    if (searchForNode(*list, path) != NULL) {
        return 1;
    }

    // Allocate memory for node
    INode *newNode = (INode *)malloc(sizeof(INode));

    newNode->path = malloc(strlen(path)+1);
    strcpy(newNode->path, path);

    newNode->changeDate = originalNode->changeDate;
    newNode->size = originalNode->size;
    newNode->nameCount = originalNode->nameCount;
    newNode->copy = originalNode;
    newNode->next = NULL;
    // Change head pointer as new node is added at the beginning
    (*list)->head = newNode;

    return 0;
}

// Find if an inode with the given path exists and return it else return null
INode * searchForNode(List *list, char *path) {
    // if list is empty return null
    if (list->head == NULL) {
        return NULL;
    }

    INode *current = list->head;

    while (current != NULL) {
        if (strcmp(current->path, path) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}


// Print all nodes
void printNodes(List *list) {
    INode *current = list->head;

    while (current != NULL) {
        printf("%s %ld\n", current->path, current->size);
        current = current->next;
    }
}


// Delete given inode (path) and all copies pointing to this inode
int delete_vertex(List **list, char *path) {
    INode *node = searchForNode(*list, path);

    // check if given node exists
    if (node == NULL) {
        return 1;
    }

    // firstly delete all copies directed to given inode
    // ..........

    // delete the node
    //when node to be deleted is the head node
    if (strcmp((*list)->head->path, path) == 0) {
        //if there is only one node in the list make list head point to null and free node
        if ((*list)->head->next == NULL) {
            INode *n = (*list)->head;
            (*list)->head = NULL;
            free(n->path);
            free(n);
            return 0;
        }

        INode *n = (*list)->head;
        (*list)->head = n->next;
        free(n->path);
        free(n);

        return 0;
    }

    //when not head node, follow the normal deletion process:
    //find the previous node
    INode *prev = (*list)->head;
    while (prev->next != NULL) {
        if (strcmp(prev->next->path, path) == 0) {
            break;
        }
        prev = prev->next;
    }

    INode *n = prev->next;        //the node to be deleted
    //check if node really exists in list (because prev pointer might have reached end of list)
    if (prev->next == NULL) {
        return 1;
    }

    //remove node from list and make previous node point to the next one of the deleted
    prev->next = n->next;
    free(n->path);
    free(n);

    return 0;

}
