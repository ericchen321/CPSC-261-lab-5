CC = gcc
CFLAGS = -g -std=gnu11 -Og -Wall -Wno-unused-function

implicit-test: implicit-test.o implicit.o tests.o

clean:
	-/bin/rm -rf implicit-test implicit-test.o implicit.o tests.o
tidy: clean
	-/bin/rm -rf *~ .*~

implicit-test.o: implicit-test.c implicit.h	tests.h
tests.o: tests.c tests.h
implicit.o: implicit.c implicit.h	
