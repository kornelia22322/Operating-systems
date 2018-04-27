CFLAGS += -Wall
CC = gcc

all: server client

server: server.c common.h
	$(CC) $(CFLAGS) server.c -o server -lrt

client: client.c common.h
	$(CC) $(CFLAGS) client.c -o client -lrt 

clean:
	-rm -rf *.o server client
