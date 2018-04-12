CFLAGS += -Wall -g
CC = gcc

all: main child

main: main.c
	$(CC) $(CFLAGS) main.c -o main

child: child.c
	$(CC) $(CFLAGS) child.c -o child

clean:
	-rm -rf *.o main child
