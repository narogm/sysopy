#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <zconf.h>
#include <wait.h>

#include "monitor.h"

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

void observe_files(char* list, int lifetime, char* type){
    FILE *file;
    if( (file=fopen(list,"r")) == NULL){
        fprintf(stderr, "Unable to open given file\n");
        exit(1);
    }
    char* record;
    size_t size = 0;
	int pid;
	while (getline(&record,&size,file) != -1){
		char* file_name, *period;
		file_name = strtok(record, " ");
		period = strtok(NULL, " ");
		if(file_name == NULL || period == NULL){
			fprintf(stderr, "File doesn't contain file name and time period in each line\n");
			exit(1);
		}
		int i=0;
		while(period[i++] != '\0'){
			if(period[i] == '\n') {
				period[i] = '\0';
			}
		}
		int converted_period = extract_int(period);
		if((pid = fork()) == 0){
			observe(file_name,converted_period,lifetime,type);
			pid = getpid();
			exit(0);
		}
    }
    free(record);
    fclose(file);
    int status;
    waitpid(pid,&status,0);
}

int main(int argc, char** argv) {
    if(argc != 4){
        fprintf(stderr, "Incorrect amount of arguments\n");
        return 1;
    }
    char* file = argv[1];
    int time = extract_int(argv[2]);
    char* type = argv[3];
    if(strcmp(type, "memory") != 0 && strcmp(type, "exec") != 0){
        fprintf(stderr, "Unknown type\n");
        return 1;
    }
    observe_files(file, time, type);
    return 0;
}