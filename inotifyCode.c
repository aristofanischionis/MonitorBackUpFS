#include "Headerfiles/inotifyCode.h"
#include "Headerfiles/functions.h"

int inotifyCode(char *source, char *backup, List *sourceINodes)
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

	/* Print the name of the directory. */
	printf("Watched is: %d, %s\n", watched, source);
	//

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
	printf("watch added!\n");
	watched++;
	
	//
	recursiveWatch(map[0].name, fd, &watched, &map);

	if (watched == 0)
		fail("Nothing to watch!");

	printf("Listening for events!\n");

	handleEvents(fd, backup, sourceINodes, &watched, &map);

	printf("I stopped listening for events!\n");
	rmWD(map, watched, fd);
	close(fd);
	free(map);
}
