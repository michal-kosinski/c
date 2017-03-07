CC = gcc
FLAGS = -Wall

default: all

run:
	$(CC) $(CFLAGS) $@.c -o $@
suma:
	$(CC) $(CFLAGS) $@.c -o $@
roznica:
	$(CC) $(CFLAGS) $@.c -o $@

all: 	run suma roznica strip

strip:
	@strip run suma roznica

clean:
	rm -f run suma roznica
