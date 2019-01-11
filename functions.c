#include "Headerfiles/functions.h"
#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "Headerfiles/defines.h"
#include "Headerfiles/tree.h"

int isDot(char *name) {
    return (((!strcmp(name, ".")) || (!strcmp(name, ".."))) ? (1) : (0));
}

// Copy source files and backup and if backup doesn't exist create it
void makeBackup(char *source, char *backup) {
    pid_t cp_pid;
    DIR *ds, *db;
    /* Open the directory specified by "source". */

    ds = opendir(source);
    /* Check it was opened. */
    if (!ds) {
        perror("Cannot open directory source in Backup\n");
        exit(EXIT_FAILURE);
    }

    /* Open the directory specified by "backup". */

    db = opendir(backup);
    /* Check it was opened. */
    if (!db) {
        if ((cp_pid = fork()) == -1) {
            perror(" fork ");
            exit(EXIT_FAILURE);
        }
        if (cp_pid == 0) {
            // child
            char *params[5];
            params[0] = "cp";
            params[1] = "-a";
            params[2] = malloc(MAX * sizeof(char));
            strcpy(params[2], source);
            params[3] = malloc(MAX * sizeof(char));
            sprintf(params[3], "%s/", backup);
            params[4] = NULL;
            execvp("cp", params);
        } else {
            wait(NULL);
        }
    } else {
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
            strcpy(params[2], backup);
            params[3] = NULL;
            execvp("rm", params);
        } else {
            wait(NULL);
        }
        // now that it is deleted let's cp the source to it

        if ((cp_pid = fork()) == -1) {
            perror(" fork ");
            exit(EXIT_FAILURE);
        }
        if (cp_pid == 0) {
            // child
            char *params[5];
            params[0] = "cp";
            params[1] = "-a";
            params[2] = malloc(MAX * sizeof(char));
            strcpy(params[2], source);
            params[3] = malloc(MAX * sizeof(char));
            sprintf(params[3], "%s/", backup);
            params[4] = NULL;
            params[4] = NULL;
            execvp("cp", params);
        } else {
            wait(NULL);
        }
    }
    closedir(ds);
    closedir(db);
    return;
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
char *formatBackupPath(char *sourceBase, char *backupBase, char *sourcePath) {
    char *backupPath = malloc(sizeof(char) * MAX);
    strcpy(backupPath, backupBase);
    char sourcePathCopy[MAX];
    strcpy(sourcePathCopy, sourcePath);
    size_t len = strlen(sourceBase);
    if (len > 0) {
        char *p = sourcePathCopy;
        while ((p = strstr(p, sourceBase)) != NULL) {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    strcat(backupPath, sourcePathCopy);
    // remove '/' character if it exists at the end of the source path
    // (strlen(eventPath)-2] because of end of text character)
    if (backupPath[strlen(backupPath) - 1] == '/') {
        backupPath[strlen(backupPath) - 1] = 0;
    }
    return backupPath;
}

// Copy a source file or directory in the dest
void copy(char *source, char *dest) {
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
    } else if (pid < 0) {
        perror("pid<0 in copy\n");
        exit(1);
    } else {
        pid_t ws = waitpid(pid, &childExitStatus, WNOHANG);
        if (ws == -1) {
            perror("waitpid error in copy\n");
            exit(1);
        }
    }
}

void fail(const char *message) {
    perror(message);
    exit(1);
}

int isDirectory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) return 0;
    return S_ISDIR(statbuf.st_mode);
}

void printStructures(Tree *sourceTree, Tree *backupTree, List *sourceINodes,
                     List *backupINodes) {
    printf("\n\nSOURCE TREE:\n");
    printTree(sourceTree);
    printf("\n");
    printf("BACKUP TREE:\n");
    printTree(backupTree);
    printf("\n");
    printf("SOURCE INODES:\n");
    printINodes(sourceINodes);
    printf("\n");
    printf("BACKUP INODES:\n");
    printINodes(backupINodes);
    printf("\n");
}
