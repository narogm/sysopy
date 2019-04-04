#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "lib.h"

int is_int(char* str){
	int i=0;
	char c;
	while ( (c=str[i++]) != '\0'){
		if( 48 > c || c > 57){
			fprintf(stderr, "%s is not an integer", str);
			return 0;
		}
	}
	return 1;
}

int get_pid(char* str){
	if(!is_int(str)){
		fprintf(stderr, "Wrong format of pid\n");
		exit(1);
	}
	int pid = atoi(str);
	if( kill(pid,0) != 0){
		fprintf(stderr, "There is not process with given pid\n");
		exit(1);
	}
	return pid;
}

int get_signals_amount(char* str){
	if(!is_int(str)){
		fprintf(stderr, "Wrong format of signals amount\n");
		exit(1);
	}
	return atoi(str);
}

int get_type(char* str){
	if(strcmp(str,"KILL") == 0){
		return KILL;
	}
	else if(strcmp(str,"SIGQUEUE") == 0){
		return SIGQUEUE;
	}
	else if(strcmp(str,"SIGRT") == 0){
		return SIGRT;
	}
	else{
		fprintf(stderr, "Unknown type\n");
		exit(1);
	}
}

void send_signals_by_type(int pid, type type, int amount){
	switch (type){
		case KILL: {
			for(int i=0; i < amount; i++){
				kill(pid, SIGUSR1);
			}
			kill(pid,SIGUSR2);
			break;
		}
		case SIGQUEUE: {
			union sigval sv;
			for (int i = 0; i < amount; i++) {
				sigqueue(pid, SIGUSR1, sv);
			}
			sigqueue(pid,SIGUSR2,sv);
			break;
		}
		case SIGRT: {
			//TODO
			for(int i=0;i<amount;i++){
				kill(pid,SIGRTMIN);
			}
			kill(pid,SIGRTMIN+1);
//			if (sig == SIGUSR1)
//				kill(pid, SIGRTMIN + 1);
//			else if (sig == SIGUSR2)
//				kill(pid, SIGRTMIN + 2);
//			else
//				kill(pid, sig);
			break;
		}
	}
}