#include <stdio.h>
#include "list.h"

int main(int argc, char const *argv[]) {
    List *list = initializeList();
    addSourceNode(&list, "list.c");
    addSourceNode(&list, "useless.c");
    printNodes(list);
    // deleteNode(&list, "list.c");
    // printf("...\n");
    // printNodes(list);
    return 0;
}
