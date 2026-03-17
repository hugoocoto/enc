all: test
	./test

test: test.c sha256.h
	gcc test.c -o test -Wall -Wextra -ggdb -fopenmp
