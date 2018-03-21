CFLAGS += -Wall -g
CC = gcc
DATA_FILE = datafile
TEST_FILE = testfile
RESULT_FILE = wyniki.txt
TIME_S = time -f "real %e user %U sys %S" -a -o $(RESULT_FILE)

all: main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

clean:
	-rm -rf *.o main $(DATA_FILE) $(TEST_FILE)

generate:
	./main generate $(DATA_FILE) $(WIDTH) $(NUMBER)
