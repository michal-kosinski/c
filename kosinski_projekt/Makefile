CC = gcc
FLAGS = -Wall

default: all

szel:
	$(CC) $(CFLAGS) $@.c -o $@

all: 	clean szel

strip:
	@strip szel

clean:
	rm -f szel
