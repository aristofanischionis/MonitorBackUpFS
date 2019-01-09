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
    return backup;
}

// Copy a source file or directory in the dest
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

void fail(const char *message) {
    perror(message);
    exit(1);
}

