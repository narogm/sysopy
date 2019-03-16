#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void generate(char** args, int i){

}

void sort(char** args, int i){

}

void copy(char** args, int i){

}



void check_number_of_args(int argc, int start, int amount){
    if(start+amount > argc){
        fprintf(stderr, "Incorrect amount of arguments\n");
        exit(1);
    }
}

int main(int argc, char** argv) {

    int i=0;
    while(i<argc){
        char* operation = NULL;

        if(strcmp(operation,"generate") == 0){
            check_number_of_args(argc,i,4);

            i = i+4;
        }
        if(strcmp(operation,"sort") == 0){
            check_number_of_args(argc,i,5);

            i = i+5;
        }
        if(strcmp(operation,"copy") == 0){
            check_number_of_args(argc,i,6);

            i = i+6;
        }

    }

    return 0;
}