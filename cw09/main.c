#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <zconf.h>

typedef struct {
	int id, runs_left;
} carriage;

typedef struct {
	int id, on_run;
} passenger;

int carriages_amount, capacity, n;
carriage *carriages_params;
int *curr_passengers_amount_in_carriage;

int next_carriage = 1;
pthread_mutex_t mt_load_carriage = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_load_carriage = PTHREAD_COND_INITIALIZER;

int can_load = 0;
pthread_mutex_t mt_load_passenger = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_load_passenger = PTHREAD_COND_INITIALIZER;

int can_unload = 0;
pthread_mutex_t mt_unload_passenger = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_unload_passenger = PTHREAD_COND_INITIALIZER;

int can_start = 0;
pthread_mutex_t mt_start = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_start = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mt_empty = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_empty = PTHREAD_COND_INITIALIZER;

int extract_int(char *str) {
	char c;
	int i = 0;
	while ((c = str[i++]) != '\0') {
		if (48 > c || c > 57) {
			fprintf(stderr, "Given argument as time is not an integer\n");
			exit(1);
		}
	}
	return atoi(str);
}

long long get_time() {
	struct timeval t;
	gettimeofday(&t, NULL);
	return 1000000 * t.tv_sec + t.tv_usec;
}

void *carriage_fun(void *arg) {
	carriage params = *((carriage *) arg);
	while (params.runs_left > 0) {
		//ladowanie wagonika
		pthread_mutex_lock(&mt_load_carriage);
		while (params.id != next_carriage) {
			pthread_cond_wait(&cond_load_carriage, &mt_load_carriage);
		}

		printf("%lld: Carriage %d is ready to be loaded\n", get_time(), params.id);
		can_load = 1;
		while (curr_passengers_amount_in_carriage[params.id - 1] < capacity) {
			pthread_cond_signal(&cond_load_passenger);
		}
		pthread_mutex_lock(&mt_start);
		if (!can_start)
			pthread_cond_wait(&cond_start, &mt_start);
		pthread_mutex_unlock(&mt_start);
		printf("%lld Carriage %d leave on run %d/%d\n", get_time(), params.id, n - params.runs_left + 1, n);
		next_carriage = next_carriage % carriages_amount + 1;
		can_start = 0;
		pthread_mutex_unlock(&mt_load_carriage);
		pthread_cond_broadcast(&cond_load_carriage);

		//wysiadanie z wagonika
		pthread_mutex_lock(&mt_load_carriage);
		while (params.id != next_carriage) {
			pthread_cond_wait(&cond_load_carriage, &mt_load_carriage);
		}
		params.runs_left--;
		printf("%lld Carriage %d finished run %d/%d\n", get_time(), params.id, n - params.runs_left, n);
		can_unload = 1;
		while (curr_passengers_amount_in_carriage[params.id - 1] > 0) {
			pthread_cond_signal(&cond_unload_passenger);
		}
		if (can_unload) {
			pthread_cond_wait(&cond_empty, &mt_empty);
		}
		printf("%lld Carriage %d is empty\n", get_time(), params.id);
		pthread_mutex_unlock(&mt_load_carriage);
		if (params.runs_left == 0) {
			next_carriage = next_carriage % carriages_amount + 1;
			pthread_cond_broadcast(&cond_load_carriage);
		}
	}
	printf("%lld Carriage %d finished all runs\n", get_time(), params.id);
	pthread_exit(NULL);
}

void *passenger_fun(void *arg) {
	passenger params = *((passenger *) arg);
	while (1) {
		pthread_mutex_lock(&mt_load_passenger);
		while (!can_load) {
			pthread_cond_wait(&cond_load_passenger, &mt_load_passenger);
		}
		int carriage_id = next_carriage;
		curr_passengers_amount_in_carriage[next_carriage - 1]++;
		printf("%lld Passenger %d entered carriage %d as %d/%d\n", get_time(), params.id, next_carriage,
			   curr_passengers_amount_in_carriage[next_carriage - 1], capacity);
		if (curr_passengers_amount_in_carriage[next_carriage - 1] == capacity) {
			printf("%lld Passenger %d pressed START button\n", get_time(), params.id);
			can_load = 0;
			can_start = 1;
			pthread_cond_signal(&cond_start);
		}
		pthread_mutex_unlock(&mt_load_passenger);
		pthread_mutex_lock(&mt_unload_passenger);
		while (carriage_id != next_carriage || !can_unload) {
			pthread_cond_wait(&cond_unload_passenger, &mt_unload_passenger);
		}
		printf("%lld Passenger %d left carriage %d ### passengers left: %d/%d\n", get_time(), params.id, carriage_id,
			   curr_passengers_amount_in_carriage[carriage_id - 1], capacity);
		curr_passengers_amount_in_carriage[carriage_id - 1]--;
		if (curr_passengers_amount_in_carriage[carriage_id - 1] == 0) {
			can_unload = 0;
			pthread_cond_signal(&cond_empty);
		}
		pthread_mutex_unlock(&mt_unload_passenger);
	}
}

int main(int argc, char **argv) {
	if (argc != 5) {
		fprintf(stderr, "Incorrect amount of arguments\n");
		exit(1);
	}
	int passengers_amount = extract_int(argv[1]);
	carriages_amount = extract_int(argv[2]);
	capacity = extract_int(argv[3]);
	n = extract_int(argv[4]);
	if (passengers_amount < capacity) {
		fprintf(stderr, "Passengers amount should be greater than carriage capacity\n");
		exit(1);
	}

	pthread_t thr_carriages[carriages_amount];
	carriages_params = malloc(sizeof(carriage) * carriages_amount);
	pthread_t thr_passenger[passengers_amount];
	passenger passengers[passengers_amount];

	curr_passengers_amount_in_carriage = calloc(capacity, sizeof(int));

	for (int i = 0; i < carriages_amount; i++) {
		carriages_params[i].id = i + 1;
		carriages_params[i].runs_left = n;
		pthread_create(&thr_carriages[i], NULL, carriage_fun, &carriages_params[i]);

	}
	for (int i = 0; i < passengers_amount; i++) {
		passengers[i].id = i + 1;
		passengers[i].on_run = 0;
		pthread_create(&thr_passenger[i], NULL, passenger_fun, &passengers[i]);
	}

	for (int i = 0; i < carriages_amount; i++) {
		pthread_join(thr_carriages[i], NULL);
	}
	for (int i = 0; i < passengers_amount; i++) {
		pthread_cancel(thr_passenger[i]);
		printf("%lld Passenger %d finished\n", get_time(), i + 1);
	}

	free(carriages_params);
	free(curr_passengers_amount_in_carriage);
	return 0;
}