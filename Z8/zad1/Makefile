CC = gcc -Wall -std=gnu99 -pthread -lm


all: compile run

compile:
	$(CC) -o main main.c -pthread -lm


run:
	./main 1 lena.ascii.pgm filter.txt lena.ascii.1.pgm
	./main 2 lena.ascii.pgm filter.txt lena.ascii.2.pgm
	./main 4 lena.ascii.pgm filter.txt lena.ascii.3.pgm
	./main 8 lena.ascii.pgm filter.txt lena.ascii.4.pgm
	./main 1 lena.ascii.pgm filter2.txt lena.ascii.5.pgm
	./main 2 lena.ascii.pgm filter2.txt lena.ascii.6.pgm
	./main 4 lena.ascii.pgm filter2.txt lena.ascii.7.pgm
	./main 8 lena.ascii.pgm filter2.txt lena.ascii.8.pgm
	./main 1 lena.ascii.pgm filter3.txt lena.ascii.9.pgm
	./main 2 lena.ascii.pgm filter3.txt lena.ascii.10.pgm
	./main 4 lena.ascii.pgm filter3.txt lena.ascii.11.pgm
	./main 8 lena.ascii.pgm filter3.txt lena.ascii.12.pgm

clean:
	rm -f *.o *.a *.so res* Times.txt lena.ascii.*.pgm
