CC = gcc -Wall
LIB_NAME = array

all: static shared

static:
	# compilation
	$(CC) -c $(LIB_NAME).c -o $(LIB_NAME).o

	# build static library
	ar rcs $(LIB_NAME).a $(LIB_NAME).o

shared:
	# build shared library .so
	$(CC) -fPIC -c $(LIB_NAME).c -o $(LIB_NAME).o
	$(CC) -fPIC -shared -o $(LIB_NAME).so $(LIB_NAME).o

clean:
	rm -f *.o
