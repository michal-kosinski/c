CC = gcc
FLAGS = -Wall

default: all

eles:
	$(CC) $(CFLAGS) $@.c -o $@

50:
	$(CC) $(CFLAGS) $@.c -o $@

all: 	clean eles 50

strip:
	@strip eles 50 

clean:
	rm -f eles 50
