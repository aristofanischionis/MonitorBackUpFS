#include "Headerfiles/inotifyCode.h"

int watched;
WDmapping map[MAX_WD];

int main(int argc, char *argv[])
{
	int fd; // descriptors returned from inotify subsystem
	if (argc < 2)
		fail("Must specify path(s) to watch");

	/*creating the INOTIFY instance*/
	fd = inotify_init();
	if (fd < 0)
		fail("inotify_init");

	watched = 0;

	// monitor root
	/* Print the name of the file and directory. */
	//
	sprintf(map[watched].name, "%s/", argv[1]);
	printf("Watched is: %d, %s\n", watched,map[watched].name);
	map[watched].wd = inotify_add_watch(fd, map[watched].name,
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
		fprintf(stderr, "Cannot watch '%s'\n", map[watched].name);
		perror("inotify_add_watch");
		watched--;
		exit(EXIT_FAILURE);
	}
	printf("watch added!\n");
	watched++;
	//
	recursiveWatch(argv[1], fd);

	if (watched == 0)
		fail("Nothing to watch!");

	printf("Listening for events!\n");

	handleEvents(fd, watched, map);

	printf("I stopped listening for events!\n");
	rmWD(map, watched, fd);
	close(fd);
}

void recursiveWatch(char *source, int fd)
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
			// sprintf(path, "%s%s/", source, d_name);
		}
		// else {
		// 	sprintf(path, "%s%s", source, d_name);
		// }
		// sprintf(path, "%s/%s", source, d_name);
		if (entry->d_type & DT_DIR)
		{
			sprintf(path, "%s/%s/", source, d_name);
		}
		else sprintf(path, "%s/%s", source, d_name);
		/* Print the name of the file and directory. */
		printf("Watched is: %d, %s\n", watched, path);
		//
		strcpy(map[watched].name, path);
		map[watched].wd = inotify_add_watch(fd, path,
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
			fprintf(stderr, "Cannot watch '%s'\n", path);
			perror("inotify_add_watch");
			watched--;
			exit(EXIT_FAILURE);
		}
		printf("watch added!\n");
		watched++;

		if (entry->d_type & DT_DIR)
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
	/* After going through all the entries, close the directory. */
	if (closedir(d))
	{
		fprintf(stderr, "Could not close '%s': %s\n",
				source, strerror(errno));
		exit(EXIT_FAILURE);
	}
}