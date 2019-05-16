//
// Created by mateusz on 16.05.19.
//

#ifndef ZAD2_COMMON_H
#define ZAD2_COMMON_H

#include <sys/time.h>
#define MAX_SIZE 512

struct box{
	int weight;
	int loader_id;
};

struct line{
	int weight, amount;
	struct box boxes[MAX_SIZE];
	int max_weight;
};

int extract_int(char *str) {
	char c;
	int i=0;
	while((c = str[i++]) != '\0'){
		if(48 > c || c > 57){
			fprintf(stderr, "Given argument as time is not an integer\n");
			exit(1);
		}
	}
	return atoi(str);
}

unsigned long get_time_in_ms() {
	struct timeval tv;
	gettimeofday(&tv,NULL);
	unsigned long time_in_ms = 1000000 * tv.tv_sec + tv.tv_usec;
	return time_in_ms;
}

#endif //ZAD2_COMMON_H
