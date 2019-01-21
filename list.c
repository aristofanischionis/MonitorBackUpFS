#include "Headerfiles/list.h"
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "Headerfiles/namelist.h"

// Allocate memory for list of inodes struct
List *initializeList(void) {
    List *list = NULL;
    list = malloc(sizeof(List));
    list->head = NULL;

    return list;
}

// Add a node to the beginning of the list, if it already exists add name to its
// nameList
INode *addINode(List **list, char *path) {
    struct stat buf;
    if (stat(path, &buf) == -1) {
        perror("Error using stat in addINode");
        return NULL;
    }
    // extract filename from path
    char *filename = basename(path);

    INode *node = searchForINode(*list, (int)buf.st_ino);
    // check if given node exists (which means that the path is a hardlink)
    if (node != NULL) {
        // if inode already exists then add path name to its nameList and
        // increase counter
        addName(&node->names, filename);
        node->nameCount++;
        return node;
    }

    INode *newNode = (INode *)malloc(sizeof(INode));
    newNode->inodeNum = (int)buf.st_ino;
    newNode->modDate = buf.st_ctime;
    newNode->size = buf.st_size;
    newNode->names = initializeNameList();
    addName(&newNode->names, filename);
    newNode->nameCount = 1;
    newNode->modified = 0;
    newNode->copy = NULL;
    newNode->next = (*list)->head;
    // Change head pointer as new node is added at the beginning
    (*list)->head = newNode;
    return (*list)->head;
}

// Find if an inode exists and return it else return null
INode *searchForINode(List *list, int inodeNum) {
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

INode *searchForINodeByPath(List *list, char *path) {
    struct stat buf;
    if (stat(path, &buf) == -1) {
        perror("Error using stat in searchForINode");
        return NULL;
    }
    int inodeNum = (int)buf.st_ino;
    return searchForINode(list, inodeNum);
}

// Print all nodes
void printINodes(List *list) {
    INode *current = list->head;

    while (current != NULL) {
        printf("%d %ld %s \twith names:  ", current->inodeNum, current->size,
               ctime(&current->modDate));
        printNames(current->names);
        current = current->next;
    }
}

// Delete given inode and all copies pointing to this inode, only if this inode
// has only one name pointing to it
int deleteINode(List **list, int inodeNum, char *name) {
    INode *node = searchForINode(*list, inodeNum);
    // check if given node exists
    if (node == NULL) {
        printf("Node does not exist\n");
        return 1;
    }

    // if node has more that one name in its namelist, delete only the name from the
    // list
    if (node->names->head->next != NULL) {
        deleteName(&node->names, name);
        node->nameCount--;
        
        return 2;
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

// normal deletion process (delete a node in the middle of the list) that
// returns the node next of the deleted one
INode *normalDelete(List **list, int inodeNum) {
    // when not head node, follow the normal deletion process:
    // find the previous node
    INode *prev = (*list)->head;
    while (prev->next != NULL) {
        if (prev->next->inodeNum == inodeNum) {
            break;
        }
        prev = prev->next;
    }

    INode *n = prev->next;  // the node to be deleted
    // check if node really exists in list (because prev pointer might have
    // reached end of list)
    if (prev->next == NULL) {
        return NULL;
    }

    // remove node from list and make previous node point to the next one of the
    // deleted
    prev->next = n->next;
    free(n);
    return prev->next;
}

void deleteList(List **list) {
    INode *current = (*list)->head;
    INode *next;

    while (current != NULL) {
        next = current->next;

        deleteNameList(&current->names);
        free(current->names);
        free(current);
        current = next;
    }

    free(*list);
}
