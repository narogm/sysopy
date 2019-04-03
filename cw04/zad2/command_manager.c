#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "command_manager.h"

void list_processes(child** children){
	int i=0;
	printf("#####\nPROCESSES LIST:\n-----\n");
	while(children[i] != NULL){
		printf("PID %i -- FILE %s -- STATUS ",children[i]->pid, children[i]->file_name);
		switch(children[i]->status){
			case 0: printf("running\n"); break;
			case 1: printf("stopped\n"); break;
			case 2: printf("ended\n"); break;
		}
		i++;
	}
	printf("#####\n");
}

void start_process(child *child){
	if(child != NULL && child->status == STOPPED){
		if(kill(child->pid, 0) == 0){
			kill(child->pid, SIGCONT);
			child->status = RUNNING;
		}
	}
}

void start_all_processes(child **children){
	int i =0;
	while(children[i] != NULL){
		start_process(children[i]);
		i++;
	}
}

void stop_process(child *child){
	if(child != NULL && child->status == RUNNING){
		if(kill(child->pid, 0) == 0){
			kill(child->pid, SIGTSTP);
			child->status = STOPPED;
		}
	}
}

void stop_all_processes(child **children){
	int i=0;
	while (children[i] != NULL){
		stop_process(children[i]);
		i++;
	}
}

void end_processes(child **children){
	int i=0;
	while(children[i] != NULL){
		kill(children[i]->pid,SIGINT);
		children[i]->status = ENDED;
		i++;
	}
}

child* create_child(const char *file_name, pid_t pid){
	child* child = malloc(sizeof(child));
	child->file_name = strdup(file_name);
	child->pid = pid;
	child->status = RUNNING;
	return child;
}

child* get_child_by_pid(child **children, int pid){
	int i=0;
	while(children[i] != NULL){
		if(children[i]->pid == pid)
			return children[i];
		i++;
	}
	return NULL;
}