#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/tree.h"

int main(void){

    Data data, data1, data2, data3, data4 , data5, data6, data7, data8;

    strcpy(data.name, "N0");
    strcpy(data1.name, "N1");
    strcpy(data2.name, "N2");
    strcpy(data3.name, "N3");
    strcpy(data4.name, "N4");
    strcpy(data5.name, "N5");
    strcpy(data6.name, "N6");
    strcpy(data7.name, "N7");
    strcpy(data8.name, "N8");

    Tree *tree = initializeTree(data);
    TreeNode* neighbour1 = addKid(tree->root, data1);
    TreeNode* neighbour2 = addKid(tree->root, data2);
    TreeNode* neighbour3 = addKid(tree->root, data3);
    TreeNode* kid4 = addKid(neighbour1, data4);
    TreeNode* kid5 = addKid(neighbour1, data5);
    TreeNode* kid6 = addKid(neighbour1, data6);
    TreeNode* kid7 = addKid(neighbour3, data7);
    TreeNode* kid8 = addKid(kid6, data8);

    printTree(tree);

    printf("\n");
    deleteNode(tree, neighbour2);

    printTree(tree);

    free(tree->root);
    free(tree);

    return 0;
}
