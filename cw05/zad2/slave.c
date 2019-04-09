#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

const int SIZE = 64;

int extract_int(char *time) {
	char c;
	int i=0;
	while((c = time[i++]) != '\0'){
		if(48 > c || c > 57){
			fprintf(stderr, "Given argument as time is not an integer\n");
			exit(1);
		}
	}
	return atoi(time);
}


int main(int argc, char** argv){
	if(argc != 3){
		fprintf(stderr, "Incorrect amount of arguments\n");
		exit(1);
	}
	char* fifo_path = argv[1];
	int amount = extract_int(argv[2]);

	int fd = open(fifo_path, O_WRONLY);
	if(fd == -1){
		fprintf(stderr, "Cannot open fifo\n");
		exit(1);
	}

	srand(time(NULL));
	char date[SIZE];
	char buf[SIZE];
	for(int i=0; i<amount; i++){
		FILE * p;
		if((p = popen("date","r")) == NULL){
			fprintf(stderr,"error whit popen\n");
			exit(1);
		}
		int size = fread(date, sizeof(char),SIZE,p);
		sprintf(buf,"PID: %i DATE: %s",getpid(),date);
		buf[size] = '\0';
		write(fd,buf,SIZE);
		pclose(p);
		sleep(rand()%4+2);
	}
	close(fd);
	return 0;
}