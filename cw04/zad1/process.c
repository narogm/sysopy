#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <zconf.h>
#include <time.h>
#include <string.h>

int active_process = 0;
int child_pid = -1;

void make_new_process() {
	int pid;
	if ((pid = fork()) == 0) {
		execl("./display_date.sh", "./display_date.sh", NULL);
		exit(1);
	} else{
		child_pid = pid;
	}
}

void stop_process(){
	if(active_process && child_pid != -1){
		kill(child_pid, SIGKILL);
		active_process = 0;
		child_pid = -1;
	}
}

void sigint_handler(int signal) {
	stop_process();
	printf("\nOtrzymano sygnał CTRL+C\nZakończenie programu\n");
	exit(0);
}

void sigtstp_handler(int signal) {
	if (!active_process) {
		stop_process();
		printf("\nOczekuję na \nCTRL+Z - kontynuacja\nCTRL+C - zakończenie programu\n");
		active_process = 1;
	} else {
		printf("\nKontunuacja\n");
		make_new_process();
		active_process = 0;
	}
}

void show_date() {
	time_t curr_time;
	struct tm *timeinfo;

	time(&curr_time);
	timeinfo = localtime(&curr_time);
	printf("Aktualna data: %s", asctime(timeinfo));
}

int main(int argc, char **argv) {
	signal(SIGINT, sigint_handler);
	struct sigaction sigact;
	sigact.sa_handler = sigtstp_handler;
	sigaction(SIGTSTP, &sigact, NULL);
	make_new_process();
	while (1) {
		pause();
	}

	return 0;
}