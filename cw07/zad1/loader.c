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
	if ((sem_id = semget(key, 4, 0666)) < 0) {
		semctl(sem_id, IPC_RMID, 0);
		fprintf(stderr, "Error while getting semaphores\n");
		exit(1);
	}
}

void load_packs() {
	int c = C;
	struct sembuf semops[2];
	semops[0].sem_num = 0;
	semops[0].sem_op = -1;
	semops[0].sem_flg = 0;
	semops[1].sem_num = 1;
	semops[1].sem_op = -N;
	semops[1].sem_flg = 0;
	struct sembuf shmsem[2];
	shmsem[0].sem_num = 2;
	shmsem[0].sem_op = -1;
	shmsem[0].sem_flg = 0;
	shmsem[1].sem_num = 3;
	shmsem[1].sem_op = 1;
	shmsem[1].sem_flg = 0;
	while (c--) {
		printf("%ld: Waiting for loading pack by %d\n", get_time_in_ms(), getpid());
		if (semop(sem_id, semops, 2) == -1) {
			fprintf(stderr, "Unable to perform action on semaphores\n");
		}
		shmsem[0].sem_op = -1;
		if (semop(sem_id, shmsem, 2) == -1) {
			fprintf(stderr, "Unable to access shared memory\n");
		}
		struct box b;
		b.w = N;
		b.id = getpid();
		printf("%i\n", L->n);
		L->boxes[L->n++] = b;
		L->w += N;
		printf("%ld: %d loaded pack %d with weight %d\n", get_time_in_ms(), getpid(), L->n - 1, N);
		printf("%ld: Line state - packs' amount: %d, packs' weight: %d\n", get_time_in_ms(), L->n, L->w);
		shmsem[0].sem_op = 1;
		if (semop(sem_id, shmsem, 1) == -1) {
			fprintf(stderr, "Unable to release shared memory\n");
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