CFLAGS += -Wall -g
CFLAGS += $(OPT)
CC = gcc

LIBDIR = ../zad1
MAINDIR = ../zad2
MAIN = $(MAINDIR)/main.c

all: main_sta main_sha main_dyn

main_sta: $(MAIN) $(LIBDIR)/libarray.a
	$(CC) $(CFLAGS) -DSTALAB=1 -o $@ $(MAIN) -L$(LIBDIR) -larray

main_sha: $(MAIN) $(LIBDIR)/libarray.so
	$(CC) $(CFLAGS) -DSHALAB=1 -o $@ $(MAIN) -L$(LIBDIR) -Wl,-rpath=$(LIBDIR) -larray

main_dyn: $(MAIN) $(LIBDIR)/libarray.so
	$(CC) $(CFLAGS) -DDYNLAB=1  -o $@ $(MAIN) -ldl

$(LIBDIR)/libarray.a:
	cd $(LIBDIR) && $(MAKE) libarray.a

$(LIBDIR)/libarray.so:
	cd $(LIBDIR) && $(MAKE) libarray.so

test:
	./main_sta >> result3b.txt && ./main_sha >> result3b.txt && ./main_dyn >> result3b.txt

run:
	$(MAKE) clean_all && $(MAKE) all OPT=$(OPT) && $(MAKE) test OPT=$(OPT)

clean:
	-rm -rf *.o main_sta main_sha main_dyn

clean_all:
	$(MAKE) clean
	cd $(LIBDIR) && $(MAKE) clean
