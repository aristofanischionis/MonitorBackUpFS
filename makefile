all: test test1 test2 test3 test4

test: main_list.c
	gcc -g main_list.c namelist.c list.c -o test

test1: main_tree.c
	gcc -g main_tree.c tree.c -o test1

test2: main_tree2.c
	gcc -g main_tree2.c tree.c -o test2

test3: main_comb.c
	gcc -g main_comb.c functions.c tree.c traverse.c namelist.c list.c -o test3

test4: inotifyCode.c
	gcc -g inotifyCode.c inotifyFunctions.c -o test4

clean:
	rm -f test test1 test2 test3 test4
