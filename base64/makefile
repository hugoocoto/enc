all: test
	./test

test: test.c base64.h
	gcc test.c -o test -DCOMPILE_TIME -Wall -Wextra -fopenmp
