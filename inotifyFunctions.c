#include <fcntl.h>
#include <stdlib.h>
#include <libgen.h>
#include "Headerfiles/eventHandlers.h"
#include "Headerfiles/functions.h"
#include "Headerfiles/inotifyFunctions.h"
#include "Headerfiles/traverse.h"

// Add files to be watched by inotify
int inotifyCode(char *source, char *backup, List *sourceINodes, List *backupINodes, Tree **sourceTree, Tree **backupTree)
{
	int watched;
	int i;
	WDmapping *map;
	int fd; // descriptors returned from inotify subsystem
	if (!strcmp(source, ""))
		fail("Must specify path to watch");

	/*creating the INOTIFY instance*/
	fd = inotify_init();
	if (fd < 0)
		fail("inotify_init");

	watched = 0;
	map = (WDmapping*)malloc(MAX_WD*sizeof(WDmapping));
	// monitor root

	strcpy(map[watched].name, source);
	map[watched].wd = inotify_add_watch(fd, source,
										IN_CREATE |
											IN_MODIFY |

											IN_ATTRIB |
											IN_CLOSE_WRITE |

											IN_DELETE |
											IN_DELETE_SELF |

											IN_MOVED_FROM |
											IN_MOVED_TO);

	if (map[watched].wd == -1)
	{
		fprintf(stderr, "Cannot watch '%s'\n", source);
		perror("inotify_add_watch");
		watched--;
		exit(EXIT_FAILURE);
	}
	watched++;
	
	recursiveWatch(map[0].name, fd, &watched, &map);

	if (watched == 0)
		fail("Nothing to watch!");

	printf("Listening for events!\n");

	handleEvents(fd, backup, sourceINodes, backupINodes, sourceTree, backupTree, &watched, &map);

	printf("I stopped listening for events!\n");
	rmWD(map, watched, fd);
	close(fd);
	free(map);
}


// Opens directories and adds thems to watch list
void recursiveWatch(char *source, int fd, int *watched, WDmapping **map) {
    DIR *d;
    /* Open the directory specified by "source". */

    d = opendir(source);
    /* Check it was opened. */
    if (!d) {
        fail("Cannot open directory source in RecWatch\n");
    }

    while (1) {
        struct dirent *entry;
        char *d_name;

        /* "Readdir" gets subsequent entries from "d". */
        entry = readdir(d);
        if (!entry) {
            /* There are no more entries in this directory, so break
   out of the while loop. */
            break;
        }
        d_name = entry->d_name;
        // if it is the cur folder or the parent
        if (isDot(d_name)) continue;
        // if it is a file don't add a wd
        if (isREG(entry->d_type)) continue;

        addWatch(source, fd, d_name, watched, map);

        /* Recursively call "recursiveWatch" with the new path. */
        recursiveWatch((*map)[(*watched) - 1].name, fd, watched, map);
    }
    /* After going through all the entries, close the directory. */
    if (closedir(d)) {
        fprintf(stderr, "Could not close '%s': %s\n", source, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void addWatch(char *source, int fd, char *d_name, int *watched,
              WDmapping **map) {
    char path[MAX];
    sprintf(path, "%s%s/", source, d_name);

    strcpy((*map)[(*watched)].name, path);

    (*map)[(*watched)].wd = inotify_add_watch(fd, path,
                                              IN_CREATE | IN_MODIFY |

                                                  IN_ATTRIB | IN_CLOSE_WRITE |

                                                  IN_DELETE | IN_DELETE_SELF |

                                                  IN_MOVED_FROM | IN_MOVED_TO);

    if ((*map)[(*watched)].wd == -1) {
        fprintf(stderr, "Cannot watch '%s'\n", path);
        perror("inotify_add_watch");
        (*watched)--;
        exit(EXIT_FAILURE);
    }
    
    (*watched)++;
}

void rmWD(WDmapping *map, int watched, int fd) {
    int i;
    if (watched <= 0) return;
    for (i = 0; i < watched; i++) {
        inotify_rm_watch(fd, map[i].wd);
    }
}
