all: mirr

mirr: main.c
	gcc -g main.c functions.c tree.c traverse.c namelist.c list.c inotifyCode.c inotifyFunctions.c -o mirr

# test4: inotifyCode.c
# 	gcc -g inotifyCode.c inotifyFunctions.c functions.c tree.c list.c namelist.c -o test4

clean:
	rm -f mirr
