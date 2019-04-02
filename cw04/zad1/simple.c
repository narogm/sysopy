#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <zconf.h>
#include <time.h>

int paused = 0;

void sigint_handler(int signal){
	printf("\nOtrzymano sygnał CTRL+C\nZakończenie programu\n");
	exit(0);
}

void sigtstp_handler(int signal){
	if(!paused){
		printf("\nOczekuję na \nCTRL+Z - kontynuacja\nCTRL+C - zakończenie programu\n");
		paused = 1;
	} else{
		printf("\nKontunuacja\n");
		paused = 0;
	}
}

void show_date(){
	time_t curr_time;
	struct tm * timeinfo;

	time ( &curr_time );
	timeinfo = localtime ( &curr_time );
	printf ( "Aktualna data: %s", asctime (timeinfo) );
}

int main() {
	signal(SIGINT, sigint_handler);
	struct sigaction sigact;
	sigact.sa_handler = sigtstp_handler;
	sigaction(SIGTSTP, &sigact, NULL);
	while(1){
		if(paused){
			pause();
		} else{
			show_date();
		}
		sleep(1);
	}
	return 0;
}