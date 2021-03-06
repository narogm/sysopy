#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <signal.h>
#include "lib.h"

int sigusr1_counter = 0;
pid_t sender_pid = -1;
type global_type = 0;
int sigusr2_received = 0;

void sigusr1_handler(int signal, siginfo_t *info, void *extra){
	sender_pid = info->si_pid;
	if(!sigusr2_received){
		sigusr1_counter++;
	} else{
		sigusr1_counter--;
	}

	if(sigusr1_counter > 0){
		send_signal_by_type(sender_pid, global_type, SIGUSR1);
	} else{
		send_signal_by_type(sender_pid, global_type, SIGUSR2);
		exit(0);
	}
}

void sigusr2_handler(int signal){
	printf("Received %i USR1 signals\n", sigusr1_counter);
	if(sender_pid == -1){
		fprintf(stderr, "Error with sender's pid\n");
		exit(1);
	}
	send_signal_by_type(sender_pid, global_type, SIGUSR1);
	sigusr2_received = 1;
}

int main(int argc, char** argv){
	if(argc != 2){
		fprintf(stderr, "Incorrect amount of arguments\n");
		exit(1);
	}
	global_type = get_type(argv[1]);

	printf("Catcher PID: %i\n",getpid());

	sigset_t oldmask, blockmask;
	sigemptyset(&oldmask);
	sigemptyset(&blockmask);
	sigfillset(&blockmask);

	sigdelset(&blockmask,SIGUSR1);
	sigdelset(&blockmask,SIGUSR2);
	sigdelset(&blockmask,SIGRTMIN);
	sigdelset(&blockmask,SIGRTMIN+1);
	if (sigprocmask(SIG_BLOCK, &blockmask, &oldmask) == -1)
	{
		fprintf(stderr, "cannot set mask \n");
		exit(1);
	}


	struct sigaction act1,act2;

	sigfillset (&act1.sa_mask);
	sigdelset(&act1.sa_mask, SIGUSR1);
	act1.sa_flags = SA_SIGINFO;
	act1.sa_handler = (__sighandler_t) sigusr1_handler;
	sigaction(SIGUSR1, &act1, NULL);
	sigaction(SIGRTMIN, &act1, NULL);

	sigfillset (&act2.sa_mask);
	sigdelset(&act2.sa_mask, SIGUSR2);
	act2.sa_flags = 0;
	act2.sa_handler = sigusr2_handler;
	sigaction(SIGUSR2, &act2, NULL);
	sigaction(SIGRTMIN + 1, &act2, NULL);
	while(1){

	}
	return 0;
}