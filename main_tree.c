#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/tree.h"

int main(void){

    Data data, data1, data2, data3, data4 , data5, data0;

    strcpy(data0.name, " root");
    strcpy(data.name, " aneighbour");
    strcpy(data1.name, " bneighbour1");
    strcpy(data2.name, " cneighbour2");
    strcpy(data3.name, " dkid");
    strcpy(data4.name, " ekid1");
    strcpy(data5.name, " fkid2");
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
    // res = search(root, data5);
    // if(res != NULL)
    // printf("name : %s and value %d\n", res->data.name, res->data.myData->num);
    // res = search(root, data4);
    // if(res != NULL)
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
    deleteNode(root, kid2);
    res = search(root, data4);
    if(res != NULL)
    printf("name : %s\n", res->data.name);
    res = search(root, data3);
    if(res != NULL)
    printf("name : %s\n", res->data.name);
    res = search(root, data5);
    if(res != NULL)
    printf("name : %s\n", res->data.name);
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
