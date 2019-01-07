#ifndef INOTIFYCODE_HEADER
#define INOTIFYCODE_HEADER

#include <dirent.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "list.h"
#include "monitoring.h"
#include "tree.h"

// The fixed size of the event buffer:
#define EVENT_SIZE (sizeof(struct inotify_event))

// The size of the read buffer: estimate 1024 events with 16 bytes per name over
// and above the fixed size above
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

//helper function prototypes
const char * eventName(struct inotify_event *event);
void fail(const char *message) ;
void recursiveWatch(char *source, int fd, int *watched, WDmapping** map);
void addWatch(char *source, int fd, char* d_name, int *watched, WDmapping** map);
void useFunction(struct inotify_event *event, int fd, char* path, char* backup, List* sourceList, List* backupList, int *watched, WDmapping** map, int wd);
void createMode(struct inotify_event *event, int fd, char* path, char* backup, List* sourceList, int *watched, WDmapping** map);
void attribMode(struct inotify_event *event, char* path, char* backup, List* sourceList);
void modifyMode(struct inotify_event *event, char* path, char* backup, List* backupList);
void closeWriteMode(struct inotify_event *event, char* path, char* backup, List* sourceList);
void deleteSelfMode(struct inotify_event *event, int fd, int wd, char* path, char* backup);
void deleteMode(struct inotify_event *event, char* path, char* backup);
void movedFromMode(struct inotify_event *event, char* path, char* backup);
void movedToMode(struct inotify_event *event, int fd, char* path, char* backup, List* sourceList, int *watched, WDmapping** map);
void handleEvents(int fd, char* backup, List *sourceList, List *backupList, Tree **sourceTree, Tree **backupTree, int *watched, WDmapping** map);
void rmWD(WDmapping *map, int watched, int fd);
int inotifyCode(char* source, char* backup, List* sourceINodes, List *backupINodes, Tree **sourceTree, Tree **backupTree);
void updateSourceTree(struct inotify_event* event, char* path, Tree **sourceTree, List *sourceList);

#endif
