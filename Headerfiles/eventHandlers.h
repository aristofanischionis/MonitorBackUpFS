#ifndef EVENTHANDLERS_HEADER
#define EVENTHANDLERS_HEADER

#include <sys/inotify.h>
#include "list.h"
#include "monitoring.h"
#include "tree.h"

// The fixed size of the event buffer:
#define EVENT_SIZE (sizeof(struct inotify_event))

// The size of the read buffer: estimate 1024 events with 16 bytes per name over
// and above the fixed size above
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

// Functions to handle an event
void handleEvents(int fd, char *backup, List *sourceList, List *backupList,
                  Tree **sourceTree, Tree **backupTree, int *watched,
                  WDmapping **map);
void makeAction(struct inotify_event *event, int fd, char *path,
                char *sourceBase, char *backup, List *sourceList,
                List *backupList, int *watched, WDmapping **map, int wd,
                char *eventPath, Tree **sourceTree, Tree **backupTree);
const char *eventName(struct inotify_event *event);

#endif