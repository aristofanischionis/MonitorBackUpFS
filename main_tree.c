#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/tree.h"

int main(void){

    Data data, data1, data2, data3, data4 , data5, data0;
    data0.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data1.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data2.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data3.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data4.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data5.myData = (iNodeData*) malloc(sizeof(iNodeData));


    strcpy(data0.name, " root");
    data0.myData->num = 0;
    strcpy(data.name, " aneighbour");
    data.myData->num = 1;
    strcpy(data1.name, " bneighbour1");
    data1.myData->num = 2;
    strcpy(data2.name, " cneighbour2");
    data2.myData->num = 3;
    strcpy(data3.name, " dkid");
    data3.myData->num = 4;
    strcpy(data4.name, " ekid1");
    data4.myData->num = 5;
    strcpy(data5.name, " fkid2");
    data5.myData->num = 6;
    ////////////////////////////////////////////////////////
    Tree *root = initializeTree(data0);
    TreeNode* neighbour = addSiblingSorted(root->root, data);
    TreeNode* neighbour1 = addSiblingSorted(root->root, data1);
    TreeNode* neighbour2 = addSiblingSorted(root->root, data2);
    TreeNode* kid = addKid(neighbour, data3);
    TreeNode* kid1 = addKid(neighbour, data4);
    TreeNode* kid2 = addKid(neighbour, data5);
    // check behaviour of addkid in kid PROBLEM HERE AS WELL SHOULD BE RECURSIVE
    ///////////////////////////////////////////////////////
    // printf("name--> %s\n", root->root->data.name);
    TreeNode* res;
    // res = search(root, data4);
    // printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    // res = search(root, data1);
    // printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    // res = search(root, data2);
    // printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    // res = search(root, data3);
    // printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    // res = search(root, data4);
    // printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    // res = search(root, data5);
    // printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    // fflush(stdout);
    // printTree(root, 0);
    ///////
    ///////
    removeNode(root, kid1);
    res = search(root, data4);
    if(res != NULL)
    printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    res = search(root, data3);
    if(res != NULL)
    printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    res = search(root, data5);
    if(res != NULL)
    printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    // removeNode(root, neighbour);
    // res = search(root, data);
    // if(res != NULL)
    // printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    // res = search(root, data3);
    // if(res != NULL)
    // printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    // res = search(root, data4);
    // if(res != NULL)
    // printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    // res = search(root, data5);
    // if(res != NULL)
    // printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    // printTree(root, 0);
    return 0;
}
