#include "Headerfiles/inotifyCode.h"
#include <signal.h>
#include <fcntl.h>
#include <libgen.h>
#include "Headerfiles/traverse.h"
#include "Headerfiles/functions.h"
#include "Headerfiles/eventActions.h"


volatile sig_atomic_t running;
int cookieValue1 = 0;
char movedName[MAX];

void handle_sigint(int sig) 
{ 
    signal(SIGINT, handle_sigint);
    printf("Caught signal for program termination\n");
    running = 0;
   
}

// Opens directories and adds thems to watch list
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
void handleEvents(int fd, char *backup, List *sourceList, List *backupList, Tree **sourceTree, Tree **backupTree, int *watched, WDmapping** map)
{
    char currentName[MAX];
    int wd;
    running = 1;
    /* Some systems cannot read integer variables if they are not
              properly aligned. On other systems, incorrect alignment may
              decrease performance. Hence, the buffer used for reading from
              the inotify file descriptor should have the same alignment as
              struct inotify_event. */
    // signal(SIGINT, handle_sigint);

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
                    wd = (*map)[j].wd;
                    break;
                }
            }

            char *source = (*map)[j].name;
            /* Print the name of the file */
            char eventPath[MAX];
            sprintf(eventPath, "%s%s", source, event->name);
            // check if event happened in a file or a directory and create its path string
            // if (event->mask & IN_ISDIR) {
            //     strcpy(eventPath, source);
            //     // printf(" [directory]\n");
            // } else {
            //     sprintf(eventPath, "%s%s", source, event->name);
            //     // printf(" [file]\n");
            // }

            /* Print the name of the file */
            
            // event->len  = 0 when the event happens for the watched object(dir)
            // and event->len >0 when it happens for a file in the dir
            if (event->len)
                printf("%s\n", event->name);

            
            // check for moved case

            if(cookieValue1 != 0){
                // check for moved case
                printf("the previous event was a moved from with cookie : %d and name %s \n", cookieValue1, movedName);
                if(event->mask & IN_MOVED_TO){
                    // just go to use Function
                    printf("I will not do anything now, just let the movedtomode run\n");
                }
                else {
                    printf("Next event is not moved to so unlink previous file \n");
                    if (unlink(movedName) == 0){
                        printf("File was deleted successfully\n");
                        // clear the movedName
                        memset(movedName,0,sizeof(movedName));
                    }
                }
            }
            
            // call the function to handle the event
            useFunction(event, fd, source, backup, sourceList, backupList, watched, map, wd);

            updateSourceTree(event, eventPath, sourceTree, sourceList);
            printTree(*sourceTree);
            // readDirectory(source, &sourceList, (*sourceTree)->root);
            // printTree(*sourceTree);
            // traverseTrees(sourceTree, backupTree, &sourceList, &backupList);

            // call readdirectories and traverse trees in order to update the logical structures

            /* Print type of filesystem object */

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

// Depending on the event, update the source tree structure
void updateSourceTree(struct inotify_event* event, char* path, Tree **sourceTree, List *sourceList) {
    if (event->mask & IN_ATTRIB) {
        return;
    } 
    else if (event->mask & IN_CLOSE_WRITE) {
        return;
    } 
    else if (event->mask & IN_CREATE) {
        // make a copy of path to pass so that it doesn't change when passed to functions
        char *pathCopy = malloc(strlen(path)+1);
        strcpy(pathCopy, path);
        TreeNode *previous = searchByPath((*sourceTree)->root, dirname(pathCopy));
        strcpy(pathCopy, path);
        Data data;
        strcpy(data.path, path);
        strcpy(data.name, basename(pathCopy));
        INode *node = addINode(&sourceList, path);
        data.inode = node;
        addKid(previous, data);
        return;
    }
    else if (event->mask & IN_DELETE) {
        TreeNode *nodeFound = searchByPath((*sourceTree)->root, path);
        deleteINode(&sourceList, nodeFound->data.inode->inodeNum, nodeFound->data.name);
        deleteNode(*sourceTree, nodeFound);
        return;
    }
    else if (event->mask & IN_DELETE_SELF) {
        TreeNode *nodeFound = searchByPath((*sourceTree)->root, path);
        deleteNode(*sourceTree, nodeFound);
        return;
    }
    else if (event->mask & IN_MODIFY) {
        return;
    }
    else if (event->mask & IN_MOVED_FROM) {
        return;
    }
    else if (event->mask & IN_MOVED_TO) {
        return;
    }
}

void useFunction(struct inotify_event *event, int fd, char* path, char* backup, List* sourceList, List* backupList, int *watched, WDmapping** map, int wd){
    if (event->mask & IN_ATTRIB){
        printf("IN ATTRIB\n");
        attribMode(event, path, backup, sourceList);
    } else if (event->mask & IN_CLOSE_WRITE){
        printf("IN CLOSE WRITE\n");
        closeWriteMode(event, path, backup, backupList);
    } else if (event->mask & IN_CREATE){
        printf("IN CREATE\n");
        createMode(event, fd, path, backup, sourceList, watched, map);
    } else if (event->mask & IN_DELETE){
        printf("IN DELETE\n");
        deleteMode(event, path, backup);
    } else if (event->mask & IN_DELETE_SELF){
        printf("IN DELETE SELF\n");
        deleteSelfMode(event, fd, wd, path, backup);
    } else if (event->mask & IN_MODIFY){
        printf("IN MODIFY\n");
        modifyMode(event, path, backup, backupList);
    }
    else if (event->mask & IN_MOVED_FROM){
        printf("IN MOVE IN\n");
        movedFromMode(event, path, backup);
    }
    else if (event->mask & IN_MOVED_TO){
        printf("IN MOVE OUT\n");
        movedToMode(event, fd, path, backup, sourceList, watched, map);
    }
    else {
        return;
    }
}

void rmWD(WDmapping *map, int watched, int fd){
    int i;
    if( watched <= 0) return;
    for(i = 0; i< watched; i++){
        inotify_rm_watch( fd, map[i].wd );
    }
}

