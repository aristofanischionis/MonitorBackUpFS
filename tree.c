#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/tree.h"
#define COUNT 10

myTree *newNode(Data data)
{
    myTree *node = (myTree *)malloc(sizeof(myTree));

    if (node)
    {
        node->data = data;
        node->next = NULL;
        node->kid = NULL;
        return (node);
    }
    perror("Something went wrong while allocating newNode\n");
    exit(EXIT_FAILURE);
}

myTree *addNext(myTree *node, Data data)
{
    if (node == NULL)
        return NULL;
    while (node->next != NULL)
        node = node->next;

    return (node->next = newNode(data));
}

myTree *addKid(myTree *node, Data data)
{
    if (node == NULL)
        return NULL;
    // I already have a kid so I will add him a next
    if (node->kid != NULL)
        return addNext(node->kid, data);
    else
        return (node->kid = newNode(data));
}

myTree *finderKids(myTree *toCheck, char *name){
    // DFS
    if((toCheck == NULL) || (!strcmp(toCheck->data.name, name))){
        printf("toCheck is null or name is found");
        return toCheck;
    }
    toCheck = toCheck->kid;
    while(toCheck != NULL){
        if (!strcmp(toCheck->data.name, name))
        {
            //found it
            return toCheck;
        }
        toCheck = toCheck->kid;
    }
    return NULL;
}

myTree *search(myTree *root, Data data)
{
    // base case
    if((root == NULL) || (!strcmp(root->data.name, data.name))){
        printf("root is null or name is found");
        return root;
    }
    myTree *res = (myTree*) malloc(sizeof(myTree));
    
    while(root != NULL){
        // search its children
        res = finderKids(root, data.name);
        if(res != NULL){
            return res;
        }
        root = root->next;
    }
    
    printf("I couldn't find %s\n", root->data.name);
    free(res);
    return NULL;
}

void deleteNodeKids(myTree* node){

    if(node->kid != NULL){
        deleteNodeKids(node->kid);
    }
    else {
        free(node);
    }
}

myTree* removeNode(myTree* root, myTree* node){
    myTree* res;
    res = search(root, node->data);
    if(res == NULL){
        printf("Couldn't find the node to remove\n");
        return NULL;
    }
    // delete the node under review
    // find the previous node
    myTree *prev = root;
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
    //
    deleteNodeKids(res);

    // free(res);
}

myTree* ordering(myTree* node){
    // order nodes according to their names
}

void printTree(myTree* root, int space){
    // Base case 
    if (root == NULL) 
        return; 
  
    // Increase distance between levels 
    space += COUNT; 
  
    // Process right child first 
    printTree(root->kid, space); 
  
    // Print current node after space 
    // count 
    printf("\n"); 
    for (int i = COUNT; i < space; i++) 
        printf(" "); 
    printf("%d\n", root->data.myData->num); 
  
    // Process left child 
    printTree(root->next, space); 
}