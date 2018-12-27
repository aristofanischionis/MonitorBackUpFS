#include <stdio.h>
#include "Headerfiles/list.h"

int main(int argc, char const *argv[]) {
    List *list = initializeList();
    addINode(&list, "list.c");
    addINode(&list, "useless.c");
    printINodes(list);
    deleteINode(&list, 6644);
    printf("...\n");
    printINodes(list);
    return 0;
}
