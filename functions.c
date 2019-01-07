#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <libgen.h>
#include "Headerfiles/functions.h"
#include "Headerfiles/tree.h"
#include "Headerfiles/defines.h"

int isDot(char *name) {
    return (((!strcmp(name, ".")) || (!strcmp(name, ".."))) ? (1) : (0));
}

void readDirectory(char *filename, List **list, TreeNode *previous) {
    DIR *file_ptr;
    Data data;
    struct dirent *direntp;
    char path[MAX];
    if ((file_ptr = opendir(filename)) == NULL) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    } else {
        while ((direntp = readdir(file_ptr)) != NULL) {
            sprintf(path, "%s/%s", filename, direntp->d_name);
            strcpy(data.name, direntp->d_name);
            strcpy(data.path, path);
            // if this is a file (not a pipe, socket, etc) add an inode
            if (isREG(direntp->d_type)) {
                INode *node = addINode(list, path);
                data.inode = node;
                addKid(previous, data);
            }
            // check if direntp is a directory
            if (isDIR(direntp->d_type)) {
                // check if it a new directory (not a . or ..)
                if (!isDot(direntp->d_name)) {
                    // if it is a new directory add it only to the tree
                    TreeNode *treenode = addKid(previous, data);
                    readDirectory(path, list, treenode);
                }
            }
        }
        closedir(file_ptr);
    }
}


void makeBackup(char *source, char *backup) {
    char buf[MAX], buf1[MAX];
    pid_t cp_pid;
    char* bPath;
    char* sourcePath;
    DIR *db;
    bPath = malloc(MAX * sizeof(char));
    strcpy(bPath, "");
    sourcePath = malloc(MAX * sizeof(char));
    if(!source || !backup){
        perror("source or backup not given\n");
        exit(1);
    } 

    db = opendir(backup);
    if(db){
        sprintf(bPath, "%s", realpath(backup, buf));
    }
    closedir(db);
    sprintf(sourcePath, "%s/", realpath(source, buf));

    if(sourcePath == NULL){
        perror("source name for backup doesn't exist\n");
        exit(1);
    }
    if(!strcmp(bPath, "")){
        // means that there is not a backup yet
        printf("Backup doesnot exist: %s, %s\n", sourcePath, backup);
        copy(sourcePath, backup);
    }
    else {
        printf("Backup exists already!  %s \n", bPath);
        // so first delete it
        if ((cp_pid = fork()) == -1) {
            perror(" fork ");
            exit(EXIT_FAILURE);
        }
        if (cp_pid == 0) {
            // child
            char *params[4];
            params[0] = "rm";
            params[1] = "-rf";
            params[2] = malloc(MAX * sizeof(char));
            strcpy(params[2], bPath);
            params[3] = NULL;
            execvp("rm", params);
        } else {
            wait(NULL);
        }
        // find the parent folder where we re going to copy the source to
        printf("I will copy source to %s \n", backup);
        copy(sourcePath, backup);
    }
}


void makeDirectory(char *path, char *name) {
    char buf[MAX];
    char toMake[MAX];
    struct stat st = {0};
    sprintf(toMake, "%s/%s", realpath(path, buf), name);
    if (stat(toMake, &st) == -1) {
        mkdir(toMake, 0700);
    }
}

// Make an identical path to sourcePath, but with backupBase as the root
char *backupPath(char *sourcePath, char *backupBase) {
    const char s[2] = "/";
    char *token;
    char toCopy[100];
    char *source, *backup;
    source = malloc(MAX * sizeof(char));
    backup = malloc(MAX * sizeof(char));
    strcpy(source, sourcePath);
    strcpy(backup, backupBase);

    token = strtok(source, s);

    while (token != NULL) {
        token = strtok(NULL, s);
        if (token == NULL) break;
        sprintf(toCopy, "%s/", token);
        strcat(backup, toCopy);
    }
    printf("Backup path is : %s \n", backup);
    return backup;
}

void copy(char *source, char *dest)
{
    int childExitStatus;
    pid_t pid;
    int status;
    if (!source || !dest) {
        perror("Source or Destination in copy is not specified\n");
        exit(1);
    }

    pid = fork();

    if (pid == 0) {
        execl("/bin/cp", "/bin/cp", "-a", source, dest, (char *)0);
    }
    else if (pid < 0) {
        perror("pid<0 in copy\n");
        exit(1);
    }
    else {
        pid_t ws = waitpid( pid, &childExitStatus, WNOHANG);
        if (ws == -1)
        {
            perror("waitpid error in copy\n");
            exit(1);
        }
    }
}

