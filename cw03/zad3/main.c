#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wait.h>
#include <sys/resource.h>

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
	char* record;
	size_t size = 0;
	fseek(file, 0, SEEK_END);
	long amount_of_files = ftell(file);
	fseek(file,0,SEEK_SET);
	int *pids = malloc(sizeof(int)*(amount_of_files+1));
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
			if(period[j] == '\n')
				period[j] = '\0';
		}
		int converted_period = extract_int(period);
		pids[i++]=observe(file_name,converted_period,lifetime,type, time_limit, memory_limit);
		printf("%i---------> %i\n",i,pids[i]);

	}
	pids[i] = -1;
	free(record);
	fclose(file);
	return pids;
}

double get_time(struct timeval time){
	return ((double) time.tv_sec) * 1000.0f + ((double) time.tv_usec)/1000.0f;
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

	int *pids = observe_files(file, time, type, time_limit, memory_limit);
	struct rusage usage;
	double sys_time = 0;
	double usr_time = 0;
	int status;
	int i=0;
	while(pids[i] != -1){
		waitpid(pids[i], &status, 0);
		getrusage(RUSAGE_CHILDREN, &usage);
		printf("pid: %i\n czas sys: %f\n",pids[i], get_time(usage.ru_stime) - sys_time);
		sys_time += get_time(usage.ru_stime);
		usr_time += get_time(usage.ru_utime);
		i++;
	}
	return 0;
}