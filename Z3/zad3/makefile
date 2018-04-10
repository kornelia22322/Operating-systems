CFLAGS += -Wall -g
CC = gcc

all: main broken

main: main.c
	$(CC) $(CFLAGS) main.c -o main

broken: broken.c
	$(CC) $(CFLAGS) broken.c -o broken

clean:
	-rm -rf *.o main broken
