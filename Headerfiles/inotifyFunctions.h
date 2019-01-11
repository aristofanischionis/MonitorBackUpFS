#ifndef INOTIFYFUNCTIONS_HEADER
#define INOTIFYFUNCTIONS_HEADER

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

// Functions to watch the file system
void fail(const char *message);
void recursiveWatch(char *source, int fd, int *watched, WDmapping **map);
void addWatch(char *source, int fd, char *d_name, int *watched,
              WDmapping **map);
void rmWD(WDmapping *map, int watched, int fd);
int inotifyCode(char *source, char *backup, List *sourceINodes,
                List *backupINodes, Tree **sourceTree, Tree **backupTree);

#endif