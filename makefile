all: mirr

mirr: main.c
	gcc -g main.c functions.c tree.c traverse.c namelist.c list.c treeUpdates.c eventHandlers.c inotifyFunctions.c eventActions.c -o mirr

clean:
	rm -f mirr
