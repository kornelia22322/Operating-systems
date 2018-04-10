CFLAGS += -Wall -g
CC = gcc

all: main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

sample: sample.c
	$(CC) $(CFLAGS) sample.c -o sample

clean:
	-rm -rf *.o main sample
