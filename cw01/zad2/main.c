#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <dlfcn.h>

#ifndef DLL
#include "library.h"
#endif

#ifdef DLL
    typedef struct file{
        char name[64];
        char location[512];
    } sys_file;
    void (*set_location)(sys_file * file, const char* location);
    void (*set_name)(sys_file * file, const char* name);
    char** (*create_table)(int size);
    int (*find_file)(sys_file * file, const char *tmp_location);
    int (*insert_to_table)(char** table, int table_size, const char* file_name);
    void (*remove_file)(char** file_table, int index);
    void (*clear_table)(char** file_table, int size);

#endif

char** table = NULL;
int TABLE_SIZE = 10;

int is_integer(char* value);

int create(char** args, int index){
    if(table != NULL){
        fprintf(stderr, "Table is already initialized\n");
        return -1;
    }
    char* size = args[index];
    if(!is_integer(size))
        return -1;
    TABLE_SIZE = atoi(size);
    table = create_table(TABLE_SIZE);
    return 0;
}

int search_directory(char** args, int index){
    char* dir = args[index];
    char* file_name = args[index+1];
    char* tmp_name = args[index+2];
    sys_file file;
    set_location(&file, dir);
    set_name(&file, file_name);
    return find_file(&file,tmp_name);

}

int remove_block(char** args, int i){
    char* index = args[i];
    if(!is_integer(index)){
        return 1;
    }
    remove_file(table, atoi(index));
    return 0;
}

int insert(char** args, int i){
    char* tmp_file_name = args[i];
    return insert_to_table(table,TABLE_SIZE,tmp_file_name);
}

void check_number_of_args(int argc, int start, int amount){
    if(start+amount > argc){
        fprintf(stderr, "Incorrect amount of arguments\n");
        exit(1);
    }
}

double get_time_difference(struct timeval start, struct timeval end){
    return ((double) end.tv_sec - start.tv_sec) * 1000.0f + ((double) end.tv_usec - start.tv_usec)/1000.0f;
}

int exec_operation(int (*operation)(char**,int), char** args, int i, const char* operation_name){
    struct rusage s,e;
    struct timeval start, end;
    getrusage(RUSAGE_SELF, &s);
    gettimeofday(&start,NULL);
    int err;
    if((err = operation(args,i)) == -1){
        return err;
    }
    getrusage(RUSAGE_SELF,&e);
    gettimeofday(&end,NULL);

    double real_time = get_time_difference(start,end);
    double user_time = get_time_difference(s.ru_utime, e.ru_utime);
    double sys_time = get_time_difference(s.ru_stime,e.ru_stime);

    FILE * file = fopen("raport2.txt","a");
    fprintf(file,"Operation %s:\n real time: %f\n user time: %f\n system time: %f\n", operation_name, real_time, user_time, sys_time);
    fclose(file);
    return 0;
}

int main(int argc, char** argv) {
#ifdef DLL
    void *handle = dlopen("library.so", RTLD_LAZY);
    if(!handle){
        fprintf(stderr, "Unable to load library.so\n");
        return 1;
    }
    set_location = dlsym(handle,"set_location");
    set_name = dlsym(handle, "set_name");
    create_table = dlsym(handle, "create_table");
    find_file = dlsym(handle, "find_file");
    insert_to_table = dlsym(handle, "insert_to_table");
    remove_file = dlsym(handle, "remove_file");
    clear_table = dlsym(handle, "clear_table");
#endif

    int i = 1;
    int err;
    while(i < argc){
        char* operation = argv[i];

        if(strcmp(operation, "create_table") == 0){
            check_number_of_args(argc, i, 2);
            if((err = exec_operation(create,argv,i+1,"creating table")) != 0)
                return err;
            i = i+2;
        }

        else if(strcmp(operation, "search_directory") == 0){
            check_number_of_args(argc,i,4);
            if((err = exec_operation(search_directory,argv,i+1,"searching directory")) != 0)
                return err;

            if((err = exec_operation(insert,argv,i+3,"inserting to table")) != 0)
                return err;
            i = i+4;
        }

        else if(strcmp(operation, "remove_block") == 0){
            check_number_of_args(argc,i,2);
            if((err = exec_operation(remove_block,argv,i+1,"removing block")) != 0)
                return err;
            i = i+2;
        }
        else{
            fprintf(stderr, "Unknown operation");
            return -1;
        }
    }
    clear_table(table,TABLE_SIZE);
#ifdef DLL
    dlclose(handle);
#endif

    return 0;
}

int is_integer(char* value){
    int i=0;
    char sign;
    while((sign = value[i++]) != '\0'){
        if(sign>57 || sign < 48){
            fprintf(stderr, "%s is not an integer\n", value);
            return 0;
        }
    }
    return 1;
}
