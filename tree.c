#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/tree.h"

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

// myTree* removeNode(myTree* root, Data data){
//     myTree* res = (myTree *)malloc(sizeof(myTree));
//     res = search(root, data);
//     if(res == NULL){
//         printf("Something went wrong in remove\n");
//         return NULL;
//     }

//     free(res);
// }
// myTree* insertNode(){

// }
