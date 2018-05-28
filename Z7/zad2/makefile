CC = gcc
CFLAGS = -g -O2 -std=gnu99

all: barber client

barber:
	$(CC) $(CFLAGS) helper.c barber.c -o barber -pthread -lrt

client:
	$(CC) $(CFLAGS) helper.c client.c -o client -pthread -lrt

clean: $(patsubst %.c, %.clean, $(wildcard *.c))

%.clean:
	rm -f $(@:.clean=)
