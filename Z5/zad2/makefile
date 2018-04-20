CFLAGS +=  -g
CC = gcc

all: slave master

main: main.c
	$(CC) $(CFLAGS) main.c -o main

slave: slave.c
	$(CC) $(CFLAGS) slave.c -o slave

master: master.c
	$(CC) $(CFLAGS) master.c -o master

clean:
	-rm -rf *.o main slave master
