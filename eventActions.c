#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "Headerfiles/eventActions.h"
#include "Headerfiles/inode.h"
#include "Headerfiles/functions.h"
#include "Headerfiles/inotifyCode.h"

extern int cookieValue1;
extern char movedName[MAX];;

void createMode(struct inotify_event *event, int fd, char* path, char* backup, List* sourceList, int *watched, WDmapping** map){
    INode *inode;
    char *backupTo;
    backupTo = malloc(MAX * sizeof(char));
    backupTo = backupPath(path, backup);
    if (event->mask & IN_ISDIR){
        // printf(" [directory]\n");
        // create a catalog mkdir in the destination
        // if (event->len <= 0){
        //     printf("event->len <= 0\n");
        //     exit(EXIT_FAILURE);
        // }
        // making the dir
        printf("Make dir!\n");
        makeDirectory(backupTo, event->name);
        
        // monitor this catalog
        printf("Monitor Dir!\n");
        addWatch(path, fd, event->name, watched, map);
    }
    else {
        // printf(" [file]\n");
        char oldPath[MAX];
        char newPath[MAX];
        
        // make paths
        sprintf(oldPath, "%s%s", path, event->name);
        sprintf(newPath, "%s%s", backupTo, event->name);
        printf("Old path is %s, new path is %s \n", oldPath, newPath);
        ///
        // check inode
        inode = searchForINodeByPath(sourceList, oldPath);
        // if(inode == NULL) fail("Inode can't be retrieved properly\n");
        // ---- Haven't tested this if case yet only the else one
        // check if the copy already exists
        if ((inode != NULL) &&  (inode->copy != NULL) ){
            //there is a copy already (which means file created is a hardlink) so link it
            printf("There is already a copy so link it!\n");
            if ( link (oldPath ,newPath) == -1 ){
                printf(" Failed to make a new hard link in -> %s, from -> %s \n", newPath, oldPath);
                exit(1);
            }
        }
        else {
            // create a new copy to the backup 
            printf("There is not a copy so create it!\n");
            int fdNewFile;
            fdNewFile = open(newPath, O_WRONLY | O_CREAT, 0644);
            close(fdNewFile);
            // add inode !!!!!
        }

    }
    free(backupTo);

}

void attribMode(struct inotify_event *event, char* path, char* backup, List* sourceList){
    struct stat statbuf;
    INode* inode;
    char buf[MAX];
    char fullPath[MAX];
    char* bPath;
    sprintf(fullPath, "%s/%s",realpath(path, buf), event->name);
    bPath = malloc(MAX * sizeof(char));
    bPath = backupPath(path, backup);
    sprintf(bPath, "%s%s", bPath, event->name);
    // 
    if (!(event->mask & IN_ISDIR)){
        // if it is a file
        printf("it is a file in attribMode %s\n", fullPath);
        if ( stat ( fullPath , & statbuf ) == -1){
            perror (" Failed to get file status \n");
            exit(1);
        }
        printf (" ctime : %s\n" , ctime(&statbuf.st_ctime));
        //
        // printf("------------> fullpath is %s \n", fullPath);
        inode = searchForINodeByPath(sourceList, fullPath);
        if(inode == NULL){
            // perror("inode is null\n");/
            perror("make sure that you have called add inode after file creation\n");
            // exit(1);
        }
        if(!inode->modDate){
            perror("inode mod data is null\n");
        }
        else {
            printf(" inode time : %s\n", ctime(&inode->modDate));
        }
        double seconds = difftime(statbuf.st_ctime, inode->modDate);
        if (seconds > 0) {
            // update the replica
            printf("update the replica\n");
            // update the moddate
            inode->modDate = statbuf.st_ctime;
            // rm old file from backup
            printf("I will remove %s \n", bPath);
            remove(bPath);
            // cp file from source to backup
            printf("I will make %s \n", bPath);
            copy(fullPath, bPath);
        }
        
    }
    free(bPath);
}

void modifyMode(struct inotify_event *event, char* path, char* backup, List* backupList){
    // char fullPath[MAX];
    struct stat statbuf;
    INode* inode;
    char buf[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = backupPath(path, backup);
    //
    sprintf(bPath, "%s%s", bPath, event->name);
    // sprintf(fullPath, "%s/%s",realpath(path, buf), event->name);
    if (!(event->mask & IN_ISDIR)){
        // if it is a file
        // printf("it is a file in modifyMode %s\n", fullPath);
        inode = searchForINodeByPath(backupList, bPath);
        if(inode == NULL){
            perror("inode is null\n");
            exit(1);
        }
        // mark it as modified
        inode->modified = 1;
    }
    free(bPath);
}

void closeWriteMode(struct inotify_event *event, char* path, char* backup, List* backupList){
    char fullPath[MAX];
    struct stat statbuf;
    INode* inode;
    char buf[MAX];
    char buf1[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = backupPath(path, backup);
    //
    sprintf(bPath, "%s%s", bPath, event->name);
    sprintf(fullPath, "%s/%s",realpath(path, buf), event->name);
    if (!(event->mask & IN_ISDIR)){
        // if it is a file
        printf("it is a file in closeWriteMode %s\n", fullPath);
        inode = searchForINodeByPath(backupList, bPath);
        if(inode == NULL){
            perror("inode is null\n");
            exit(1);
        }
        // if it is marked as modified
        if(inode->modified == 1){
            // copy it
            // rm old file from backup first
            sprintf(bPath, "%s", realpath(bPath, buf1));
            printf("I will remove %s \n", bPath);
            remove(bPath);
            // cp file from source to backup
            printf("I will make %s \n", bPath);
            copy(fullPath, bPath);
            // it is not modified any more
            inode->modified = 0;
        }
    }
    free(bPath);
}

void deleteMode(struct inotify_event *event, char* path, char* backup){
    // char fullPath[MAX];
    char buf[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = backupPath(path, backup);
    //
    sprintf(bPath, "%s%s", bPath, event->name);
    // sprintf(fullPath, "%s/%s",realpath(path, buf), event->name);
    if (!(event->mask & IN_ISDIR)){
        // if it is a file
        printf("it is a file in deleteMode %s\n", bPath);
        if (unlink(bPath) == 0){
            printf("File was deleted successfully from backup\n");
        }
        else perror("An error occured when trying to delete file\n");
    }
    free(bPath);
}

void deleteSelfMode(struct inotify_event *event, int fd, int wd, char* path, char* backup){
    char buf[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = backupPath(path, backup);
    //
    sprintf(bPath, "%s/", realpath(bPath, buf));
   
    // if it is a dir
    printf("it is dir %s in DeleteSelf \n", bPath);
    // remove it from backup
    rmdir(bPath);
    //
    inotify_rm_watch(fd, wd);
    
}

// file moved outside the watched dir
void movedFromMode(struct inotify_event *event, char* path, char* backup){
    // have to wait for the next event check if it is moved to and then check the cookie field
    // if cookie is the same the file remains in the same folder just under different name
    // event->cookie
    // make a note of the cookie val
    char buf[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = backupPath(path, backup);
    //
    sprintf(bPath, "%s/", realpath(bPath, buf));
    sprintf(bPath, "%s%s", bPath, event->name);
    printf("I am moved from mode and the path of the file is %s \n", bPath);
    cookieValue1 = event->cookie;
    strcpy(movedName, bPath);
    free(bPath);
}

// file moved inside the watched dir
void movedToMode(struct inotify_event *event, int fd, char* path, char* backup, List* sourceList, int *watched, WDmapping** map){
    char buf[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = backupPath(path, backup);
    //
    sprintf(bPath, "%s/", realpath(bPath, buf));
    sprintf(bPath, "%s%s", bPath, event->name);
    printf("I am moved to mode and the path of the file is %s \n", bPath);

    if(cookieValue1 == event->cookie){
        // it is the same hierarchy
        // must move name to the correct folder
        printf("same cookies so copy and unlink the previous file\n");
        rename(movedName, bPath);
        // copy(movedName, bPath);
        // delete the movedname afterwards we don't need it there
        if(unlink(movedName) == 0 ) printf("Deleted the file successfully!\n");
    }
    else {
        printf("not the same cookies\n");
        createMode(event, fd, path, backup, sourceList, watched, map);
        copy(movedName, bPath);
    }
    cookieValue1 = 0;
    // clear the movedName
    memset(movedName,0,sizeof(movedName));
    free(bPath);
}