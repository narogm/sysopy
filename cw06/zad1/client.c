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
    client_queue = msgget(key, 0666|IPC_CREAT);

    key_t server_key = ftok("server", 0);
    server_id = msgget(server_key, 0);

    struct msg message;
    message.msg_type=INIT;
    message.client_id = getpid();
//    message.to_client = -1;
    sprintf(message.msg_data, "%i", key);

    msgsnd(server_id, &message, sizeof(message), 0);

	msgrcv(client_queue, &message, sizeof(message), 0, 0);
	client_id = atoi(message.msg_data);

}

void stop(){
	printf("closing client");
	char content[MSG_SIZE];
	sprintf(content, "%i", client_id);
	send_msg(server_id, content, STOP, client_id);
	msgctl(client_queue, IPC_RMID, NULL);
	exit(0);
}

void list(){
	send_msg(server_id, "", LIST, client_id);
	struct msg message;
	msgrcv(client_queue, &message, sizeof(message), MSG_CLIENT, 0);
	printf("Received from server: %s\n", message.msg_data);
}

void friends(char* list){
	send_msg(server_id, list, FRIENDS, client_id);
	struct msg message;
	msgrcv(client_queue, &message, sizeof(message), MSG_CLIENT, 0);
}

void to_all(char* content){
	send_msg(server_id, content, TO_ALL, client_id);
}

void to_friends(char* content){
	send_msg(server_id, content, TO_FRIENDS, client_id);
}
//
//void to_one(int id, char* content){
////	send_msg(server_id, content, TO_FRIENDS, client_id);
//	struct msg message;
//	message.msg_type = TO_ONE;
//	message.client_id = client_id;
//	message.to_client = id;
//	sprintf(message.msg_data, "%s", content);
//	msgsnd(server_id, &message, sizeof(message), 0);
//
//}

void echo(char* content){
	send_msg(server_id, content, ECHO, client_id);
	struct msg message;
	msgrcv(client_queue, &message, sizeof(message), MSG_CLIENT, 0);
	printf("Received from server: %s\n", message.msg_data);
}

void signal_handler(){
	stop();
}

void sigusr_handler(){
	struct msg message;
	msgrcv(client_queue, &message, sizeof(message), 0, 0);
	if(message.msg_type == STOP_CLIENT){
		stop();
	}
	else{
		printf("Received message from server: %s\n", message.msg_data);

	}
}

int is_integer_list(char* line){
	int i = 0;
	char c;
	while((c = line[i++]) != '\0'){
		if((c>57 || 48>c) && c != 32 && c != '\n'){	// && c != '\n'
			fprintf(stderr, "Invalid integer list\n");
			printf("error |%c|\n", c);
			return 0;
		}
	}
	return 1;
}

int main(){
	initialize();
	signal(SIGINT, signal_handler);
	signal(SIGUSR1, sigusr_handler);

	size_t tmp_size;
	char* line = malloc(sizeof(char) * MSG_SIZE);
	char command[MSG_SIZE];
	while(1){
		scanf("%s",command);
		if(strcmp(command, "STOP") == 0){
			stop();
			exit(0);
		}
		else if(strcmp(command, "LIST") == 0){
			list();
		}
		else if(strcmp(command, "FRIENDS") == 0){
			getline(&line, &tmp_size, stdin);
			if(is_integer_list(line))
				friends(line);
		}
		else if(strcmp(command, "2ALL") == 0){
			scanf("%s", command);
			to_all(command);
		}
		else if(strcmp(command, "2FRIENDS") == 0){
			scanf("%s", command);
			to_friends(command);
		}
		else if(strcmp(command, "2ONE") == 0){
			int id;
			scanf("%i", id);
			scanf("%s", command);
//			to_one(id, command);
		}
		else if(strcmp(command, "ECHO") == 0){
			scanf("%s", command);
			echo(command);
		}
		else{
			fprintf(stderr, "Unknown command\n");
		}
	}
}