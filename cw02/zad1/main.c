#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>
#include <fcntl.h>
#include <zconf.h>

int generate(char** args, int i){
    char* file_name = args[i];
    int amount = atoi(args[i + 1]);
    int size = atoi(args[i+2]);

    time_t t;
    srand(time(&t));

    FILE *file = fopen(file_name,"a");
    if(file == NULL){
        fprintf(stderr, "Unable to open file\n");
        return -1;
    }
    char* record = malloc(sizeof(char)*size);
    while(amount--){
        for(int i=0; i<size; i++){
            record[i] = (char) (rand()%25 + 97);
        }
        record[size-1] = '\n';
        fwrite(record,size,1,file);
    }
    fclose(file);
    free(record);
    return 0;
}

int check_file_size(char* file_name, int amount, int size){
    FILE *file = fopen(file_name, "r");
    fseek(file,0L,SEEK_END);
    long file_size = ftell(file);
    fclose(file);
    if(file_size < amount * size){
        fprintf(stderr, "File is to small to sort given amount and size\n");
        return 1;
    }
    return 0;
}

int sys_sort(char* file_name, int amount, int size){
    if(check_file_size(file_name, amount, size)){
        return 1;
    }
    int fd = open(file_name,O_RDWR);
    if(fd == -1){
        fprintf(stderr, "Unable to open file\n");
        return -1;
    }

    char *min_record = malloc(sizeof(char) * size);
    char *curr_record = malloc(sizeof(char) * size);

    for(int cur=0; cur < amount; cur++){
        lseek(fd, size*cur, SEEK_SET);
        read(fd, min_record, size);
        int min_index = cur;

        for(int index=cur+1; index<amount; index++){
            read(fd, curr_record, 1);

            if(curr_record[0] < min_record[0]){
                min_index = index;
                min_record[0] = curr_record[0];
                read(fd, min_record + 1, size-1);
            } else{
                lseek(fd, size -1, SEEK_CUR);
            }
        }
        lseek(fd, cur*size, SEEK_SET);
        read(fd, curr_record, size);
        lseek(fd, cur*size, SEEK_SET);
        write(fd, min_record, size);
        lseek(fd, min_index*size, SEEK_SET);
        write(fd, curr_record, size);
    }

    close(fd);
    free(min_record);
    free(curr_record);
    return 0;
}

int lib_sort(char* file_name, int amount, int size){
    if(check_file_size(file_name, amount, size)){
        return 1;
    }
    FILE *file = fopen(file_name,"r+");
    if(file == NULL){
        fprintf(stderr, "Unable to open file\n");
        return -1;
    }
    char *min_record = malloc(sizeof(char) * size);
    char *curr_record = malloc(sizeof(char) * size);

    for(int cur=0; cur < amount; cur++){
        fseek(file, sizeof(char)*size*cur, SEEK_SET);
        fread(min_record, sizeof(char), size, file);
        int min_index = cur;

        for(int index=cur+1; index<amount; index++){
            fread(curr_record, sizeof(char), 1, file);

            if(curr_record[0] < min_record[0]){
                min_index = index;
                min_record[0] = curr_record[0];
                fread(min_record + 1, sizeof(char), size-1 , file);
            } else{
                fseek(file, sizeof(char)*(size -1), SEEK_CUR);
            }
        }
        fseek(file, sizeof(char)*cur*size, SEEK_SET);
        fread(curr_record, sizeof(char), size, file);
        fseek(file, sizeof(char)*cur*size, SEEK_SET);
        fwrite(min_record, sizeof(char), size, file);
        fseek(file, sizeof(char)*min_index*size, SEEK_SET);
        fwrite(curr_record, sizeof(char), size,file);
    }

    fclose(file);
    free(min_record);
    free(curr_record);

    return 0;
}

int sort(char** args, int i){
    char* file_name = args[i];
    int amount = atoi(args[i+1]);
    int size = atoi(args[i+2]);
    char* type = args[i+3];

    if(strcmp(type, "sys") == 0){
        return sys_sort(file_name, amount, size);
    }
    else if(strcmp(type, "lib") == 0){
        return lib_sort(file_name, amount, size);
    } else{
        fprintf(stderr, "Unknown way to sort file\n");
        return -1;
    }
}

int sys_copy(char* in_file, char* out_file, int amount, int size){
    int in = open(in_file,O_RDONLY);
    int out = open(out_file,O_WRONLY|O_CREAT, S_IRWXU|S_IRGRP|S_IROTH);
    if(in == -1 || out == -1){
        fprintf(stderr, "Unable to open file while copying\n");
        return -1;
    }
    char *record = malloc(sizeof(char)*size);
    while(amount--){
        if(read(in, record, size) == 0){
            fprintf(stderr, "File to copy is too small\n");
            return -1;
        }
        write(out, record, size);
    }
    close(in);
    close(out);
    free(record);
    return 0;
}

int lib_copy(char* in_file, char* out_file, int amount, int size){
    FILE *in = fopen(in_file, "r");
    FILE *out = fopen(out_file, "w");
    if(in == NULL || out == NULL){
        fprintf(stderr, "Unable to open file while copying");
        return -1;
    }
    char *record = malloc(sizeof(char)*size);
    while(amount--){
        if(fread(record, sizeof(char), size, in) == 0){
            fprintf(stderr, "File to copy is too small\n");
            return -1;
        }
        fwrite(record, sizeof(char), size, out);
    }

    fclose(in);
    fclose(out);
    free(record);
    return 0;
}

int copy(char** args, int i){
    char* in_file = args[i];
    char* out_file = args[i+1];
    int amount = atoi(args[i+2]);
    int size = atoi(args[i+3]);
    char* type = args[i+4];

    if(strcmp(type, "sys") == 0){
        return sys_copy(in_file, out_file, amount, size);
    }
    else if(strcmp(type, "lib") == 0){
        return lib_copy(in_file, out_file, amount, size);
    } else{
        fprintf(stderr, "Unknown way to sort file\n");
        printf("%s\n\n",type);
        return -1;
    }
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

    double user_time = get_time_difference(s.ru_utime, e.ru_utime);
    double sys_time = get_time_difference(s.ru_stime,e.ru_stime);

    FILE * file = fopen("wyniki.txt","a");
    fprintf(file,"Operation %s:\n user time: %f\n system time: %f\n\n", operation_name, user_time, sys_time);
    fclose(file);
    return 0;
}

int main(int argc, char** argv) {


    int i=1;
    int err;
    while(i<argc){
        char* operation = argv[i];

        if(strcmp(operation,"generate") == 0){
            check_number_of_args(argc,i,4);
            if((err = exec_operation(generate,argv,i+1,"generate")) != 0)
                return err;
            i = i+4;
        }
        else if(strcmp(operation,"sort") == 0){
            check_number_of_args(argc,i,5);
            char *type = argv[i+4];
            char *str= calloc(8, sizeof(char));
            strcat(str,"sort ");
            strcat(str,type);
            if((err = exec_operation(sort,argv,i+1,str)) != 0)
                return err;
            i = i+5;
            free(str);
        }
        else if(strcmp(operation,"copy") == 0){
            check_number_of_args(argc,i,6);
            char *type = argv[i+5];
            char *str= calloc(8, sizeof(char));
            strcat(str,"copy ");
            strcat(str,type);
            if((err = exec_operation(copy,argv,i+1,str)) != 0)
                return err;
            i = i+6;
            free(str);
        }
        else{
            fprintf(stderr,"Unknown operation\n");
            return -1;
        }

    }
    return 0;
}