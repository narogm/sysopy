#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>

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
        fwrite(record,1,size,file);
    }
    fclose(file);
    free(record);
    return 0;
}

int sort(char** args, int i){
    //char* file_name = args[i];
    //int amount = atoi(args[i+1]);
    //int size = atoi(args[i+2]);
    //char* type = args[i+3];

    return 0;
}

int copy(char** args, int i){
    //char* in_file = args[i];
    //char* out_file = args[i+1];
    //int amount = atoi(args[i+2]);
    //int size = atoi(args[i+3]);
    //char* type = args[i+4];

    return 0;
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

    //double real_time = get_time_difference(start,end);
    //double user_time = get_time_difference(s.ru_utime, e.ru_utime);
    //double sys_time = get_time_difference(s.ru_stime,e.ru_stime);

    //FILE * file = fopen("raport2.txt","a");
    //fprintf(file,"Operation %s:\n real time: %f\n user time: %f\n system time: %f\n", operation_name, real_time, user_time, sys_time);
    //fclose(file);
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
            if((err = exec_operation(sort,argv,i+1,"sort")) != 0)
                return err;
            i = i+5;
        }
        else if(strcmp(operation,"copy") == 0){
            check_number_of_args(argc,i,6);
            if((err = exec_operation(copy,argv,i+1,"copy")) != 0)
                return err;
            i = i+6;
        }
        else{
            fprintf(stderr,"Unknown operation\n");
            return -1;
        }

    }



    return 0;
}