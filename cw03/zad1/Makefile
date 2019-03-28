CC = gcc
CFLAGS = -Wall

all: main

test:
	./main /usr ">" 18-03-2019 stat #jako ostatni argument nalezy podac sposob wykonania (stat/nftw)

main: main.o
	$(CC) $(CFLAGS) -o main main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o main