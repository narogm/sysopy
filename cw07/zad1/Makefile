CC = gcc
CFLAGS = -Wall

all: truck loader loaders_creator

truck: truck.o
	$(CC) $(CFLAGS) -o truck truck.o

truck.o: truck.c
	$(CC) $(CFLAGS) -c truck.c

loader: loader.o
	$(CC) $(CFLAGS) -o loader loader.o

loader.o: loader.c
	$(CC) $(CFLAGS) -c loader.c

loaders_creator: loaders_creator.o
	$(CC) $(CFLAGS) -o loaders_creator loaders_creator.o

loaders_creator.o: loaders_creator.c
	$(CC) $(CFLAGS) -c loaders_creator.c

clean:
	rm -f *.o truck loader loaders_creator