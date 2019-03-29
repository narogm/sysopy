CC = gcc
CFLAGS = -Wall

all: main tester


main: main.o monitor.o
	$(CC) $(CFLAGS) -o main main.o monitor.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

monitor.o: monitor.c
	$(CC) $(CFLAGS) -c monitor.c

tester: tester.o
	$(CC) $(CFLAGS) -o tester tester.o

tester.o: tester.c
	$(CC) $(CFLAGS) -c tester.c

clean:
	rm -f *.o main tester