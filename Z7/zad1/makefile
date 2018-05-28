CC = gcc
CFLAGS = -g -O2 -std=gnu99 -Wall -lrt

all: barber client

barber:
	$(CC) $(CFLAGS) helper.c barber.c -o barber

client:
	$(CC) $(CFLAGS) helper.c client.c -o client

clean: $(patsubst %.c, %.clean, $(wildcard *.c))

%.clean:
	rm -f $(@:.clean=)
