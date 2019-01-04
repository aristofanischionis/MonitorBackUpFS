#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h> 
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
            // strcpy(data.name, path);
            strcpy(data.name, direntp->d_name);
            // if this is a file (not a pipe, socket, etc) add an inode
            if (isREG(direntp->d_type))
            {
                INode *node = addINode(list, path);
                // INode *node = addINode(list, direntp->d_name);
                data.inode = node;
                addKid(previous, data);
            }
            // check if direntp is a directory
            if (isDIR(direntp->d_type))
            {
                // check if it a new directory (not a . or ..)
                if (!isDot(direntp->d_name))
                {
                    // if it is a new directory add
                    INode *node = addINode(list, path);
                    // INode *node = addINode(list, direntp->d_name);
                    data.inode = node;
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
        perror("Cannot open directory source\n");
        exit(EXIT_FAILURE);
    }

    /* Open the directory specified by "backup". */

    db = opendir(backup);
    /* Check it was opened. */
    if (!db)
    {
        printf("Backup doesn't exist yet!\n");
        if ((cp_pid = fork()) == -1)
        {
            perror(" fork ");
            exit(EXIT_FAILURE);
        }
        if (cp_pid == 0)
        {
            //child
            char *params[5];
            params[0] = "cp";
            params[1] = "-a";
            params[2] = malloc(MAX * sizeof(char));
            strcpy(params[2], source);
            params[3] = malloc(MAX * sizeof(char));
            sprintf(params[3],"%s/", backup);
            params[4] = NULL;
            execvp("cp", params);
        }
        else{
            wait(NULL);
        }
    }
    else {
        printf("Backup exists already!\n");
        // so first delete it
        if ((cp_pid = fork()) == -1)
        {
            perror(" fork ");
            exit(EXIT_FAILURE);
        }
        if (cp_pid == 0)
        {
            //child
            char *params[4];
            params[0] = "rm";
            params[1] = "-rf";
            params[2] = malloc(MAX * sizeof(char));
            strcpy(params[2], backup);
            params[3] = NULL;
            execvp("rm", params);
        }
        else{
            wait(NULL);
        }
        // now that it is deleted let's cp the source to it

        if ((cp_pid = fork()) == -1)
        {
            perror(" fork ");
            exit(EXIT_FAILURE);
        }
        if (cp_pid == 0)
        {
            //child
            char *params[5];
            params[0] = "cp";
            params[1] = "-a";
            params[2] = malloc(MAX * sizeof(char));
            strcpy(params[2], source);
            params[3] = malloc(MAX * sizeof(char));
            sprintf(params[3],"%s/", backup);
            params[4] = NULL;
            execvp("cp", params);
        }
        else{
            wait(NULL);
        }

    }
    closedir(ds);
    closedir(db);
    return;
}