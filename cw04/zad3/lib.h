#ifndef ZAD3_LIB_H
#define ZAD3_LIB_H


typedef enum{
	KILL=0,SIGQUEUE=1,SIGRT=2
} type;

int is_int(char* str);

int get_pid(char* str);

int get_signals_amount(char* str);

int get_type(char* str);

void send_signals_by_type(int pid, type type, int amount);

#endif //ZAD3_LIB_H
