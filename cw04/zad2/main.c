#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <zconf.h>
#include <wait.h>
#include <sys/resource.h>

#include "monitor.h"
#include "command_manager.h"

child ** children = NULL;

void handler(int signal){
	end_processes(children);
	exit(0);
}

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

int get_number_of_lines(char* file_name){
	FILE* file;
	file = fopen(file_name,"r");
	int amount=0;
	size_t size =0;
	char* record;
	while(getline(&record,&size,file) != -1){
		amount++;
	}
	fclose(file);
	return amount;
}

void observe_files(char* list, char* type){
	int file_size = get_number_of_lines(list);
	children = calloc(sizeof(child*), file_size + 1);
	FILE *file;
	if( (file=fopen(list,"r")) == NULL){
		fprintf(stderr, "Unable to open given file\n");
		exit(1);
	}

	char* record;
	size_t size = 0;
	int pid;
	int index=0;
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
			observe(file_name,converted_period,type);
			pid = getpid();
			exit(0);
		}
		else{
			children[index]=create_child(file_name,pid);
			index++;
		}
	}
	free(record);
	fclose(file);
}

int is_int(char* str){
	char c;
	int i=0;
	while((c = str[i++]) != '\0'){
		if(48 > c || c > 57){
			return 0;
		}
	}
	return 1;
}

double get_time(struct timeval tm) {
	return tm.tv_sec*1000.0 + ((double) tm.tv_usec)/1000.0;
}

int main(int argc, char** argv) {
    if(argc != 3){
        fprintf(stderr, "Incorrect amount of arguments\n");
        return 1;
    }
    char* file = argv[1];
    char* type = argv[2];
    if(strcmp(type, "memory") != 0 && strcmp(type, "exec") != 0){
        fprintf(stderr, "Unknown type\n");
        return 1;
    }
	observe_files(file, type);

    signal(SIGINT, handler);

    char command[10];
    while (1){
    	scanf("%s",command);

		if(strcmp(command, "LIST") == 0){
			list_processes(children);
		}
    	else if(strcmp(command, "STOP") == 0){
			scanf("%s",command);
			if(is_int(command)){
    			stop_process(get_child_by_pid(children, atoi(command)));
    		}
    		else if(strcmp(command, "ALL") == 0){
    			stop_all_processes(children);
    		}
    		else{
    			fprintf(stderr, "After command STOP you should pass PID or \"ALL\"");
    		}
    	}
    	else if(strcmp(command, "START") == 0){
			scanf("%s",command);
			if(is_int(command)){
				start_process(get_child_by_pid(children,atoi(command)));
			}
			else if(strcmp(command, "ALL") == 0){
				start_all_processes(children);
			}
			else{
				fprintf(stderr, "After command START you should pass PID or \"ALL\"");
			}
    	}
    	else if(strcmp(command, "END") == 0){
    		end_processes(children);
			struct rusage ru;
			double sys_time = 0;
			double usr_time = 0;
			int status;
			int i=0;
			while(children[i] != NULL){
				waitpid(children[i]->pid, &status, 0);
				getrusage(RUSAGE_CHILDREN, &ru);
				printf("pid: %i\nczas sys: %f\nczas uzytk: %f\n",
					   children[i]->pid, get_time(ru.ru_stime) - sys_time, get_time(ru.ru_utime) - usr_time);
				sys_time += get_time(ru.ru_stime);
				usr_time += get_time(ru.ru_utime);

				free(children[i]->file_name);
				free(children[i]);
				i++;
			}
			free(children);
			return 0;
    	}
    	else{
    		fprintf(stderr, "Unknown command\n");
    	}
    }
    return 0;
}