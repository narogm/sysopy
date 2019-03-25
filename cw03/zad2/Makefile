CC = gcc
CFLAGS = -Wall

all: main


main: main.o monitor.o
	$(CC) $(CFLAGS) -o main main.o monitor.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

monitor.o: monitor.c
	$(CC) $(CFLAGS) -c monitor.c

clean:
	rm -f *.o main