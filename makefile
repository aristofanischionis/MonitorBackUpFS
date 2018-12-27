all: test test1 test2 test3

test: main_list.c
	gcc -g main_list.c namelist.c list.c -o test

test1: main_tree.c
	gcc -g main_tree.c tree.c -o test1

test2: main_tree2.c
	gcc -g main_tree2.c tree.c -o test2

test3: main_comb.c
	gcc -g main_comb.c tree.c -o test3

clean:
	rm -f test test1 test2 test3
