#include "Headerfiles/inotifyCode.h"
#include <signal.h>
#include <unistd.h>
#include "Headerfiles/list.h"
#include "Headerfiles/functions.h"


volatile sig_atomic_t running;

void handle_sigint(int sig) 
{ 
    signal(SIGINT, handle_sigint);
    printf("Caught signal for program termination\n");
    running = 0;
   
}

void recursiveWatch(char *source, int fd, int *watched, WDmapping** map)
{
	DIR *d;
	/* Open the directory specified by "source". */

	d = opendir(source);
	/* Check it was opened. */
	if (!d)
	{
		fail("Cannot open directory source in RecWatch\n");
	}

	while (1)
	{
		struct dirent *entry;
		char *d_name;

		/* "Readdir" gets subsequent entries from "d". */
		entry = readdir(d);
		if (!entry)
		{
			/* There are no more entries in this directory, so break
               out of the while loop. */
			break;
		}
		d_name = entry->d_name;
		// if it is the cur folder or the parent
		if (isDot(d_name))
			continue;
		// if it is a file don't add a wd
		if (isREG(entry->d_type))
			continue;


		addWatch(source, fd, d_name, watched, map);

		/* Recursively call "recursiveWatch" with the new path. */
        // printf("Name givenn--------> %s \n", (*map)[(*watched)-1].name);
		recursiveWatch((*map)[(*watched)-1].name, fd, watched, map);
	}
	/* After going through all the entries, close the directory. */
	if (closedir(d))
	{
		fprintf(stderr, "Could not close '%s': %s\n",
				source, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void addWatch(char *source, int fd, char* d_name, int *watched, WDmapping** map){
	char path[MAX];
	sprintf(path, "%s%s/", source, d_name);

		/* Print the name of the directory. */
		printf("Watched is: %d, %s\n", (*watched), path);
		//
		strcpy((*map)[(*watched)].name, path);
        printf("-->>>> %s \n", (*map)[(*watched)].name);
        fflush(stdout);
		(*map)[(*watched)].wd = inotify_add_watch(fd, path,
											IN_CREATE |
												IN_MODIFY |

												IN_ATTRIB |
												IN_CLOSE_WRITE |

												IN_DELETE |
												IN_DELETE_SELF |

												IN_MOVED_FROM |
												IN_MOVED_TO);

		if ((*map)[(*watched)].wd == -1)
		{
			fprintf(stderr, "Cannot watch '%s'\n", path);
			perror("inotify_add_watch");
			(*watched)--;
			exit(EXIT_FAILURE);
		}
		printf("watch added!\n");
		(*watched)++;
}

// take list of source inodes
void handleEvents(int fd, char* backup, List *sourceList, int *watched, WDmapping** map)
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
            for (j = 0; j < (*watched); j++)
            {
                if ((*map)[j].wd == event->wd)
                {
                    printf("Name of watched dir is : %s | ", (*map)[j].name);
                    break;
                }
            }
            /* Print the name of the file */

            if (event->len)
                printf("%s", event->name);

            // call the function to handle the event
            useFunction(event, fd, (*map)[j].name, backup, sourceList, watched, map);
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

void useFunction(struct inotify_event *event, int fd, char* path, char* backup, List* list, int *watched, WDmapping** map){
    if (event->mask & IN_ATTRIB)
        return;
    else if (event->mask & IN_CLOSE_WRITE)
        return;
    else if (event->mask & IN_CREATE){
        createMode(event, fd, path, backup, list, watched, map);
    }
    else if (event->mask & IN_DELETE)
        return;
    else if (event->mask & IN_DELETE_SELF)
        return;
    else if (event->mask & IN_MODIFY)
        return;
    else if (event->mask & IN_MOVED_FROM)
        return;
    else if (event->mask & IN_MOVED_TO)
        return;
    else
        return;
}

void rmWD(WDmapping *map, int watched, int fd){
    int i;
    if( watched <= 0) return;
    for(i = 0; i< watched; i++){
        inotify_rm_watch( fd, map[i].wd );
    }
}

void createMode(struct inotify_event *event, int fd, char* path, char* backup, List* sourceList, int *watched, WDmapping** map){
    INode *inode;
    char *backupTo;
    if (event->mask & IN_ISDIR){
        // printf(" [directory]\n");
        // create a catalog mkdir in the destination
        if (event->len <= 0){
            printf("event->len <= 0\n");
            exit(EXIT_FAILURE);
        }
        // monitor this catalog
        printf("Monitor Dir!\n");
        addWatch(path, fd, event->name, watched, map);
        printf("Make dir!\n");
        backupTo = malloc(MAX * sizeof(char));
        backupTo = backupPath(path, backup);
        makeDirectory(backupTo, event->name);
        free(backupTo);
    }
    else {
        // printf(" [file]\n");
        inode = searchForINodeByPath(sourceList, path);
        if (inode->copy != NULL ){
            //there is a copy already so link it
            printf("There is already a copy so link it!\n");
        }
        else {
            // create a new copy to the backup 
            printf("There is not a copy so create it!\n");
        }

    }

}

char* backupPath(char* sourcePath, char* backupBase){
    const char s[2] = "/";
    char *token;
    char toCopy[100];
    char *source, *backup;
    source = malloc(MAX * sizeof(char));
    backup = malloc(MAX * sizeof(char));
    strcpy(source, sourcePath);
    strcpy(backup, backupBase);

    token = strtok(source, s);

    while( token != NULL ) {
        token = strtok(NULL, s);
        if(token == NULL) break;
        sprintf(toCopy, "%s/", token);
        strcat(backup, toCopy);
    }
    printf("Backup path is : %s \n", backup);
    return backup;
}
