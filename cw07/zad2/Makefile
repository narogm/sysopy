CC = gcc
CFLAGS = -Wall

all: truck loader loaders_creator

truck: truck.c
	$(CC) $(CFLAGS) truck.c -o truck -lrt -lpthread

loader: loader.c
	$(CC) $(CFLAGS) loader.c -o loader -lrt -lpthread

loaders_creator: loaders_creator.c
	$(CC) $(CFLAGS) loaders_creator.c -o loaders_creator

clean:
	rm -f *.o truck loader loaders_creator