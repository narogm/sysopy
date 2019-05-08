//
// Created by mateusz on 30.04.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "common.h"

int clients_count = 0;

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
//	if(make_queue()){
//	}

    key_t key = ftok("server", 0);
    int msgid = msgget(key, 0666|IPC_CREAT);
	signal(SIGINT, signal_handler);
    struct msg message;
    msgrcv(msgid, &message, sizeof(message), 7, 0);
    printf("Receiveed:\n%s\n", message.msg_data);

    client_key = atoi(message.msg_data);

    client_queue = msgget(client_key, 0);
    sprintf(message.msg_data, "%i", ++clients_count);
    msgnsd(client_key, &message, sizeof(message), 0);

//    while(1){
//
//	}
}