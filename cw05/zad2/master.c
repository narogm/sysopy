#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const int SIZE = 512;

int main(int argc, char **argv){
	if(argc != 2){
		fprintf(stderr, "Incorrect amount of arguments\n");
		exit(1);
	}
	char* fifo_path = argv[1];
	if(mkfifo(fifo_path,0666) == -1){
		fprintf(stderr,"Cannot make fifo\n");
		exit(1);
	}
	int fd = open(fifo_path, O_RDONLY);
	if(fd == -1){
		fprintf(stderr,"Cannot open fifo\n");
		exit(1);
	}
	char buf[SIZE];
	int size;
	while ((size=read(fd,buf,SIZE)) > 0){
		printf("Reading in master:\n%s\n",buf);
	}
	close(fd);
	return 0;
}