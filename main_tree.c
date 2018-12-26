#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/tree.h"

int main(void){

    Data data, data1, data2, data3, data4 , data5;

    data.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data1.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data2.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data3.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data4.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data5.myData = (iNodeData*) malloc(sizeof(iNodeData));


    strcpy(data.name, "myname is neighbour");
    data.myData->num = 1;
    strcpy(data1.name, "myname is neighbour1");
    data1.myData->num = 2;
    strcpy(data2.name, "myname is neighbour2");
    data2.myData->num = 3;
    strcpy(data3.name, "myname is kid");
    data3.myData->num = 4;
    strcpy(data4.name, "myname is kid1");
    data4.myData->num = 5;
    strcpy(data5.name, "myname is kid2");
    data5.myData->num = 6;
    ////////////////////////////////////////////////////////
    Tree *root = initializeTree();
    TreeNode* neighbour = addSiblingSorted(root->root, data);
    TreeNode* neighbour1 = addSiblingSorted(root->root, data1);
    TreeNode* neighbour2 = addSiblingSorted(root->root, data2);
    TreeNode* kid = addKid(neighbour, data3);
    TreeNode* kid1 = addKid(neighbour, data4);
    TreeNode* kid2 = addKid(neighbour, data5);
    ///////////////////////////////////////////////////////
    printf("name--> %s\n", root->root->data.name);
    TreeNode* res;
    res = search(root, data);
    printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    res = search(root, data1);
    printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    res = search(root, data2);
    printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    res = search(root, data3);
    printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    res = search(root, data4);
    printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    res = search(root, data5);
    printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    fflush(stdout);
    // printTree(root, 0);
    ///////
    ///////
    // removeNode(root, neighbour); // if I try removing kids it  doesn't work...
    // printTree(root, 0);
    return 0;
}
