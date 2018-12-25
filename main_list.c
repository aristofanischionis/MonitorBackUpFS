#include <stdio.h>
#include "list.h"

int main(int argc, char const *argv[]) {
    List *list = initializeList();
    addSourceNode(&list, "list.c");
    addSourceNode(&list, "useless.c");
    printNodes(list);
    deleteNode(&list, 1838157);
    printf("...\n");
    printNodes(list);
    return 0;
}
