CC = gcc
FLAGS = -Wall

default: all strip

serwer:
	$(CC) $(CFLAGS) socklib.c $@.c -o $@

all:	serwer

strip:
	@strip serwer 

clean:
	rm -f serwer 
