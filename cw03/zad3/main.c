#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wait.h>
#include <sys/resource.h>
#include <zconf.h>

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

int* observe_files(char* list, int lifetime, char* type, int time_limit, int memory_limit){
    FILE *file;
    if( (file=fopen(list,"r")) == NULL){
        fprintf(stderr, "Unable to open given file\n");
        exit(1);
    }
	fseek(file, 0, SEEK_END);
	long len = ftell(file);
	fseek(file, 0, SEEK_SET);
    size_t size = 0;
	int *pids = malloc(sizeof(int)*len+1);
	char* record;
	int i=0;
	while (getline(&record,&size,file) != -1){
        char* file_name, *period;
        file_name = strtok(record, " ");
        period = strtok(NULL, " ");
        if(file_name == NULL || period == NULL){
			fprintf(stderr, "File doesn't contain file name and time period in each line\n");
			exit(1);
		}
		int j=0;
		while(period[j++] != '\0'){
			if(period[j] == '\n') {
				period[j] = '\0';
			}
		}
		int converted_period = extract_int(period);
        int pid;
		if((pid = fork()) == 0){
			observe(file_name,converted_period,lifetime,type, time_limit, memory_limit);
			exit(0);
		} else {
			pids[i++] = pid;
		}

    }
    free(record);
    fclose(file);
    pids[i] = -1;
	return pids;
}

double get_time(struct timeval tm) {
	return tm.tv_sec*1000.0 + ((double) tm.tv_usec)/1000.0;
}

int main(int argc, char** argv) {
    if(argc != 6){
        fprintf(stderr, "Incorrect amount of arguments\n");
        return 1;
    }
    char* file = argv[1];
    int time = extract_int(argv[2]);
    char* type = argv[3];
    int time_limit = extract_int(argv[4]);
    int memory_limit = extract_int(argv[5]);
    if(strcmp(type, "memory") != 0 && strcmp(type, "exec") != 0){
        fprintf(stderr, "Unknown type\n");
        return 1;
    }
	int* pids = observe_files(file, time, type, time_limit, memory_limit);
	struct rusage ru;
	double sys_time = 0;
	double usr_time = 0;
	int status;
	int i=0;
	while(pids[i] != -1){
		waitpid(pids[i], &status, 0);
		getrusage(RUSAGE_CHILDREN, &ru);
		printf("pid: %i\nczas sys: %f\nczas uzytk: %f\n",
				pids[i], get_time(ru.ru_stime) - sys_time, get_time(ru.ru_utime) - usr_time);
		sys_time += get_time(ru.ru_stime);
		usr_time += get_time(ru.ru_utime);
		i++;
	}
    return 0;
}