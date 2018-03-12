CFLAGS += -Wall -g
CC = gcc

LIBDIR = ../zad1
MAINDIR = ../zad2
MAIN = $(MAINDIR)/main.c

all: main_sta

main_sta: $(MAIN) $(LIBDIR)/libarray.a
	$(CC) $(CFLAGS) -DSTALAB=1 -o $@ $(MAIN) -L$(LIBDIR) -larray

$(LIBDIR)/libarray.a:
	cd $(LIBDIR) && $(MAKE) libarray.a

clean:
	-rm -rf *.o main_sta

clean_all:
	$(MAKE) clean
	cd $(LIBDIR) && $(MAKE) clean
