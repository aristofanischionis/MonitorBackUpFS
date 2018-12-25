all: test test1

test: main_list.c
	gcc -g main_list.c namelist.c list.c -o test

test1: main_tree.c
	gcc -g main_tree.c tree.c -o test1

clean:
	rm -f test test1
