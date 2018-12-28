#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "Headerfiles/tree.h"
#include "Headerfiles/functions.h"

int main(int argc, char const *argv[]) {

    if(argc < 3){
        perror("Not enough arguments provided\n");
    	exit(EXIT_FAILURE);
    }

    char *sourceFilename = malloc(strlen(argv[1])+1);
    strcpy(sourceFilename, argv[1]);
    char *backupFilename = malloc(strlen(argv[2])+1);
    strcpy(backupFilename, argv[2]);

    readDirectory(sourceFilename);
    readDirectory(backupFilename);

    return 0;
}
