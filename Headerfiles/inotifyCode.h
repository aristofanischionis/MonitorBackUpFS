#ifndef INOTIFYCODE_HEADER
#define INOTIFYCODE_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <poll.h>
#include <dirent.h>
#include <sys/stat.h>
#include "monitoring.h"
#include "list.h"

//The fixed size of the event buffer:
#define EVENT_SIZE  ( sizeof (struct inotify_event) )

//The size of the read buffer: estimate 1024 events with 16 bytes per name over and above the fixed size above
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

//helper function prototypes
const char * eventName(struct inotify_event *event);
void fail(const char *message) ;
void recursiveWatch(char *source, int fd, int *watched, WDmapping** map);
void addWatch(char *source, int fd, char* d_name, int *watched, WDmapping** map);
void useFunction(struct inotify_event *event, int fd, char* path, char* backup, List* list, int *watched, WDmapping** map);
void createMode(struct inotify_event *event, int fd, char* path, char* backup, List* sourceList, int *watched, WDmapping** map);
void handleEvents(int fd, char* backup, List *sourceList, int *watched, WDmapping** map);
void rmWD(WDmapping *map, int watched, int fd);
int inotifyCode(char* source, char* backup, List* sourceINodes);

#endif
