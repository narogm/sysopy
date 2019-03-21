CC = gcc
CFLAGS = -Wall
OFLAG = -O0
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

all: test

test: 
	@make --no-print-directory clean
	@make --no-print-directory static
	./main create_table 100 search_directory /usr main main_res search_directory /usr .c c_res remove_block 1 search_directory /usr .cpp cpp_res remove_block 2
	@make --no-print-directory clean
	@make --no-print-directory shared
	./main create_table 100 search_directory /usr main main_res search_directory /usr .c c_res remove_block 1 search_directory /usr .cpp cpp_res remove_block 2
	@make --no-print-directory clean
	@make --no-print-directory dynamic
	./main create_table 100 search_directory /usr main main_res search_directory /usr .c c_res remove_block 1 search_directory /usr .cpp cpp_res remove_block 2

static: main.o library.o
	$(CC) $(CFLAGS) $(OFLAG) -o main main.o library.o

shared: main.c library.so
	$(CC) $(CFLAGS) $(OFLAG) -o main main.c -Wl,-rpath=. -L. library.so 

dynamic: main.c library.so
	$(CC) $(CFLAGS) $(OFLAG) -o main main.c -g -Wl,-rpath=. -L. -ldl -D DLL library.so

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
