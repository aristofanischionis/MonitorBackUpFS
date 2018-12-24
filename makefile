all: test

test: main_list.c
	gcc -g main_list.c list.c -o test

clean:
	rm -f test
