#ifndef ZAD2_COMMAND_MANAGER_H
#define ZAD2_COMMAND_MANAGER_H

#include <stdlib.h>

typedef enum {
	RUNNING =0, STOPPED =1, ENDED =2
} child_status;

typedef struct {
	char *file_name;
	pid_t pid;
	child_status status;
} child;

void list_processes(child** children);
void start_process(child *child);
void start_all_processes(child **children);
void stop_process(child *child);
void stop_all_processes(child **children);
void end_processes(child **children);

child* create_child(const char *file_name, pid_t pid);
child* get_child_by_pid(child **children, int pid);

#endif //ZAD2_COMMAND_MANAGER_H
