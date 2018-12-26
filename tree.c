#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/tree.h"
#define COUNT 10

Tree* initializeTree(void) {
    Tree *tree = NULL;
    tree = malloc(sizeof(Tree));
    // tree->root = NULL;
    Data rootData;
    rootData.myData = (iNodeData*)malloc(sizeof(iNodeData));
    rootData.myData->num = 0;
    strcpy(rootData.name, "root");
    tree->root = newNode(rootData);
    
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
    TreeNode *prev = (TreeNode*) malloc(sizeof(TreeNode));
    if (node == NULL || prev == NULL) {
        return NULL;
    }
    prev = node;
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

///////////////////////////////////////////////////////////////////////

// function to find out middle element 
// TreeNode* middle(TreeNode* start, TreeNode* last) 
// { 
//     printf("I am here");
//     if (start == NULL) 
//         return NULL; 
  
//     TreeNode* slow = start; 
//     TreeNode* fast = start->kid; 
//     if(fast == NULL){
//         return slow;
//     }
//     while (fast != last) 
//     { 
//         fast = fast->kid; 
//         if (fast != last) 
//         { 
//             slow = slow->kid; 
//             fast = fast->kid; 
//         } 
//     } 
  
//     return slow; 
// } 
  
// Function for implementing the Binary 
// Search on linked list 
// TreeNode* binarySearch(TreeNode *head, char* name) 
// { 
//     TreeNode* start = head; 
//     TreeNode* last = NULL; 
  
//     do
//     { 
//         // Find middle 
//         TreeNode* mid = middle(start, last); 
  
//         // If middle is empty 
//         if (mid == NULL) 
//             return NULL; 
  
//         // If name is present at middle 
//         if (!strcmp(mid->data.name, name)) 
//             return mid; 
  
//         // If name is more than mid 
        
//         else if (strcmp(mid->data.name, name) < 0) 
//             start = mid->kid; 
  
//         // If the name is less than mid. 
//         else
//             last = mid; 
  
//     } while (last == NULL || last->kid != start); 
  
//     // name not present 
//     return NULL; 
// } 
///////////////////////////////////////////////////////////////////////
// TreeNode* finderKids(TreeNode *toCheck, char *name) {
//     // DFS
//     if ((toCheck == NULL) || (!strcmp(toCheck->data.name, name))) {
//         return toCheck;
//     }
//     if(toCheck->kid == NULL) return NULL;
//     return binarySearch(toCheck, name);
//     return NULL;
// }

TreeNode *finderKids(TreeNode *toCheck, char *name){
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

TreeNode* search(Tree *root, Data data) {
    // base case
    if ((root == NULL) || (root->root == NULL) || (!strcmp(root->root->data.name, data.name))) {
        printf("root is null or name is found");
        return root->root;
    }
    TreeNode *res;
    TreeNode* toCheck;
    toCheck = root->root;
    // res = root->root;
    while(toCheck != NULL) {
        // search its children
        res = finderKids(toCheck, data.name);
        if (res != NULL) {
            return res;
        }
        toCheck = toCheck->sibling;
    }

    printf("I couldn't find %s\n", data.name);
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

int removeNode(Tree* root, TreeNode* node) {
    TreeNode* res;
    res = search(root, node->data);
    if (res == NULL) {
        printf("Couldn't find the node to remove\n");
        return 1;
    }
    // delete its kids
    deleteNodeKids(res);
    // delete the node under review
    // find the previous node
    TreeNode *prev = root->root;
    while (prev->sibling != NULL) {
        if (!strcmp(prev->sibling->data.name, node->data.name) ) {
            break;
        }
        prev = prev->sibling;
    }

    res = prev->sibling;        //the node to be deleted
    //check if node really exists in list (because prev pointer might have reached end of list)
    if (res == NULL) {
        return 1;
    }

    //remove node from list and make previous node point to the sibling one of the deleted
    prev->sibling = res->sibling;
    free(res);
    return 0;
}

// void printTree(Tree* root, int space) {
//     // Base case
//     if (root->root == NULL)
//         return;

//     // Increase distance between levels
//     space += COUNT;

//     if(root->root->kid) printTree(root->root->kid, space);

//     // Print current node after space
//     // count
//     printf("\n");
//     for (int i = COUNT; i < space; i++)
//         printf("-");
//     printf("%d\n", root->root->data.myData->num);

//     if(root->root->sibling) printTree(root->root->sibling, space);
// }

// void printBranch(TreeNode *node){

//     TreeNode* current = node;

//     if(current->kid != NULL){
//         printBranch(current->kid);
//     }
//     else printf("-->%s\n",current->data.name);


// }