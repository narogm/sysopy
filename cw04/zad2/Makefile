CC = gcc
CFLAGS = -Wall

all: main tester


main: main.o monitor.o command_manager.o
	$(CC) $(CFLAGS) -o main main.o monitor.o command_manager.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

monitor.o: monitor.c
	$(CC) $(CFLAGS) -c monitor.c

command_manager.o: command_manager.c
	$(CC) $(CFLAGS) -c command_manager.c

tester: tester.o
	$(CC) $(CFLAGS) -o tester tester.o

tester.o: tester.c
	$(CC) $(CFLAGS) -c tester.c

clean:
	rm -f *.o main tester
