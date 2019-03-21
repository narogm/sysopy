CC = gcc
CFLAGS = -Wall
OFLAG = -O0
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

all: static


static: main.o library.o
	$(CC) $(CFLAGS) $(OFLAG) -o main main.o library.o

shared: main.c library.so
	$(CC) $(CFLAGS) $(OFLAG) -o main main.c -Wl,-rpath=. -L. library.so 

main.o: main.c
	$(CC) $(CFLAGS) $(OFLAG) -c main.c

library.so: library.h library.o
	$(CC) $(CFLAGS) $(OFLAG) -shared -fPIC -o library.so library.o

library.o: library.c
	$(CC) $(CFLAGS) $(OFLAG) -fPIC -c library.c

library.a: library.o
	ar csr library.a library.o

clean:
	rm -f *.o *.so *.a main
