//
// Created by mateusz on 30.04.19.
//

#ifndef ZAD1_COMMON_H
#define ZAD1_COMMON_H


#include <sys/msg.h>

#define MSG_SIZE = 20
#define MAX_CLIENTS_AMOUNT = 5

typedef struct msg{
	long msg_type;
	char msg_data[MSG_SIZE];
};

enum msg_types{
	STOP = 1,
	LIST = 2,
	FRIENDS = 3,
	TO_ALL = 4,
	TO_FRIENDS = 5,
	TO_ONE = 6,
	ECHO = 7,
	INIT = 8
};

#endif //ZAD1_COMMON_H
