#include <zconf.h>
#include <memory.h>
#include <sys/stat.h>
#include "monitor.h"

time_t get_last_modification_time(char* path){
    struct stat st;
    if(stat(path,&st) == -1){
        return -1;
    }
    return st.st_mtime;
}

void observe_memory(char* file_name, int period, int lifetime, char* type){

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
}