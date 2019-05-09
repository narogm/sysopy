//
// Created by mateusz on 30.04.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"

int client_id = -1;
int client_queue = -1;
int server_id = -1;

void initialize(){
    key_t key = ftok("client", getpid());
    printf("client key %i\n", key);
    client_queue = msgget(key, 0666|IPC_CREAT);

    key_t server_key = ftok("server", 0);
    server_id = msgget(server_key, 0);

    struct msg message;
    message.msg_type=INIT;
    sprintf(message.msg_data, "%i", key);

    printf("wyslanie wiadomosci  -> %d\n",server_id);
    msgsnd(server_id, &message, sizeof(message), 0);

	msgrcv(client_queue, &message, sizeof(message), 0, 0);
	client_id = atoi(message.msg_data);

}

void signal_handler(){
	printf("obsluga sygnalu\n");
	char content[MSG_SIZE];
	sprintf(content, "%i", client_id);
	printf("client id %i\n",client_id);
	send_msg(server_id, content, STOP);
	msgctl(client_queue, IPC_RMID, NULL);
	exit(0);
}

int main(){
	initialize();
	signal(SIGINT, signal_handler);
	struct msg message;
	msgrcv(client_queue, &message, sizeof(message), 0, 0);
	printf("odebrano wiadomosc\n");
//	client_id = atoi(message.msg_data);
	msgctl(client_queue, IPC_RMID, NULL);
	exit(0);
	char command[MSG_SIZE];
	while(1){
		;
//		scanf("%s",command);
//		if(strcmp(command, "ECHO"))
	}
}