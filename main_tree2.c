#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/tree.h"

int main(void){

    Data data, data1, data2, data3, data4 , data5, data6;

    data.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data1.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data2.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data3.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data4.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data5.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data6.myData = (iNodeData*) malloc(sizeof(iNodeData));

    strcpy(data.name, "N1");
    strcpy(data1.name, "N2");
    strcpy(data2.name, "N3");
    strcpy(data3.name, "N4");
    strcpy(data4.name, "N5");
    strcpy(data5.name, "N6");
    strcpy(data6.name, "N7");

    Tree *tree = initializeTree(data);
    TreeNode* neighbour = addKid(tree->root, data1);
    TreeNode* neighbour1 = addKid(tree->root, data2);
    TreeNode* neighbour2 = addKid(tree->root, data3);
    TreeNode* kid = addKid(neighbour, data4);
    TreeNode* kid1 = addKid(neighbour, data5);
    TreeNode* kid2 = addKid(neighbour, data6);


    printf("root--> %s\n", tree->root->data.name);

    printTree(tree);
    return 0;
}