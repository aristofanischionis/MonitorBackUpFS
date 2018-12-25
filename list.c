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
    struct stat buf;
    if (stat(path, &buf) == -1) {
        perror("Error using stat");
        exit(1);
    }

    INode *node = searchForNode(*list, (int) buf.st_ino);
    // check if given node exists
    if (node != NULL) {
        // if inode already exists then add path name to its nameList and increase counter
        addName(&node->names, path);
        node->nameCount++;
        return 1;
    }

    INode *newNode = (INode *)malloc(sizeof(INode));
    newNode->inodeNum = (int) buf.st_ino;
    newNode->modDate = buf.st_mtime;
    newNode->size = buf.st_size;
    newNode->names = initializeNameList();
    newNode->nameCount = (int) buf.st_nlink;
    newNode->copy = NULL;
    newNode->next = (*list)->head;
    // Change head pointer as new node is added at the beginning
    (*list)->head = newNode;
    return 0;
}

// Add a copied inode to the start of list
int addCopyNode(List **list, int inodeNum, char *path, int inodeNumOriginal) {
    INode *originalNode = searchForNode(*list, inodeNumOriginal);

    struct stat buf;
    if (stat(path, &buf) == -1) {
        perror("Error using stat");
        exit(1);
    }

    INode *node = searchForNode(*list, (int) buf.st_ino);
    // check if given node exists
    if (node != NULL) {
        // if inode already exists then add path name to its nameList and increase counter
        addName(&node->names, path);
        node->nameCount++;
        return 1;
    }

    // Allocate memory for node
    INode *newNode = (INode *)malloc(sizeof(INode));

    newNode->inodeNum = (int) buf.st_ino;
    newNode->modDate = originalNode->modDate;
    newNode->size = originalNode->size;
    newNode->names = initializeNameList();
    newNode->nameCount = originalNode->nameCount;
    newNode->copy = originalNode;
    newNode->next = NULL;
    // Change head pointer as new node is added at the beginning
    (*list)->head = newNode;

    return 0;
}

// Find if an inode exists and return it else return null
INode * searchForNode(List *list, int inodeNum) {
    // if list is empty return null
    if (list->head == NULL) {
        return NULL;
    }

    INode *current = list->head;

    while (current != NULL) {
        if (current->inodeNum == inodeNum) {
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
        printf("%d %ld %s\n", current->inodeNum, current->size, ctime(&current->modDate));
        current = current->next;
    }
}

// Delete given inode and all copies pointing to this inode
int deleteNode(List **list, int inodeNum) {
    INode *node = searchForNode(*list, inodeNum);
    // check if given node exists
    if (node == NULL) {
        return 1;
    }
    // firstly delete all copies directed to given inode
    INode *current = (*list)->head;

    while (current != NULL) {
        if (current->copy != NULL && current->copy->inodeNum == inodeNum) {
            // delete name list first
            deleteNameList(&current->names);

            if (current->inodeNum == (*list)->head->inodeNum) {
                INode *n = (*list)->head;
                (*list)->head = n->next;
                free(n);
                current = (*list)->head;
            } else {
                current = normalDelete(list, inodeNum);
            }
        } else {
            current = current->next;
        }
    }

    // delete the node
    // when node to be deleted is the head node
    if ((*list)->head->inodeNum == inodeNum) {
        INode *n = (*list)->head;
        (*list)->head = n->next;
        free(n);
        return 0;
    }

    normalDelete(list, inodeNum);

    return 0;

}

// normal deletion process (delete a node in the middle of the list) that returns the
// node next of the deleted one
INode * normalDelete(List **list, int inodeNum) {
    // when not head node, follow the normal deletion process:
    // find the previous node
    INode *prev = (*list)->head;
    while (prev->next != NULL) {
        if (prev->next->inodeNum == inodeNum) {
            break;
        }
        prev = prev->next;
    }

    INode *n = prev->next;        //the node to be deleted
    //check if node really exists in list (because prev pointer might have reached end of list)
    if (prev->next == NULL) {
        return NULL;
    }

    //remove node from list and make previous node point to the next one of the deleted
    prev->next = n->next;
    free(n);
    return prev->next;
}
