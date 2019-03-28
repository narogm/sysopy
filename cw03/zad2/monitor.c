#include <stdio.h>
#include <zconf.h>
#include <memory.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include "monitor.h"

time_t get_last_modification_time(char* file){
    struct stat st;
    if(stat(file,&st) == -1){
        return -1;
    }
    return st.st_mtime;
}

char* formatted_date(time_t t){
    char* date = malloc(sizeof(char) * 20);
    struct tm *tmTime;
    time(&t);
    tmTime = localtime(&t);
    strftime(date, 20, "%Y-%m-%d_%H-%M-%S", tmTime);
    return date;
}

char* get_content(char* file_name){
	struct stat st;
	if(stat(file_name,&st) != 0){
		fprintf(stderr, "stat error\n");
		exit(1);
	}

    FILE *file;
    if( (file = fopen(file_name,"r")) == NULL){
        fprintf(stderr,"Unable to get content from file\n");
        exit(1);
    }
    char* content = malloc(st.st_size+1);
	fread(content,1,st.st_size,file);
    fclose(file);
	content[st.st_size] = '\0';
	return content;
}

void observe_memory(char* file_name, int period, int lifetime){
	char* content;
	content = get_content(file_name);
	time_t modification_time = get_last_modification_time(file_name);
	while(lifetime > 0){
		time_t curr_modification = get_last_modification_time(file_name);
		if(curr_modification != modification_time){
			FILE * file;
            char path[512];
            char* date = formatted_date(modification_time);
            sprintf(path, "%s/%s_%s.txt", "./archiwum", file_name, date);
            if( (file = fopen(path,"w")) == NULL) {
                fprintf(stderr, "Unable to open file\n");
                exit(1);
            }
            fwrite(content, sizeof(char), strlen(content), file);
			free(content);
			content = get_content(file_name);

			modification_time = curr_modification;
            free(date);
        }
        int sleep_time;
        if(lifetime < period){
            sleep_time = lifetime;
        } else{
            sleep_time = period;
        }
        sleep(sleep_time);
        lifetime -= period;
	}
}

const char* file_name(const char *name) {
	char* ptr = strrchr(name, '/');
	return ptr == NULL ? name : ptr + 1;
}

void execute_exec(char* file, time_t modification){
	if(fork() == 0){
		char path[512];
		char* date = formatted_date(modification);
		sprintf(path, "%s/%s_%s.txt", "./archiwum", file_name(file), date);
		free(date);
		if(execlp("cp", "cp", file, path, NULL) == -1){
			fprintf(stderr, "Error while copying file with exec\n");
			exit(1);
		}
	}
}

void observe_exec(char* file, int period, int lifetime){
	time_t modification_time = -1;
	while(lifetime > 0){
		time_t curr_modification = get_last_modification_time(file);
		if(curr_modification != modification_time){
			execute_exec(file, curr_modification);
			modification_time = curr_modification;
		}

		int sleep_time;
		if(lifetime < period){
			sleep_time = lifetime;
		} else{
			sleep_time = period;
		}
		sleep(sleep_time);
		lifetime -= period;
	}
}

void observe(char* file, int period, int lifetime, char* type){
    if(fork() == 0){
        if(strcmp(type,"memory") == 0){
            observe_memory(file, period, lifetime);
        } else{
            observe_exec(file, period, lifetime);
        }
    }
}
