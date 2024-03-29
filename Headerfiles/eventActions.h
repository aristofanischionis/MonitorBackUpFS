#ifndef EVENTACTIONS_HEADER
#define EVENTACTIONS_HEADER

#include <sys/inotify.h>
#include "list.h"
#include "monitoring.h"
#include "tree.h"

// Functions that implement the algorithms for each event
void createMode(struct inotify_event* event, int fd, char* path,
                char* sourceBase, char* backup, List* sourceList, int* watched,
                WDmapping** map, Tree *sourceTree);
void attribMode(struct inotify_event* event, char* path, char* sourceBase,
                char* backup, List* sourceList);
void modifyMode(struct inotify_event* event, char* path, char* sourceBase,
                char* backup, List* backupList);
void closeWriteMode(struct inotify_event* event, char* path, char* sourceBase,
                    char* backup, List* sourceList);
void deleteSelfMode(struct inotify_event* event, int fd, int wd, char* path,
                    char* sourceBase, char* backup);
void deleteMode(struct inotify_event* event, char* path, char* sourceBase,
                char* backup);
void movedFromMode(struct inotify_event* event, char* path, char* sourceBase,
                   char* backup);
int movedToMode(struct inotify_event* event, int fd, char* path,
                 char* sourceBase, char* backup, List* sourceList, int* watched,
                 WDmapping** map, Tree *sourceTree);

#endif