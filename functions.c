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
#include "Headerfiles/functions.h"
#include "Headerfiles/defines.h"

int isDot(char *name)
{
    return (((!strcmp(name, ".")) || (!strcmp(name, ".."))) ? (1) : (0));
}

void readDirectory(char *filename, List **list, TreeNode *previous)
{
    DIR *file_ptr;
    Data data;
    struct dirent *direntp;
    char path[MAX];
    if ((file_ptr = opendir(filename)) == NULL)
    {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }
    else
    {
        while ((direntp = readdir(file_ptr)) != NULL)
        {
            sprintf(path, "%s/%s", filename, direntp->d_name);
            strcpy(data.name, direntp->d_name);
            // if this is a file (not a pipe, socket, etc) add an inode
            if (isREG(direntp->d_type))
            {
                INode *node = addINode(list, path);
                data.inode = node;
                addKid(previous, data);
            }
            // check if direntp is a directory
            if (isDIR(direntp->d_type))
            {
                // check if it a new directory (not a . or ..)
                if (!isDot(direntp->d_name))
                {
                    // if it is a new directory add it onnly to the tree
                    // INode *node = addINode(list, path);
                    // data.inode = node;
                    TreeNode *treenode = addKid(previous, data);
                    readDirectory(path, list, treenode);
                }
            }
        }
        closedir(file_ptr);
    }
}

void makeBackup(char *source, char *backup)
{
    pid_t cp_pid;
    DIR *ds, *db;
    /* Open the directory specified by "source". */

    ds = opendir(source);
    /* Check it was opened. */
    if (!ds)
    {
        perror("Cannot open directory source in Backup\n");
        exit(EXIT_FAILURE);
    }

    /* Open the directory specified by "backup". */

    db = opendir(backup);
    /* Check it was opened. */
    if (!db)
    {
        printf("Backup doesn't exist yet!\n");
        copy(source, backup);
    }
    else
    {
        printf("Backup exists already!\n");
        // so first delete it
        remove(backup);
        // now that it is deleted let's cp the source to it
        copy(source, backup);
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

void copy(char *source, char *dest)
{
    int childExitStatus;
    pid_t pid;
    int status;
    if (!source || !dest) {
        /* handle as you wish */
        perror("Source or Destination in copy is not specified\n");
        exit(1);
    }

    pid = fork();

    if (pid == 0) { /* child */
        execl("/bin/cp", "/bin/cp", "-a", source, dest, (char *)0);
    }
    else if (pid < 0) {
        /* error - couldn't start process - you decide how to handle */
        perror("pid<0 in copy\n");
        exit(1);
    }
    else {
        /* parent - wait for child - this has all error handling, you
         * could just call wait() as long as you are only expecting to
         * have one child process at a time.
         */
        pid_t ws = waitpid( pid, &childExitStatus, WNOHANG);
        if (ws == -1)
        { /* error - handle as you wish */
            perror("waitpid error in copy\n");
            exit(1);
        }

        // if( WIFEXITED(childExitStatus)) /* exit code in childExitStatus */
        // {
        //     status = WEXITSTATUS(childExitStatus); /* zero is normal exit */
        //     /* handle non-zero as you wish */
        // }
        // else if (WIFSIGNALED(childExitStatus)) /* killed */
        // {
        // }
        // else if (WIFSTOPPED(childExitStatus)) /* stopped */
        // {
        // }
    }
}

