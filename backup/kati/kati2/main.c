#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

//The fixed size of the event buffer:
#define EVENT_SIZE  (sizeof (struct inotify_event))

//The size of the read buffer: estimate 1024 events with 16 bytes per name over and above the fixed size above
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

void fail(const char *message) {
	perror(message);
	exit(EXIT_FAILURE);
}
const char * targetType(const struct inotify_event *event) {
	if( event->len == 0 )
		return "";
	else
		return event->mask & IN_ISDIR ? "directory" : "file";
}

const char * targetName(const struct inotify_event *event) {
	return event->len > 0 ? event->name : "";
}

const char * eventName(const struct inotify_event *event) {
	if (event->mask & IN_ACCESS)
		return "access";
	else if (event->mask & IN_ATTRIB)
		return "attrib";
	else if (event->mask & IN_CLOSE_WRITE)
		return "close write";
	else if (event->mask & IN_CLOSE_NOWRITE)
		return "close nowrite";
	else if (event->mask & IN_CREATE)
		return "create";
	else if (event->mask & IN_DELETE)
		return "delete";
	else if (event->mask & IN_DELETE_SELF)
		return "watch target deleted";
	else if (event->mask & IN_MODIFY)
		return "modify";
	else if (event->mask & IN_MOVE_SELF)
		return "watch target moved";
	else if (event->mask & IN_MOVED_FROM)
		return "moved out";
	else if (event->mask & IN_MOVED_TO)
		return "moved into";
	else if (event->mask & IN_OPEN)
		return "open";
	else
		return "unknown event";
}

/* Read all available inotify events from the file descriptor 'fd'.
          wd is the table of watch descriptors for the directories in argv.
          argc is the length of wd and argv.
          argv is the list of watched directories.
          Entry 0 of wd and argv is unused. */

static void handleEvents(int fd, int *wd, int argc, char *argv[]) {
    /* Some systems cannot read integer variables if they are not
              properly aligned. On other systems, incorrect alignment may
              decrease performance. Hence, the buffer used for reading from
              the inotify file descriptor should have the same alignment as
              struct inotify_event. */

    char buf[EVENT_BUF_LEN]
        __attribute__((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    int i;
    ssize_t len;
    char *ptr;

    /* Loop while events can be read from inotify file descriptor. */

    for (;;) {

        /* Read some events. */

        len = read(fd, buf, sizeof buf);
        if (len == -1 && errno != EAGAIN) fail("read");

        /* If the nonblocking read() found no events to read, then
                  it returns -1 with errno set to EAGAIN. In that case,
                  we exit the loop. */

  


        for (ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {

            event = (const struct inotify_event *)ptr;

            /* Print the name of the watched directory */

            for (i = 1; i < argc; ++i) {
                if (wd[i] == event->wd) {
                    printf("%s\n", argv[i]);
                    break;
                }
            }
            // event is fully received, process
			printf("WD:%i %s %s %s COOKIE=%u\n",
				event->wd, eventName(event),
				targetType(event), targetName(event), event->cookie);

        }
    }
}

int main(int argc, char *argv[]) {
    char buf;
    int fd, i, pollNum, watched = 0;
    int *wd;
    nfds_t nfds;
    struct pollfd fds[2];

    if (argc < 2) fail("Usage: ./mirr PATH [PATH ...]\n");

    printf("Press ENTER key to terminate.\n");

    /* Create the file descriptor for accessing the inotify API */

    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) fail("inotify_init1");

    /* Allocate memory for watch descriptors */

    wd = calloc(argc, sizeof(int));
    if (wd == NULL) fail("calloc");

    /* Mark directories for events
              - file was opened
              - file was closed */

    for (i = 1; i < argc; i++) {
        wd[i] = inotify_add_watch(fd, argv[i], IN_ALL_EVENTS);
        watched++;
        if (wd[i] == -1) {
            fprintf(stderr, "Cannot watch '%s'\n", argv[i]);
            perror("inotify_add_watch");
            watched--;
            // exit(EXIT_FAILURE);
        }
    }
    if( watched == 0 ) fail("Nothing to watch!");
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
    while (1) {
        pollNum = poll(fds, nfds, -1);
        if (pollNum == -1)
        {
            if (errno == EINTR)
                continue;
            fail("poll");
        }

        if (pollNum > 0) {

            if (fds[0].revents & POLLIN) {

                /* Console input is available. Empty stdin and quit */

                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }

            if (fds[1].revents & POLLIN) {

                /* Inotify events are available */

                handleEvents(fd, wd, argc, argv);
            }
        }
    }
    printf("Listening for events stopped.\n");

    /* Close inotify file descriptor */

    close(fd);

    free(wd);
    exit(EXIT_SUCCESS);
}
