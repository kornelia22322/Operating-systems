CC = gcc
CFLAGS = -g -O2 -std=gnu11 -w

all: main

main: main.c
	$(CC) $(CFLAGS) main.c -o main -pthread -lm

%.out: %.c makefile
	$(CC) $(CFLAGS) $< -o $(@:.out=)

clean: $(patsubst %.c, %.clean, $(wildcard *.c))

%.clean:
	rm -f $(@:.clean=)
