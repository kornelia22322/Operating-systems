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

test_sort_sys:
	cp $(DATA_FILE) $(TEST_FILE)
	echo "\nsort sys" >> $(RESULT_FILE)
	$(TIME_S) ./main sort sys $(TEST_FILE) $(WIDTH) $(NUMBER)

test_copy_sys:
	cp $(DATA_FILE) $(TEST_FILE)
	echo "\ncopy sys" >> $(RESULT_FILE)
	$(TIME_S) ./main copy sys $(TEST_FILE)
