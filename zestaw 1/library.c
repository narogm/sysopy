#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "library.h"

const char* tmp_location = "/tmp";

sys_file get_current_location(){
    sys_file file;
    strcpy(file.location, ".");
    strcpy(file.name, "");
    return file;
}

void set_location(sys_file * file, const char* location){
    strcpy(file -> location, location);
}

void set_name(sys_file * file, const char* name){
    strcpy(file -> name, name);
}

char** create_table(int size){
    return (char**) calloc(size, sizeof(char*));
}

int find_file(sys_file * file, const char *tmp_name){
    char command[1024];
    sprintf(command, "find %s -name \"*%s*\" > %s/%s", file->location, file->name, tmp_location, tmp_name);
    return system(command);
}

int get_index(char** table, int table_size);

char* get_file_content(const char* file_name);

int insert_to_table(char** table, int table_size, const char* file_name){
    if(table == NULL){
        fprintf(stderr, "Uninitialized table\n");
        return -1;
    }
    int index = get_index(table,table_size);
    if(index == -1){
        return -1;
    }
    if((table[index] = get_file_content(file_name)) == NULL)
        return -1;
    return index;
}

void remove_file(char** file_table, int index){
    free(file_table[index]);
    file_table[index] = NULL;
}

void clear_table(char** file_table, int size){
    if(file_table != NULL){
        for(int i=0; i<size; i++){
            remove_file(file_table,i);
        }
        free(file_table);
    }
}

int get_index(char** table, int table_size){
    for(int i=0; i<table_size; i++){
        if(table[i] == NULL) return i;
    }
    fprintf(stderr, "There isn't any free space in the table\n");
    return -1;
}

char* get_file_content(const char* file_name){
    FILE *file;
    char path[1024];
    sprintf(path, "%s/%s", tmp_location, file_name);
    file = fopen(path,"r");
    if(file == NULL){
        return NULL;
    }
    fseek(file,0,SEEK_END);
    long length = ftell(file);
    fseek(file,0,SEEK_SET);
    char* content = malloc(length);
    if(content){
        fread(content,1,length,file);
    }
    fclose(file);
    return content;
}