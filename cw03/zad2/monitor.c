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
    FILE *file;
    if( (file = fopen(file_name,"r")) == NULL){
        fprintf(stderr,"Unable to get content from file\n");
        exit(1);
    }
    char* content = strdup("");
    char* line;
    size_t len;
    while(getline(&line, &len, file) != -1)
        strcat(content,line);
    fclose(file);
    return content;
}


void observe_memory(char* file_name, int period, int lifetime, char* type){
    char* content = NULL;
    time_t modification_time = -1;
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
            content = get_content(file_name);
            fwrite(content, sizeof(char), strlen(content), file);
            free(content);

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

void observe_exec(char* file_name, int period, int lifetime, char* type){

}

void observe(char* file_name, int period, int lifetime, char* type){
    if(fork() == 0){
        if(strcmp(type,"memory") == 0){
            observe_memory(file_name, period, lifetime, type);
        } else{
            observe_exec(file_name, period, lifetime, type);
        }
    }
    printf("%i",getpid());
}