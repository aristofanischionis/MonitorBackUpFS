#include "list.h"

int main(int argc, char const *argv[]) {
    List *list = initializeList();
    addSourceNode(&list, "list.c");
    addSourceNode(&list, "useless.c");
    printNodes(list);
    return 0;
}
