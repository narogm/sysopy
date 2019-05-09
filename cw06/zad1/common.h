//
// Created by mateusz on 30.04.19.
//

#ifndef ZAD1_COMMON_H
#define ZAD1_COMMON_H


#include <sys/msg.h>

#define MSG_SIZE  100
#define MAX_CLIENTS_AMOUNT 10

struct msg{
	long msg_type;
	char msg_data[MSG_SIZE];
	int client_id;
//	int to_client;
};

enum to_server_types{
	STOP = 1,
	LIST = 2,
	FRIENDS = 3,
	TO_ALL = 4,
	TO_FRIENDS = 5,
	TO_ONE = 6,
	ECHO = 7,
	INIT = 8
};

enum to_client_types{
	STOP_CLIENT = 1,
	INIT_CLIENT = 2,
	MSG_CLIENT = 3
};


void send_msg(int queue, char* content, int type, int client_id){
	struct msg message;
	message.msg_type = type;
	message.client_id = client_id;
	sprintf(message.msg_data, "%s", content);
	msgsnd(queue, &message, sizeof(message), 0);
}

#endif //ZAD1_COMMON_H
