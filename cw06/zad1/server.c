//
// Created by mateusz on 30.04.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include "common.h"

typedef struct client{
	int client_key;
	int queue_id;
	int closed;
	int client_pid;
	int is_friend;
}client;

int server_queue_key;
int clients_count = 0;
client clients[MAX_CLIENTS_AMOUNT];
int friends[MAX_CLIENTS_AMOUNT];

char* get_curr_date(){
	time_t curr_time;
	char* result;
	curr_time = time(NULL);
	result = ctime(&curr_time);
	return result;
}

int* parse_list(char* list){
	char *buff = strdup(list);
//	printf("|%s|\n", buff);
	int * result = malloc(sizeof(int) * MAX_CLIENTS_AMOUNT);
	char *tmp;
	tmp = strtok(buff, " ");
	int index = 0;
//	printf("Parsing list from %s\n", list);
	while(tmp != NULL){
//		printf("index %i\n", index);
		result[index++] = atoi(tmp);
		tmp = strtok(NULL, " ");
//		printf("|%s|\n", tmp);
//		printf("test\n");
	}
	if(index < MAX_CLIENTS_AMOUNT) {
		result[index] = -1;
	}
	return result;
}

void register_client(struct msg message){
	printf("registering client\n");
	if(clients_count+1 < MAX_CLIENTS_AMOUNT){
		clients[clients_count].client_key = atoi(message.msg_data);
		clients[clients_count].queue_id = msgget(clients[clients_count].client_key, 0);
		clients[clients_count].client_pid = message.client_id;
		clients[clients_count].closed = 0;
		clients[clients_count].is_friend = 0;
		sprintf(message.msg_data, "%i", clients_count);
		msgsnd(clients[clients_count].queue_id, &message, sizeof(message), 0);
		clients_count++;
	}
	else {
		fprintf(stderr, "Unable to add next client -> the table is full\n");
	}

}

void list(struct msg message){
	char response[MSG_SIZE];
	int i=0;
	while(i < MAX_CLIENTS_AMOUNT){
		if(!clients[i].closed && clients[i].is_friend){
			sprintf(response, "%s %i", response, i);
		}
		i++;
	}
	send_msg(clients[message.client_id].queue_id, response, MSG_CLIENT, -1);
}

void friend(struct msg message){
	printf("responding to friend\n");
	char* list = strdup(message.msg_data);
	int* new_friends = parse_list(list);
	int i = 0;
	printf("elo xDDD\n");
	while((i < MAX_CLIENTS_AMOUNT) && (new_friends[i] != -1)){
		printf("elo\n");
		if(!clients[new_friends[i]].closed){
			printf("no siema\n");
			clients[new_friends[i]].is_friend = 1;
		}
		i++;
	}
	send_msg(clients[message.client_id].queue_id, "", MSG_CLIENT, -1);
}

void to_all(struct msg message){
	printf("to all\n");
	char response[MSG_SIZE];
	sprintf(response, "%s %i %s", message.msg_data, message.client_id, get_curr_date());
	for(int i=0;i<MAX_CLIENTS_AMOUNT;i++){
		if(!clients[i].closed && i!=message.client_id){
			send_msg(clients[i].queue_id, response, MSG_CLIENT, -1);
			kill(clients[i].client_pid, SIGUSR1);
		}
	}
}

void to_friends(struct msg message){
	printf("to friends\n");
	char response[MSG_SIZE];
	sprintf(response, "%s %i %s", message.msg_data, message.client_id, get_curr_date());
	for(int i=0;i<MAX_CLIENTS_AMOUNT;i++){
		if(!clients[i].closed && clients[i].is_friend){
			send_msg(clients[i].queue_id, response, MSG_CLIENT, -1);
			kill(clients[i].client_pid, SIGUSR1);
		}
	}
}
//
//void to_one(struct msg message){
//	printf("to one\n");
//	char response[MSG_SIZE];
//	sprintf(response, "%s %i %s", message.msg_data, message.client_id, get_curr_date());
//	if(!clients[message.to_client].closed){
//		send_msg(clients[message.to_client].queue_id, response, MSG_CLIENT, -1);
//		kill(clients[message.to_client].client_pid, SIGUSR1);
//	}
//}

void echo(struct msg message){
	printf("responding to echo\n");
	char response[MSG_SIZE];
	sprintf(response, "%s %s", message.msg_data, get_curr_date());
	send_msg(clients[message.client_id].queue_id, response, MSG_CLIENT, -1);
}

void proceed_msg(struct msg message){

	switch (message.msg_type){
		case STOP:
			printf("konczenie clienta\n");
			clients[atoi(message.msg_data)].closed = 1;
			break;
		case LIST:
			list(message);
			break;
		case FRIENDS:
			friend(message);
			break;
		case TO_ALL:
			to_all(message);
			break;
		case TO_FRIENDS:
			to_friends(message);
			break;
		case TO_ONE:
//			to_one(message);
			break;
		case ECHO:
			echo(message);
			break;
		case INIT:
			register_client(message);
			break;
		default:
			fprintf(stderr, "Wrong message type\n");
	}
}

void signal_handler(){
	struct msg message;
	for(int i=0; i<MAX_CLIENTS_AMOUNT; i++){
		if(!clients[i].closed){
			printf("elo %i\n",i);
			clients[i].closed = 1;
			send_msg(clients[i].queue_id, "", STOP_CLIENT, -1);
			kill(clients[i].client_pid, SIGUSR1);
			msgrcv(server_queue_key, &message, sizeof(message), STOP, 0);
		}
	}
	printf("obsluga sygnalu\n");
	msgctl(server_queue_key, IPC_RMID, NULL);
	exit(0);
}

int main(){
	for(int i =0; i<MAX_CLIENTS_AMOUNT; i++){
		clients[i].closed = 1;
	}

    key_t key = ftok("server", 0);
    server_queue_key = msgget(key, 0666|IPC_CREAT);
    printf("server -> %d\n",server_queue_key);

    signal(SIGINT, signal_handler);

    struct msg message;

    while(1){
    	msgrcv(server_queue_key, &message, sizeof(message), -8, 0);
    	printf("otrzymanie komunikatu\n");
    	proceed_msg(message);
	}
}