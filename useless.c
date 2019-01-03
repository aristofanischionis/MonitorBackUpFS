#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include "Headerfiles/monitoring.h"

int watched;
WDmapping map[MAX_WD];
//The fixed size of the event buffer:
#define EVENT_SIZE (sizeof(struct inotify_event))

//The size of the read buffer: estimate 1024 events with 16 bytes per name over and above the fixed size above
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

void fail(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}
const char *targetType(int i)
{
    if (map[i].type == IS_DIRECTORY) return "directory";

    if (map[i].type == IS_FILE) return "file";
}

const char *targetName(const struct inotify_event *event)
{
    return event->len > 0 ? event->name : "";
}

const char *eventName(const struct inotify_event *event)
{
    if (event->mask & IN_ATTRIB)
        return "attrib";
    else if (event->mask & IN_CLOSE_WRITE)
        return "close write";
    else if (event->mask & IN_CREATE)
        return "create";
    else if (event->mask & IN_DELETE)
        return "delete";
    else if (event->mask & IN_DELETE_SELF)
        return "watch target deleted";
    else if (event->mask & IN_MODIFY)
        return "modify";
    else if (event->mask & IN_MOVED_FROM)
        return "moved out";
    else if (event->mask & IN_MOVED_TO)
        return "moved into";
    else
        return "unknown event";
}

/* Read all available inotify events from the file descriptor 'fd'.
          wd is the table of watch descriptors for the directories in argv.
          argc is the length of wd and argv.
          argv is the list of watched directories.
          Entry 0 of wd and argv is unused. */

static void handleEvents(int fd)
{
    /* Some systems cannot read integer variables if they are not
              properly aligned. On other systems, incorrect alignment may
              decrease performance. Hence, the buffer used for reading from
              the inotify file descriptor should have the same alignment as
              struct inotify_event. */

    char buf[EVENT_BUF_LEN]
        __attribute__((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    int i, j;
    ssize_t len;
    char *ptr;

    /* Loop while events can be read from inotify file descriptor. */

    while(1)
    {
        i = 0;
        /* Read some events. */

        len = read(fd, buf, sizeof buf);
        if (len == -1 && errno != EAGAIN)
            fail("read");

        /* If the nonblocking read() found no events to read, then
                  it returns -1 with errno set to EAGAIN. In that case,
                  we exit the loop. */

        if (len <= 0)
            break;

        /* Loop over all events in the buffer */

        while(i < len){

        
        // for (ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len)
        // {

            event = (const struct inotify_event *)&buf[i];

            /* Print the name of the watched directory */

            // for (i = 0; i < watched; i++)
            // {
            //     if (map[i].wd == event->wd)
            //     {
            //         printf("%s\n", map[i].name);
            //         break;
            //     }
            // }
            // // if()
            // // event is fully received, process
            // printf("WD:%d %s %s %s COOKIE=%u\n",
            //        event->wd, eventName(event),
            //        targetType(i), targetName(event), event->cookie);
            /* Print event type */

            printf("%s: ", eventName(event));
            if(!strcmp(eventName(event), "unknown event")){
                printf("the mask is %x ", event->mask);
            }

            /* Print the name of the watched directory */

            for (j = 1; j < watched; ++j)
            {
                if (map[j].wd == event->wd)
                {
                    printf("%s/", map[j].name);
                    break;
                }
            }

            /* Print the name of the file */

            if (event->len)
                printf("%s", event->name);

            /* Print type of filesystem object */

            if (event->mask & IN_ISDIR)
                printf(" [directory]\n");
            else
                printf(" [file]\n");

            i += EVENT_SIZE + event->len;
        }
    }
}

static void recursiveWatch(char *source, int fd)
{
    DIR *d;
    char path[MAX];
    /* Open the directory specified by "source". */

    d = opendir(source);
    /* Check it was opened. */
    if (!d)
    {
        fail("Cannot open directory source\n");
    }

    while (1)
    {
        struct dirent *entry;
        const char *d_name;

        /* "Readdir" gets subsequent entries from "d". */
        entry = readdir(d);
        if (!entry)
        {
            /* There are no more entries in this directory, so break
               out of the while loop. */
            break;
        }
        d_name = entry->d_name;
        //
        if (entry->d_type & DT_DIR)
        {

            /* Check if the directory is "d" or d's parent. */

            if ((!strcmp(d_name, "..")) || (!strcmp(d_name, ".")))
            {
                continue;
            }
        }
        //
        /* Print the name of the file and directory. */
        printf("Watched is: %d, %s/%s\n", watched, source, d_name);
        //
        sprintf(path, "%s/%s", source, d_name);
        strcpy(map[watched].name, path);
        map[watched].wd = inotify_add_watch(fd, path, 
        IN_CREATE |
        IN_MODIFY |
        
        IN_ATTRIB | 
        IN_CLOSE_WRITE |
        
        IN_DELETE |
        IN_DELETE_SELF |
        
        IN_MOVED_FROM |
        IN_MOVED_TO
        );
        
        if (map[watched].wd == -1) {
            fprintf(stderr, "Cannot watch '%s'\n", path);
            perror("inotify_add_watch");
            watched--;
            exit(EXIT_FAILURE);
        }
        if (entry->d_type & DT_DIR)
        {
            map[watched].type = IS_DIRECTORY;
        }
        else {
            map[watched].type = IS_FILE;
        }
        printf("watch added!\n");
        watched++;
        
        if (entry->d_type & DT_DIR)
        {
            /* Check that the directory is not "d" or d's parent. */

            if (strcmp(d_name, "..") != 0 &&
                strcmp(d_name, ".") != 0)
            {
                int path_length;
                char path[MAX];

                path_length = snprintf(path, MAX,
                                       "%s/%s", source, d_name);
                // printf("2nd time to see that dir so I didn't put a watch here %s\n", path);
                if (path_length >= MAX)
                {
                    fprintf(stderr, "Path length has got too long.\n");
                    exit(EXIT_FAILURE);
                }
                // add the dir 
                /* Recursively call "recursiveWatch" with the new path. */
                recursiveWatch(path, fd);
            }
        }
    }
    /* After going through all the entries, close the directory. */
    if (closedir(d))
    {
        fprintf(stderr, "Could not close '%s': %s\n",
                source, strerror(errno));
        exit(EXIT_FAILURE);
    }
}
int main(int argc, char *argv[])
{
    char buf;
    int fd, i, pollNum, watched = 0;
    // int *wd;
    // WDmapping map[MAX_WD];
    nfds_t nfds;
    struct pollfd fds[2];
    watched = 0;

    if (argc < 2)
        fail("Usage: ./mirr PATH [PATH ...]\n");

    printf("Press ENTER key to terminate.\n");

    /* Create the file descriptor for accessing the inotify API */

    fd = inotify_init();
    if (fd == -1)
        fail("inotify_init");

    recursiveWatch(argv[1], fd);
    
    // if (!map[0].wd)
    //     fail("Nothing to watch!");
    /* Prepare for polling */

    nfds = 2;

    /* Console input */

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    /* Inotify input */
    fds[1].fd = fd;
    fds[1].events = POLLIN;

    /* Wait for events and/or terminal input */

    printf("Listening for events.\n");
    while (1)
    {
        pollNum = poll(fds, nfds, -1);
        if (pollNum == -1)
        {
            if (errno == EINTR)
                continue;
            fail("poll");
        }

        if (pollNum > 0)
        {

            if (fds[0].revents & POLLIN)
            {

                /* Console input is available. Empty stdin and quit */

                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }

            if (fds[1].revents & POLLIN)
            {

                /* Inotify events are available */

                handleEvents(fd);
            }
        }
    }
    printf("Listening for events stopped.\n");

    /* Close inotify file descriptor */
    /* Clean up*/
    for (i = 0; i < watched; i++){
        inotify_rm_watch( fd, map[i].wd );
    }
    
    close(fd);

    // free(wd);
    exit(EXIT_SUCCESS);
}
