CC = gcc
CFLAGS = -g -O2 -std=gnu99 -Wall

all: $(patsubst %.c, %.out, $(wildcard *.c))

%.out: %.c makefile
	$(CC) $(CFLAGS) $< -o $(@:.out=) -lpthread

clean: $(patsubst %.c, %.clean, $(wildcard *.c))

%.clean:
	rm -f $(@:.clean=)
