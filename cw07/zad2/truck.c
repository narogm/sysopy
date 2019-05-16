//
// Created by mateusz on 16.05.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <zconf.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common.h"

int shm_id;
sem_t *sem_line, *sem_line_counter;
struct line *L;
int X, K, M;

void initialize(){
	if((shm_id = shm_open("/shm_line", O_RDWR | O_CREAT, 0666)) < 0){
		fprintf(stderr, "Error while getting shared memory\n");
		exit(1);
	}
	if(ftruncate(shm_id, sizeof(struct line)) == -1){
		fprintf(stderr, "Error while truncate\n");
		exit(1);
	}
	if((L = mmap(NULL, sizeof(struct line),  PROT_WRITE | PROT_READ, MAP_SHARED, shm_id, 0)) == MAP_FAILED){
		fprintf(stderr, "Error while accessing shared memory\n");
		exit(1);
	}

	if((sem_line = sem_open("/sem_line", O_RDWR | O_CREAT, 0666, 1)) < 0){
		fprintf(stderr, "Error while getting semaphore\n");
		exit(1);
	}
	if((sem_line_counter = sem_open("/sem_line_counter", O_RDWR | O_CREAT, 0666, K)) < 0){
		fprintf(stderr, "Error while getting semaphore\n");
		exit(1);
	}
	L->amount = 0;
	L->max_weight = M;
}

void stop(){
	shm_unlink("/shm_line");
	sem_unlink("/sem_line");
	sem_unlink("/sem_line_counter");
}

void sig_handler(){
	stop();
	exit(0);
}

void load_trucks() {
	int packs_on_truck = 0;
	while(1) {
		printf("%ld: Checking line...\n", get_time_in_ms());
		if(sem_wait(sem_line) == -1) {
			fprintf(stderr, "Error with shared memory\n");
		}
		int i = 0;
		while(i < L->amount) {
			if (sem_post(sem_line_counter) == -1) {
				fprintf(stderr, "Unable to get pack\n");
			}
			struct box b = L->boxes[i];
			L->weight -= b.weight;
			printf("%ld: Loading to truck pack from loader: %d\n", get_time_in_ms(), b.loader_id);
			packs_on_truck++;
			printf("%ld: Loaded on truck: %d/%d\n", get_time_in_ms(), packs_on_truck, X);
			if(packs_on_truck == X) {
				printf("%ld: Truck fully loaded. Swapping...\n", get_time_in_ms());
				sleep(1);
				packs_on_truck = 0;
				printf("%ld: New truck arrived\n", get_time_in_ms());

			}
			i++;
		}
		L->amount = 0;
		if(sem_post(sem_line) == -1) {
			fprintf(stderr, "Error with shared memory\n");
		}
		sleep(1);
	}
}

int main(int argc, char** argv){
	if(argc != 4){
		fprintf(stderr, "Incorrect amount of arguments\n");
		exit(1);
	}
	X = extract_int(argv[1]);
	K = extract_int(argv[2]);
	M = extract_int(argv[3]);

	signal(SIGINT, sig_handler);

	initialize();
	load_trucks();
	return 0;
}