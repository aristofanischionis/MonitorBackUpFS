#include "Headerfiles/eventActions.h"
#include "Headerfiles/eventHandlers.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include "Headerfiles/functions.h"
#include "Headerfiles/inode.h"
#include "Headerfiles/inotifyFunctions.h"

extern int cookieValue1;
extern char movedName[MAX];

void createMode(struct inotify_event* event, int fd, char* path,
                char* sourceBase, char* backup, List* sourceList, int* watched,
                WDmapping** map, Tree *sourceTree) {
    INode* inode;
    char* backupTo;
    backupTo = malloc(MAX * sizeof(char));
    backupTo = formatBackupPath(sourceBase, backup, path);
    if (event->mask & IN_ISDIR) {
        makeDirectory(backupTo, event->name);

        addWatch(path, fd, event->name, watched, map);
    } else {
        char oldPath[MAX];
        char newPath[MAX];
        char thisPath[2*MAX];

        // make paths
        sprintf(oldPath, "%s/%s", path, event->name); // path of the new file
        sprintf(newPath, "%s/%s", backupTo, event->name);

        // check inode
        inode = searchForINodeByPath(sourceList, oldPath);
       
        // if(inode == NULL) fail("Inode can't be retrieved properly\n");
        // ---- Haven't tested this if case yet only the else one
        // check if the copy already exists
        // && (inode->copy != NULL)
        if ((inode != NULL)) {
            // there is a copy already (which means file created is a hardlink)
            // find a path of a file that is hardlinked to the same inode (in source)
            TreeNode *treeNodeHardlinked = searchByINodeNum(sourceTree->root, inode->inodeNum, oldPath);
            if (treeNodeHardlinked == NULL) {
                printf("is null\n");
            }
            char hardlinkedPath[MAX];
            strcpy(hardlinkedPath, treeNodeHardlinked->data.path);
            // so link it
             //
            sprintf(thisPath, "%s/%s", backupTo, inode->names->head->name); 
            if (link(thisPath, newPath) == -1) {
                printf(
                    " Failed to make a new hard link in -> %s, from -> %s \n",
                    newPath, oldPath);
                exit(1);
            }
        } else {
            // create a new copy to the backup
            int fdNewFile;
            fdNewFile = open(newPath, O_WRONLY | O_CREAT, 0644);
            close(fdNewFile);
        }
    }
    free(backupTo);
}

void attribMode(struct inotify_event* event, char* path, char* sourceBase,
                char* backup, List* sourceList) {
    struct stat statbuf;
    INode* inode;
    char buf[MAX];
    char fullPath[MAX];
    char* bPath;
    sprintf(fullPath, "%s/%s", realpath(path, buf), event->name);
    bPath = malloc(MAX * sizeof(char));
    bPath = formatBackupPath(sourceBase, backup, path);
    sprintf(bPath, "%s/%s", bPath, event->name);
    //
    if (!(event->mask & IN_ISDIR)) {
        // if it is a file
        if (stat(fullPath, &statbuf) == -1) {
            perror(" Failed to get file status \n");
            exit(1);
        }
        char sourcePath[MAX];
        sprintf(sourcePath, "%s/%s", path, event->name);
        inode = searchForINodeByPath(sourceList, sourcePath);
        if (inode == NULL) {
            perror(
                "make sure that you have called add inode after file creation\n");
            // exit(1);
        }
        if (!inode->modDate) {
            perror("inode mod data is null\n");
        }
        double seconds = difftime(statbuf.st_ctime, inode->modDate);
        if (seconds > 0) {
            // update the replica
            // update the moddate
            inode->modDate = statbuf.st_ctime;
            // rm old file from backup
            remove(bPath);
            // cp file from source to backup
            copy(fullPath, bPath);
        }
    }
    free(bPath);
}

void modifyMode(struct inotify_event* event, char* path, char* sourceBase,
                char* backup, List* backupList) {
    struct stat statbuf;
    INode* inode;
    char buf[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = formatBackupPath(sourceBase, backup, path);
    sprintf(bPath, "%s/%s", bPath, event->name);
    if (!(event->mask & IN_ISDIR)) {
        // if it is a file
        inode = searchForINodeByPath(backupList, bPath);
        if (inode == NULL) {
            perror("inode is null\n");
            exit(1);
        }
        // mark it as modified
        inode->modified = 1;
    }
    free(bPath);
}

void closeWriteMode(struct inotify_event* event, char* path, char* sourceBase,
                    char* backup, List* backupList) {
    char fullPath[MAX];
    struct stat statbuf;
    INode* inode;
    char buf[MAX];
    char buf1[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = formatBackupPath(sourceBase, backup, path);

    sprintf(bPath, "%s/%s", bPath, event->name);
    sprintf(fullPath, "%s/%s", realpath(path, buf), event->name);
    if (!(event->mask & IN_ISDIR)) {
        // if it is a file
        inode = searchForINodeByPath(backupList, bPath);
        if (inode == NULL) {
            perror("inode is null\n");
            exit(1);
        }
        // if it is marked as 
        
        if (inode->modified == 1) {
            // If it is a hardlink copy only the contents of the source 
            // file to backup file (so that inode doesn't change)
            sprintf(bPath, "%s", realpath(bPath, buf1));
            if( inode->nameCount > 1){
                printf("This is a hardlinked file in backup\n");
                int childExitStatus;
                pid_t pid;
                int status;
                
                pid = fork();
                if (pid == 0) {
                    execl("/bin/sh/cat", "cat", fullPath, " >> ", bPath, NULL);
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
            } else {   
                // cp file from source to backup
                copy(fullPath, bPath);
                // it is not modified any more
                inode->modified = 0;
            }
        }
    }
    free(bPath);
}

void deleteMode(struct inotify_event* event, char* path, char* sourceBase,
                char* backup) {
    // char fullPath[MAX];
    char buf[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = formatBackupPath(sourceBase, backup, path);

    sprintf(bPath, "%s/%s", bPath, event->name);
    if (!(event->mask & IN_ISDIR)) {
        // if it is a file
        if (unlink(bPath) != 0) {
            perror("An error occured when trying to delete file\n");
        }
    }
    free(bPath);
}

void deleteSelfMode(struct inotify_event* event, int fd, int wd, char* path,
                    char* sourceBase, char* backup) {
    char buf[MAX];
    char* bPath;
    pid_t pid;
    bPath = malloc(MAX * sizeof(char));
    bPath = formatBackupPath(sourceBase, backup, path);

    sprintf(bPath, "%s/", realpath(bPath, buf));

    // if it is a dir
    // remove it from backup
    if ((pid = fork()) == -1) {
        perror(" fork ");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
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
    // rmdir(bPath);

    inotify_rm_watch(fd, wd);
}

// file moved outside the watched dir
void movedFromMode(struct inotify_event* event, char* path, char* sourceBase,
                   char* backup) {
    // have to wait for the next event check if it is moved to and then check
    // the cookie field if cookie is the same the file remains in the same
    // folder just under different name event->cookie make a note of the cookie
    // val
    char buf[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = formatBackupPath(sourceBase, backup, path);
    sprintf(bPath, "%s", realpath(bPath, buf));
    sprintf(bPath, "%s/%s", bPath, event->name);
    cookieValue1 = event->cookie;
    strcpy(movedName, bPath);
    free(bPath);
}

// file moved inside the watched dir
int movedToMode(struct inotify_event* event, int fd, char* path,
                 char* sourceBase, char* backup, List* sourceList, int* watched,
                 WDmapping** map, Tree *sourceTree) {
    char buf[MAX], buf1[MAX];
    char* bPath;
    char* sourcePath;
    // flag will be returned and it shows if a file was moved from an exterior folder or not
    int flag = 0;
    bPath = malloc(MAX * sizeof(char));
    sourcePath = malloc(MAX * sizeof(char));
    //
    bPath = formatBackupPath(sourceBase, backup, path);
    sprintf(bPath, "%s", realpath(bPath, buf));
    sprintf(bPath, "%s/%s", bPath, event->name);

    sprintf(sourcePath,"%s/%s", realpath(path, buf1), event->name);

    if (cookieValue1 == event->cookie) {
        // it is in the same hierarchy
        rename(movedName, bPath);
        // delete the movedname afterwards we don't need it there
        flag = 1;
    } else {
        createMode(event, fd, path, sourceBase, backup, sourceList, watched,
                   map, sourceTree);
        copy(sourcePath, bPath);
        flag = 2;
    }
    cookieValue1 = 0;
    // clear the movedName
    memset(movedName, 0, sizeof(movedName));
    free(bPath);
    return flag;
}