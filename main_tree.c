#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/tree.h"

int main(void){

    Data data, data1, data2, data3, data4;

    data.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data1.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data2.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data3.myData = (iNodeData*) malloc(sizeof(iNodeData));
    data4.myData = (iNodeData*) malloc(sizeof(iNodeData));


    strcpy(data.name, "myname is root");
    data.myData->num = 1;
    strcpy(data1.name, "myname is 111");
    data1.myData->num = 2;
    strcpy(data2.name, "myname is 2222");
    data2.myData->num = 3;
    strcpy(data3.name, "myname is 33333");
    data3.myData->num = 4;
    strcpy(data4.name, "myname is 44444");
    data4.myData->num = 5;

    myTree* root = newNode(data);
    myTree* neighbour = addNext(root, data1);
    myTree* neighbour1 = addNext(neighbour, data3);
    myTree* kid = addKid(neighbour, data2);
    myTree* kid1 = addKid(kid, data4);

    myTree* res = search(root, data4);
    printf("........%s,,,,,,,,,%d\n", res->data.name, res->data.myData->num);
    fflush(stdout);
    return 0;
}