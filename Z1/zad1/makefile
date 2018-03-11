CFLAGS += -Wall -g
CC = gcc

all: libarray.a libarray.so

book.o: array.c array.h
	$(CC) $(CFLAGS) -c array.c

libarray.a: array.o
	ar rcs $@ $^

libarray.so: array.c array.h
	$(CC) -fPIC -shared array.c -o $@


clean:
	-rm -rf *.o *.a *.so
