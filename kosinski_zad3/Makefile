CC = gcc
FLAGS = -Wall

default: all

sigint:
	$(CC) $(CFLAGS) $@.c -o $@

sigchld:
	$(CC) $(CFLAGS) $@.c -o $@

all: 	sigint sigchld

strip:
	@strip sigint sigchld

clean:
	rm -f sigint sigchld
