#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>


int isDirectory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) {
        return 0;
    }
    return S_ISDIR(statbuf.st_mode);
}

void readDirectory(char *filename) {
    DIR *file_ptr;
    struct dirent *direntp;
    if ((file_ptr = opendir(filename)) == NULL) {
        perror("Cannot open source file");
        exit(EXIT_FAILURE);
    } else {
        while ((direntp = readdir(file_ptr)) != NULL) {
            printf("inode %d of the entry %s\n" , \
            (int)direntp->d_ino, direntp->d_name);
            if (isDirectory(direntp->d_name)) {
                readDirectory(direntp->d_name);
            }
        }
        closedir(file_ptr);
    }
}
