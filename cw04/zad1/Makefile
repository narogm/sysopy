CC = gcc
CFLAGS = -Wall

all: simple


simple: simple.o
	$(CC) $(CFLAGS) -o simple simple.o

simple.o: simple.c
	$(CC) $(CFLAGS) -c simple.c

process: process.o
	$(CC) $(CFLAGS) -o process process.o

process.o: process.c
	$(CC) $(CFLAGS) -c process.c

clean:
	rm -f *.o simple process