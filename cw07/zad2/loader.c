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
int N;
int C = -1;
struct line *L;

void initialize() {
	if((shm_id = shm_open("/shm_line", O_RDWR, 0666)) < 0){
		fprintf(stderr, "Error while getting shared memory\n");
		exit(1);
	}
	if((L = mmap(NULL, sizeof(struct line),  PROT_WRITE | PROT_READ, MAP_SHARED, shm_id, 0)) == MAP_FAILED){
		fprintf(stderr, "Error while accessing shared memory\n");
		exit(1);
	}

	if((sem_line = sem_open("/sem_line", O_RDWR, 0666, 1)) < 0){
		fprintf(stderr, "Error while getting semaphores\n");
		exit(1);
	}
	if((sem_line_counter = sem_open("/sem_line_counter", O_RDWR, 0666)) < 0){
		fprintf(stderr, "Error while getting semaphores\n");
		exit(1);
	}
}

void load_packs() {
	int c = C;
	while(c--){
		if(sem_wait(sem_line_counter) == -1) {
			fprintf(stderr, "Error with semaphores\n");
		}
		if(sem_wait(sem_line) == -1) {
			fprintf(stderr, "Error with shared memory\n");
		}
		if(L->weight + N > L->max_weight) {
			if(sem_post(sem_line_counter) == -1) {
				fprintf(stderr, "Error with semaphore counter\n");
			}
			sleep(1);
		} else {
			struct box b;
			b.loader_id = getpid();
			b.weight = N;
			L->boxes[L->amount++] = b;
			L->weight += N;
			printf("%ld: %d loaded pack with weight %d\n", get_time_in_ms(), getpid(), N);
			printf("%ld: Line state - packs' amount: %d, packs' weight: %d\n", get_time_in_ms(), L->amount, L->weight);
		}
		if(sem_post(sem_line) == -1) {
			fprintf(stderr, "Error shared memory\n");
		}
	}
}

int main(int argc, char **argv) {
	if (argc != 2 && argc != 3) {
		fprintf(stderr, "Incorrect amount of arguments\n");
		exit(1);
	}
	N = extract_int(argv[1]);
	if (argc == 3) {
		C = extract_int(argv[2]);
	}

	initialize();
	load_packs();
}