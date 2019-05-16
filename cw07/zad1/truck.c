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
	if((sem_id = semget(key, 4, 0666|IPC_CREAT)) < 0){
		semctl(sem_id, IPC_RMID, 0);
		fprintf(stderr, "Error while getting semaphores\n");
		exit(1);
	}
	//init
	L->n = 0;
	union semun{
		int val;
		struct semid_ds *buf;
		ushort *array;
	} attr;

	attr.val = K;
	if(semctl(sem_id, 0, SETVAL, attr) == -1){
		fprintf(stderr, "Unable to set value X\n");
	}
	attr.val = M;
	if(semctl(sem_id, 1, SETVAL, attr) == -1){
		fprintf(stderr, "Unable to set value K\n");
	}
	attr.val = X;
	if(semctl(sem_id, 2, SETVAL, attr) == -1){
		fprintf(stderr, "Unable to set value M\n");
	}

}

void stop(){
	semctl(sem_id, IPC_RMID, 0);
	shmctl(shm_id, IPC_RMID, 0);
}

void sig_handler(){
	printf("obsluga SIGINT\n");
	stop();
	exit(0);
}

void load_trucks() {
	struct sembuf shmops[2];
	shmops[0].sem_num = 2;
	shmops[0].sem_op = -1;
	shmops[0].sem_flg = 0;
	shmops[1].sem_num = 3;
	shmops[1].sem_op = -1;
	shmops[1].sem_flg = 0;

	// do notyfikowania na temat ilosci i wagi
	struct sembuf semops [2];
	//size
	semops[0].sem_num = 0;
	semops[0].sem_op = 1;
	semops[0].sem_flg = 0;
	//weigth
	semops[1].sem_num = 1;
	semops[1].sem_flg = 0;
	int truck_load = 0;
//	sleep(10);
	while(1) {
		shmops[0].sem_op = -1;
		shmops[1].sem_op = -1;
		printf("%ld: Checking belt...\n", get_time_in_ms());
		if(semop(sem_id, shmops, 2) == -1) {
			fprintf(stderr, "Unable to access shared memory\n");
		}
		int i = 0;
		while(i < L->n) {
			semops[1].sem_op = L->boxes[i].w;
			if (semop(sem_id, semops, 2) == -1) {
				fprintf(stderr, "Unable to get pack\n");
			}
			struct box b = L->boxes[i];
			L->w -= b.w;
			printf("%ld: Loading to truck pack with weight %d from %d\n", get_time_in_ms(), b.w, b.id);
			i++;
			truck_load++;
			printf("%ld: Loaded on truck: %d/%d\n", get_time_in_ms(), truck_load, X);
			if(truck_load == X) {
				printf("%ld: Truck fully loaded. Swapping...\n", get_time_in_ms());
				sleep(2);
				truck_load = 0;
				printf("%ld: New truck arrived\n", get_time_in_ms());
			}
		}
		L->n = 0;
		shmops[0].sem_op = 1;
		if(semop(sem_id, shmops, 1) == -1) {
			fprintf(stderr, "Unable to release shared memory\n");
		}
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