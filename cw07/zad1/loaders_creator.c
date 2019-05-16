//
// Created by mateusz on 15.05.19.
//

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <zconf.h>
#include "common.h"

int A,N,C = -1;

void generate(){
	int i =0;
	while(i<A){
		if(fork() == 0){
			char n_to_str[10];
			sprintf(n_to_str, "%i", N);
			if(C == -1){
				execl("./loader", "./loader", n_to_str, NULL);
				//should not reach this
				fprintf(stderr, "Error while starting loader\n");
				exit(1);
			}
			else{
				char c_to_str[10];
				sprintf(c_to_str, "%i", C);
				execl("./loader", "./loader", n_to_str, c_to_str, NULL);
				//should not reach this
				fprintf(stderr, "Error while starting loader\n");
				exit(1);
			}
		}
		i++;
	}
}

int main(int argc, char** argv){
	if(argc != 3 && argc != 4){
		fprintf(stderr, "Incorrect amount of arguments\n");
		exit(1);
	}
	A = extract_int(argv[1]);
	N = extract_int(argv[2]);
	if(argc == 4){
		C = extract_int(argv[3]);
	}
	printf("elo\n");

	srand(time(0));
	generate();
	while (A --){
		wait(NULL);
	}
}