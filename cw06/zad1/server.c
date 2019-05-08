//
// Created by mateusz on 30.04.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "common.h"

void execute_echo(){

}

void proceed_msg(struct msg message){

	switch (message.msg_type){
		case STOP:
			break;
		case LIST:
			break;
		case FRIENDS:
			break;
		case TO_ALL:
			break;
		case TO_FRIENDS:
			break;
		case TO_ONE:
			break;
		case ECHO:

			break;
		default:
			fprintf(stderr, "Wrong message type\n");
	}
	STOP = 1,
	LIST = 2,
	FRIENDS = 3,
	TO_ALL = 4,
	TO_FRIENDS = 5,
	TO_ONE = 6,
	ECHO = 7
}

void signal_handler(){
	//stop all client
	//close queue
	exit(0);
}

int main(){
	if(make_queue()){
		//wypisanie bledu jesli nie udalo sie stworzyc kolejki
	}

	signal(SIGINT, signal_handler);
	while(1){
		struct msg message;

	}
}