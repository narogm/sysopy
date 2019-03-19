CC = gcc
CFLAGS = -Wall
SIZE = 1
AMOUNT = 20

all: main

RECORDSIZES = 4 512 1024 4096 8192
RECORDAMOUNT = 100

main: main.o
	$(CC) $(CFLAGS) -o main main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o main
