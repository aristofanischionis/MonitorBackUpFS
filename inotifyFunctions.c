#include "Headerfiles/inotifyCode.h"
#include <signal.h>
#include <fcntl.h>
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
void handleEvents(int fd, char* backup, List *sourceList, List *backupList, int *watched, WDmapping** map)
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
            /* Print the name of the file */

            if (event->len)
                printf("%s", event->name);

            // call the function to handle the event
            useFunction(event, fd, (*map)[j].name, backup, sourceList, backupList, watched, map, wd);
            // call readdirectories and traverse trees in order to update the logical structures

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

void useFunction(struct inotify_event *event, int fd, char* path, char* backup, List* sourceList, List* backupList, int *watched, WDmapping** map, int wd){
    if (event->mask & IN_ATTRIB){
        attribMode(event, path, backup, sourceList);
    }
    else if (event->mask & IN_CLOSE_WRITE){
        closeWriteMode(event, path, backup, backupList);
    }
    else if (event->mask & IN_CREATE){
        createMode(event, fd, path, backup, sourceList, watched, map);
    }
    else if (event->mask & IN_DELETE){
        deleteMode(event, path, backup);
    }
    else if (event->mask & IN_DELETE_SELF){
        deleteSelfMode(event, fd, wd, path, backup);
    }
    else if (event->mask & IN_MODIFY){
        modifyMode(event, path, backup, backupList);
    }
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
    backupTo = malloc(MAX * sizeof(char));
    backupTo = backupPath(path, backup);
    if (event->mask & IN_ISDIR){
        // printf(" [directory]\n");
        // create a catalog mkdir in the destination
        if (event->len <= 0){
            printf("event->len <= 0\n");
            exit(EXIT_FAILURE);
        }
        // making the dir
        printf("Make dir!\n");
        makeDirectory(backupTo, event->name);
        
        // monitor this catalog
        printf("Monitor Dir!\n");
        addWatch(path, fd, event->name, watched, map);
    }
    else {
        // printf(" [file]\n");
        char oldPath[MAX];
        char newPath[MAX];
        
        // make paths
        sprintf(oldPath, "%s%s", path, event->name);
        sprintf(newPath, "%s%s", backupTo, event->name);
        printf("Old path is %s, new path is %s \n", oldPath, newPath);
        ///
        // check inode
        inode = searchForINodeByPath(sourceList, oldPath);
        // if(inode == NULL) fail("Inode can't be retrieved properly\n");
        // ---- Haven't tested this if case yet only the else one
        // check if the copy already exists
        if ((inode != NULL) &&  (inode->copy != NULL) ){
            //there is a copy already (which means file created is a hardlink) so link it
            printf("There is already a copy so link it!\n");
            if ( link (oldPath ,newPath) == -1 ){
                printf(" Failed to make a new hard link in -> %s, from -> %s \n", newPath, oldPath);
                exit(1);
            }
        }
        else {
            // create a new copy to the backup 
            printf("There is not a copy so create it!\n");
            int fdNewFile;
            fdNewFile = open(newPath, O_WRONLY | O_CREAT, 0644);
            close(fdNewFile);
            // add inode copyy!!!!!
        }

    }
    free(backupTo);

}

void attribMode(struct inotify_event *event, char* path, char* backup, List* sourceList){
    struct stat statbuf;
    INode* inode;
    char buf[MAX];
    char fullPath[MAX];
    char* bPath;
    sprintf(fullPath, "%s/%s",realpath(path, buf), event->name);
    bPath = malloc(MAX * sizeof(char));
    bPath = backupPath(path, backup);
    sprintf(bPath, "%s%s", bPath, event->name);
    // 
    if (!(event->mask & IN_ISDIR)){
        // if it is a file
        printf("it is a file in attribMode %s\n", fullPath);
        if ( stat ( fullPath , & statbuf ) == -1){
            perror (" Failed to get file status \n");
            exit(1);
        }
        printf (" ctime : %s\n" , ctime(&statbuf.st_ctime));
        //
        // printf("------------> fullpath is %s \n", fullPath);
        inode = searchForINodeByPath(sourceList, fullPath);
        if(inode == NULL){
            perror("inode is null\n");
            perror("make sure that you have called add inode after file creation\n");
            // exit(1);
        }
        if(!inode->modDate){
            perror("inode mod data is null\n");
        }
        else {
            printf(" inode time : %s\n", ctime(&inode->modDate));
        }
        double seconds = difftime(statbuf.st_ctime, inode->modDate);
        if (seconds > 0) {
            // update the replica
            printf("update the replica\n");
            // update the moddate
            inode->modDate = statbuf.st_ctime;
            // rm old file from backup
            printf("I will remove %s \n", bPath);
            remove(bPath);
            // cp file from source to backup
            printf("I will make %s \n", bPath);
            copy(fullPath, bPath);
        }
        
    }
    free(bPath);
}

void modifyMode(struct inotify_event *event, char* path, char* backup, List* backupList){
    // char fullPath[MAX];
    struct stat statbuf;
    INode* inode;
    char buf[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = backupPath(path, backup);
    //
    sprintf(bPath, "%s%s", bPath, event->name);
    // sprintf(fullPath, "%s/%s",realpath(path, buf), event->name);
    if (!(event->mask & IN_ISDIR)){
        // if it is a file
        // printf("it is a file in modifyMode %s\n", fullPath);
        inode = searchForINodeByPath(backupList, bPath);
        if(inode == NULL){
            perror("inode is null\n");
            exit(1);
        }
        // mark it as modified
        inode->modified = 1;
    }
}

void closeWriteMode(struct inotify_event *event, char* path, char* backup, List* backupList){
    char fullPath[MAX];
    struct stat statbuf;
    INode* inode;
    char buf[MAX];
    char buf1[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = backupPath(path, backup);
    //
    sprintf(bPath, "%s%s", bPath, event->name);
    sprintf(fullPath, "%s/%s",realpath(path, buf), event->name);
    if (!(event->mask & IN_ISDIR)){
        // if it is a file
        printf("it is a file in closeWriteMode %s\n", fullPath);
        inode = searchForINodeByPath(backupList, bPath);
        if(inode == NULL){
            perror("inode is null\n");
            exit(1);
        }
        // if it is marked as modified
        if(inode->modified == 1){
            // copy it
            // rm old file from backup first
            sprintf(bPath, "%s", realpath(bPath, buf1));
            printf("I will remove %s \n", bPath);
            remove(bPath);
            // cp file from source to backup
            printf("I will make %s \n", bPath);
            copy(fullPath, bPath);
            // it is not modified any more
            inode->modified = 0;
        }
    }
    free(bPath);
}

void deleteMode(struct inotify_event *event, char* path, char* backup){
    // char fullPath[MAX];
    char buf[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = backupPath(path, backup);
    //
    sprintf(bPath, "%s%s", bPath, event->name);
    // sprintf(fullPath, "%s/%s",realpath(path, buf), event->name);
    if (!(event->mask & IN_ISDIR)){
        // if it is a file
        printf("it is a file in deleteMode %s\n", bPath);
        if (unlink(bPath) == 0){
            printf("File was deleted successfully from backup\n");
        }
        else perror("An error occured when trying to delete file\n");
    }
}

void deleteSelfMode(struct inotify_event *event, int fd, int wd, char* path, char* backup){
    char buf[MAX];
    char* bPath;
    bPath = malloc(MAX * sizeof(char));
    bPath = backupPath(path, backup);
    //
    sprintf(bPath, "%s/", realpath(bPath, buf));
   
    // if it is a dir
    printf("it is dir %s in DeleteSelf \n", bPath);
    // remove it from backup
    rmdir(bPath);
    //
    inotify_rm_watch(fd, wd);
    
}