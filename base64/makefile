all: test 
	./test

test: test.c base64.h makefile
	gcc test.c -o test -Wall -Wextra -fopenmp -std=c99

clean:
	rm -f ./test


