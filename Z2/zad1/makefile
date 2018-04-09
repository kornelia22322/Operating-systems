CFLAGS += -Wall -g
CC = gcc
DATA_FILE = datafile
TEST_FILE_SYS = testfile_sys
TEST_FILE_LIB = testfile_lib
COPY_FILE_SYS = copyfile_sys
COPY_FILE_LIB = copyfile_lib

RESULT_FILE = wyniki.txt
TIME_S = time -f "real %e user %U sys %S" -a -o $(RESULT_FILE)

all: main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

clean:
	-rm -rf *.o main $(DATA_FILE) $(TEST_FILE_SYS) $(TEST_FILE_LIB) $(COPY_FILE_SYS) $(COPY_FILE_LIB)

generate:
	./main generate $(DATA_FILE) $(WIDTH) $(NUMBER)

test_sort_sys:
	cp $(DATA_FILE) $(TEST_FILE_SYS)
	echo "\nsort sys" >> $(RESULT_FILE)
	$(TIME_S) ./main sort sys $(TEST_FILE_SYS) $(WIDTH) $(NUMBER)
	
test_sort_lib:
	cp $(DATA_FILE) $(TEST_FILE_LIB)
	echo "\nsort lib" >> $(RESULT_FILE)
	$(TIME_S) ./main sort lib $(TEST_FILE_LIB) $(WIDTH) $(NUMBER)

test_copy_sys:
	echo "\ncopy sys" >> $(RESULT_FILE)
	$(TIME_S) ./main copy sys $(DATA_FILE) $(COPY_FILE_SYS) $(WIDTH) $(NUMBER)

test_copy_lib:
	echo "\ncopy lib" >> $(RESULT_FILE)
	$(TIME_S) ./main copy lib $(DATA_FILE) $(COPY_FILE_LIB) $(WIDTH) $(NUMBER)
