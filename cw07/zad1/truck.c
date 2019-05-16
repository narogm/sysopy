#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <zconf.h>
#include "common.h"

int shm_id, sem_id;
struct line *L;
int X, K, M;

void initialize(){
	key_t key = ftok("key_shm", 63);
	if((shm_id = shmget(key, sizeof(L), 0666|IPC_CREAT)) < 0){
		shmctl(shm_id, IPC_RMID, 0);
		fprintf(stderr, "Error while getting shared memory\n");
		exit(1);
	}
	if((L = shmat(shm_id, 0, 0)) == (void*)-1){
		fprintf(stderr, "Error while accessing shared memory\n");
		exit(1);
	}

	key = ftok("key_sem", 63);
	if((sem_id = semget(key, 3, 0666|IPC_CREAT)) < 0){
		semctl(sem_id, IPC_RMID, 0);
		fprintf(stderr, "Error while getting semaphores\n");
		exit(1);
	}
	L->amount = 0;
	union semun{
		int val;
		struct semid_ds *buf;
		ushort *array;
	} attr;

	attr.val = K;
	if(semctl(sem_id, 0, SETVAL, attr) == -1){
		fprintf(stderr, "Unable to set value K\n");
	}
	attr.val = M;
	if(semctl(sem_id, 1, SETVAL, attr) == -1){
		fprintf(stderr, "Unable to set value M\n");
	}
	attr.val = 1;
	if(semctl(sem_id, 2, SETVAL, attr) == -1){
		fprintf(stderr, "Unable to set value\n");
	}

}

void sig_handler(){
	printf("\nobsluga SIGINT\n");
	semctl(sem_id, IPC_RMID, 0);
	shmctl(shm_id, IPC_RMID, 0);
	exit(0);
}

void load_trucks() {
	//sem line
	struct sembuf sem_line[1];
	sem_line[0].sem_num = 2;
	sem_line[0].sem_op = -1;
	sem_line[0].sem_flg = 0;

	struct sembuf sem_access [2];
	//size
	sem_access[0].sem_num = 0;
	sem_access[0].sem_op = 1;
	sem_access[0].sem_flg = 0;
	//weigth
	sem_access[1].sem_num = 1;
	sem_access[1].sem_flg = 0;
	int packs_on_truck = 0;
	while(1) {
		sem_line[0].sem_op = -1;
		printf("%ld: Checking belt...\n", get_time_in_ms());
		if(semop(sem_id, sem_line, 1) == -1) {
			fprintf(stderr, "Error with shared memory\n");
		}
		int i = 0;
		while(i < L->amount) {
			sem_access[1].sem_op = L->boxes[i].weight;
			if (semop(sem_id, sem_access, 2) == -1) {
				fprintf(stderr, "Unable to get pack\n");
			}
			struct box b = L->boxes[i];
			L->weight -= b.weight;
			printf("%ld: Loading to truck pack from loader: %d\n", get_time_in_ms(), b.loader_id);
			packs_on_truck++;
			printf("%ld: Loaded on truck: %d/%d\n", get_time_in_ms(), packs_on_truck, X);
			if(packs_on_truck == X) {
				printf("%ld: Truck fully loaded. Swapping...\n", get_time_in_ms());
				sleep(2);
				packs_on_truck = 0;
				printf("%ld: New truck arrived\n", get_time_in_ms());
			}
			i++;
		}
		L->amount = 0;
		sem_line[0].sem_op = 1;
		if(semop(sem_id, sem_line, 1) == -1) {
			fprintf(stderr, "Unable to release shared memory\n");
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