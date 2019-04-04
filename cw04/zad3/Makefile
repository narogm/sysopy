CC = gcc
CFLAGS = -Wall

all: catcher sender


catcher: catcher.o lib.o
	$(CC) $(CFLAGS) -o catcher catcher.o lib.o

catcher.o: catcher.c
	$(CC) $(CFLAGS) -c catcher.c

sender: sender.o lib.o
	$(CC) $(CFLAGS) -o sender sender.o lib.o

sender.o: sender.c
	$(CC) $(CFLAGS) -c sender.c

lib.o: lib.c
	$(CC) $(CFLAGS) -c lib.c

clean:
	rm -f *.o catcher sender