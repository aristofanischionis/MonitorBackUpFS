#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Headerfiles/namelist.h"

// Allocate memory for list of names struct
NameList * initializeNameList(void) {
    NameList *list = NULL;
    list = malloc(sizeof(NameList));
    list->head = NULL;

    return list;
}

// Add a new name to the start of list
int addName(NameList **list, char *name) {
    NameListNode *node = searchForName(*list, name);
    // check if given name exists
    if (node != NULL) {
        return 1;
    }

    NameListNode *newNode = (NameListNode *)malloc(sizeof(NameListNode));
    strcpy(newNode->name, name);
    newNode->next = (*list)->head;
    // Change head pointer as new node is added at the beginning
    (*list)->head = newNode;
    return 0;
}

// Find if a name exists and return it else return null
NameListNode * searchForName(NameList *list, char *name) {
    // if list is empty return null
    if (list->head == NULL) {
        return NULL;
    }

    NameListNode *current = list->head;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Print all names
void printNames(NameList *list) {
    NameListNode *current = list->head;

    while (current != NULL) {
        printf("%s\n", current->name);
        current = current->next;
    }
}

// Delete given name
int deleteName(NameList **list, char *name) {
    NameListNode *node = searchForName(*list, name);
    // check if given node exists
    if (node == NULL) {
        return 1;
    }

    // delete the name
    // when node to be deleted is the head node
    if (strcmp((*list)->head->name, name) == 0) {
        NameListNode *n = (*list)->head;
        (*list)->head = n->next;
        free(n);
        return 0;
    }

    // when not head node, follow the normal deletion process:
    // find the previous node
    NameListNode *prev = (*list)->head;
    while (prev->next != NULL) {
        if (strcmp(prev->next->name, name) == 0) {
            break;
        }
        prev = prev->next;
    }

    NameListNode *n = prev->next;        //the node to be deleted
    //check if node really exists in list (because prev pointer might have reached end of list)
    if (prev->next == NULL) {
        return 1;
    }

    //remove node from list and make previous node point to the next one of the deleted
    prev->next = n->next;
    free(n);

    return 0;

}

void deleteNameList(NameList **list) {
    NameListNode *current = (*list)->head;
    NameListNode *next;

    while (current != NULL) {
        next = current->next;
        current = current->next;
        free(current);
        current = next;
    }
}

