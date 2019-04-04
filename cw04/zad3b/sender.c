#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "lib.h"

int sigusr1_counter =0;
type global_type = 0;
int amount=0;
int pid;

void sigusr1_handler(){
	if(amount>0){
		amount--;
	}
	else{
		sigusr1_counter++;
	}
	if(amount == 0){
		send_signal_by_type(pid,global_type,SIGUSR2);
		amount = -1;
	}
	send_signal_by_type(pid,global_type,SIGUSR1);
}

void sigusr2_handler(){
	printf("Received %i USR1 signals\n", sigusr1_counter);
	exit(0);

}

int main(int argc, char** argv) {
	if(argc != 4){
		fprintf(stderr, "Incorrect amount of arguments\n");
		exit(1);
	}
	pid = get_pid(argv[1]);
	amount = get_signals_amount(argv[2]);
	global_type = get_type(argv[3]);

	struct sigaction act1, act2;
	sigfillset (&act1.sa_mask);
	sigdelset(&act1.sa_mask, SIGUSR1);
	act1.sa_flags = SA_SIGINFO;
	act1.sa_handler = sigusr1_handler;
	sigaction(SIGUSR1, &act1, NULL);
	sigaction(SIGRTMIN, &act1, NULL);

	sigfillset (&act2.sa_mask);
	sigdelset(&act2.sa_mask, SIGUSR2);
	act2.sa_flags = 0;
	act2.sa_handler = sigusr2_handler;
	sigaction(SIGUSR2, &act2, NULL);
	sigaction(SIGRTMIN + 1, &act2, NULL);
	send_signal_by_type(pid,global_type,SIGUSR1);
	while(1){

	}

	return 0;
}