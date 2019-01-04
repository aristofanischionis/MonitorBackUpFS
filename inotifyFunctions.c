#include "Headerfiles/inotifyCode.h"
#include <signal.h>
#include <unistd.h>
#include "Headerfiles/list.h"


volatile sig_atomic_t running;

void handle_sigint(int sig) 
{ 
    signal(SIGINT, handle_sigint);
    printf("Caught signal for program termination\n");
    running = 0;
   
}

// take list of inodes
void handleEvents(int fd, int watched, WDmapping *map)
{
    char currentName[MAX];
    running = 1;
    /* Some systems cannot read integer variables if they are not
              properly aligned. On other systems, incorrect alignment may
              decrease performance. Hence, the buffer used for reading from
              the inotify file descriptor should have the same alignment as
              struct inotify_event. */
    signal(SIGINT, handle_sigint);

    int length, read_ptr, read_offset, j; //management of variable length events
	char buffer[EVENT_BUF_LEN];	//the buffer to use for reading the events
    /* Loop while events can be read from inotify file descriptor. */

    read_offset = 0; //remaining number of bytes from previous read
	while (running) {
		/* read next series of events */
		length = read(fd, buffer + read_offset, sizeof(buffer) - read_offset);
		if (length < 0) fail("read");

        if (running == 0) break;
		length += read_offset; // if there was an offset, add it to the number of bytes to process
		read_ptr = 0;
		
		// process each event
		// make sure at least the fixed part of the event in included in the buffer
		while (read_ptr + EVENT_SIZE <= length ) { 
			//point event to beginning of fixed part of next inotify_event structure
			struct inotify_event *event = (struct inotify_event *) &buffer[ read_ptr ];
			
			// if however the dynamic part exceeds the buffer, 
			// that means that we cannot fully read all event data and we need to 
			// deffer processing until next read completes
			if( (read_ptr + EVENT_SIZE + event->len > length) || (running == 0) ) 
				break;
			//event is fully received, process
            if (event->len){
                strcpy(currentName, event->name);
            }
            // check if it should be ignored
            if(currentName[0] == '.'){
                read_ptr += EVENT_SIZE + event->len;
                continue;
            }
            
            if(!strcmp(eventName(event), "unknown event")){
                // printf("the mask is %x ", event->mask);
                read_ptr += EVENT_SIZE + event->len;
                continue;
            }
            printf("%s: ", eventName(event));

            /* Print the name of the watched directory */
            for (j = 0; j < watched; j++)
            {
                if (map[j].wd == event->wd)
                {
                    printf("Name of watched dir is : %s | ", map[j].name);
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

			//advance read_ptr to the beginning of the next event
			read_ptr += EVENT_SIZE + event->len;
		}
		//check to see if a partial event remains at the end
		if( read_ptr < length ) {
			//copy the remaining bytes from the end of the buffer to the beginning of it
			memcpy(buffer, buffer + read_ptr, length - read_ptr);
			//and signal the next read to begin immediatelly after them			
			read_offset = length - read_ptr;
		} else
			read_offset = 0;
		
	}
	// typically, for each wd, need to: inotify_rm_watch(fd, wd);

}

void fail(const char *message) {
	perror(message);
	exit(1);
}

const char *eventName(struct inotify_event *event)
{
    if (event->mask & IN_ATTRIB)
        return "attrib";
    else if (event->mask & IN_CLOSE_WRITE)
        return "close write";
    else if (event->mask & IN_CREATE){
        createMode(event);
        return "create";
    }
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

void rmWD(WDmapping *map, int watched, int fd){
    int i;
    if( watched <= 0) return;
    for(i = 0; i< watched; i++){
        inotify_rm_watch( fd, map[i].wd );
    }
}

void createMode(struct inotify_event *event){

    if (event->mask & IN_ISDIR){
        // printf(" [directory]\n");

    }
    else {
        // printf(" [file]\n");
        searchForINode();
    }

}