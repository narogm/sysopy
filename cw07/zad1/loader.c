//
// Created by mateusz on 14.05.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <zconf.h>
#include "common.h"

int shm_id, sem_id;
int N;
int C = -1;
struct line *L;

void initialize() {
	key_t key = ftok("key_shm", 63);
	if ((shm_id = shmget(key, sizeof(L), 0666)) < 0) {
		shmctl(shm_id, IPC_RMID, 0);
		fprintf(stderr, "Error while getting shared memory\n");
		exit(1);
	}
	if((L = shmat(shm_id, 0, 0)) == (void*)-1){
		fprintf(stderr, "Error while getting shared memory\n");
		exit(1);
	}

	key = ftok("key_sem", 63);
	if ((sem_id = semget(key, 3, 0666)) < 0) {
		semctl(sem_id, IPC_RMID, 0);
		fprintf(stderr, "Error while getting semaphores\n");
		exit(1);
	}
}

void load_packs() {
	struct sembuf shm_line[1];
	shm_line[0].sem_num = 2;
	shm_line[0].sem_op = -1;
	shm_line[0].sem_flg = 0;

	struct sembuf sem_access[2];
	sem_access[0].sem_num = 0;
	sem_access[0].sem_op = -1;
	sem_access[0].sem_flg = 0;
	sem_access[1].sem_num = 1;
	sem_access[1].sem_op = -N;
	sem_access[1].sem_flg = 0;

	while (C--) {
		if (semop(sem_id, sem_access, 2) == -1) {
			fprintf(stderr, "Error with semaphores\n");
		}
		shm_line[0].sem_op = -1;
		if (semop(sem_id, shm_line, 1) == -1) {
			fprintf(stderr, "Error with shared memory\n");
		}
		struct box b;
		b.loader_id = getpid();
		b.weight = N;
		L->boxes[L->amount++] = b;
		L->weight += N;
		printf("%ld: %d loaded pack with weight %d\n", get_time_in_ms(), getpid(), N);
		printf("%ld: Line state - packs' amount: %d, packs' weight: %d\n", get_time_in_ms(), L->amount, L->weight);

		shm_line[0].sem_op = 1;
		if (semop(sem_id, shm_line, 1) == -1) {
			fprintf(stderr, "Error with shared memory\n");
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