//
// Created by mateusz on 30.04.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "common.h"

typedef struct client{
	int client_key;
	int queue_id;
	int closed;
}client;

int server_queue_key;
int clients_count = 0;

client clients[MAX_CLIENTS_AMOUNT];

void register_client(struct msg message){
	clients[clients_count].client_key = atoi(message.msg_data);
	clients[clients_count].queue_id = msgget(clients[clients_count].client_key, 0);
	clients[clients_count].closed = 0;
	sprintf(message.msg_data, "%i", clients_count);
	msgsnd(clients[clients_count].queue_id, &message, sizeof(message), 0);
	clients_count++;
}

void close_client(int id){
	clients[id].closed = 1;
}

void execute_echo(){

}

void proceed_msg(struct msg message){

	switch (message.msg_type){
		case STOP:
//			close_client(atoi(message.msg_data));
			printf("konczenie clienta\n");
			clients[atoi(message.msg_data)].closed = 1;
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
		case INIT:
			register_client(message);
			break;
		default:
			fprintf(stderr, "Wrong message type\n");
	}
}

void signal_handler(){
	for(int i=0; i<MAX_CLIENTS_AMOUNT; i++){
		if(!clients[i].closed){
			printf("elo\n");
			clients[i].closed = 1;
			send_msg(clients[i].queue_id, "", STOP_CLIENT);
		}
	}
	printf("obsluga sygnalu\n");
	msgctl(server_queue_key, IPC_RMID, NULL);
	exit(0);
}

int main(){
//	if(make_queue()){
//	}
	for(int i =0; i<MAX_CLIENTS_AMOUNT; i++){
		clients[i].closed = 1;
	}

    key_t key = ftok("server", 0);
    server_queue_key = msgget(key, 0666|IPC_CREAT);
    printf("server -> %d\n",server_queue_key);

    signal(SIGINT, signal_handler);

    struct msg message;
    msgrcv(server_queue_key, &message, sizeof(message), 8, 0);
    printf("Receiveed:\n%s\n", message.msg_data);
    register_client(message);

    int client_key = atoi(message.msg_data);
    printf("client_key %i\n",client_key);

//    int client_queue = msgget(client_key, 0);
//    sprintf(message.msg_data, "%i", ++clients_count);
//    msgsnd(client_queue, &message, sizeof(message), 0);
//    printf("no siema\n");
    while(1){
    	if(msgrcv(server_queue_key, &message, sizeof(message), STOP, IPC_NOWAIT) != -1){
    		//zatrzymaj clienta
    		proceed_msg(message);
    	}
    	else if(msgrcv(server_queue_key, &message, sizeof(message), LIST, IPC_NOWAIT) != -1){

    	}
    	else if(msgrcv(server_queue_key, &message, sizeof(message), 0, IPC_NOWAIT) != -1){

    	}
	}
}