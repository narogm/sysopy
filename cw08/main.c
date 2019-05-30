#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

typedef struct image{
	int ** pixels;
	int w, h, m;
}image;

typedef struct filter{
	float** pixels;
	int c;
}filter;

image *input, *output;
filter* fil;
int amount, type;

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

long long get_time(){
	struct timeval t;
	gettimeofday(&t, NULL);
	return 1000000 * t.tv_sec + t.tv_usec;
}

image* read_img(char* file_name){
	FILE *file = fopen(file_name, "r");
	if(file == NULL){
		fprintf(stderr, "Unable to open file with image\n");
		exit(1);
	}
	char header[2];
	fscanf(file, "%s", header);
	if(strcmp(header,"P2") != 0){
		fprintf(stderr, "Wrong format of input file\n");
		exit(1);
	}
	image* result = malloc(sizeof(image));
	fscanf(file, "%d", &result->w);
	fscanf(file, "%d", &result->h);
	fscanf(file, "%d", &result->m);

	result->pixels = malloc(sizeof(int*) * result->h);
	for(int i=0; i<result->h; i++){
		result->pixels[i] = malloc(sizeof(int) * result->w);
		for(int j=0; j<result->w; j++){
			int val;
			fscanf(file, "%d", &val);
			result->pixels[i][j] = val;
		}
	}
	fclose(file);
	return result;
}

filter* read_filter(char* file_name){
	FILE* file = fopen(file_name, "r");
	if(file == NULL){
		fprintf(stderr, "Unable to open filter file\n");
		exit(1);
	}

	filter* result = malloc(sizeof(filter));
	fscanf(file, "%i", &result->c);
	result->pixels = malloc(sizeof(float) * result->c);
	for(int i=0; i<result->c; i++){
		result->pixels[i] = malloc(sizeof(float) * result->c);
		for(int j=0; j<result->c; j++){
			float value;
			fscanf(file, "%f", &value);
			result->pixels[i][j] = value;
		}
	}
	fclose(file);
	return result;
}

int max(int x, int y){
	return x>y ? x : y;
}

void proceed(int x, int y){
	float sum = 0;
	for(int i=0; i<fil->c; i++){
		for(int j=0; j<fil->c; j++){
			int index_i = max(0, x-ceil(fil->c/2)+i-1);
			int index_j = max(0, y-ceil(fil->c/2)+j-1);
			if(index_i<input->h && index_j<input->w)
				sum += input->pixels[index_i][index_j] * fil->pixels[i][j];
		}
	}
	output->pixels[x][y] = round(sum);
}

void *apply_filter(void *id){
	long long start_time = get_time();
	int k = *((int*) id);
	int m = amount;
	if(type == 0){
		for(int j = (k-1)*input->w/m; j<k*input->w/m; j++){
			for(int i=0; i<input->h; i++)
				proceed(i, j);
		}
	}
	else{
		for(int j=k-1; j<input->w; j+=m){
			for(int i=0; i<input->h; i++)
				proceed(i, j);
		}
	}
	return (void *) (get_time() - start_time);
}

void save_to_file(char* file_name){
	FILE* file = fopen(file_name, "w");
	if(file == NULL){
		fprintf(stderr, "Unable to open file to save result\n");
		exit(1);
	}
	fprintf(file, "P2\n%d %d\n%d\n", output->w, output->h, output->m);
	for(int i=0; i<output->h; i++){
		for(int j=0; j<output->w; j++){
			fprintf(file, "%d ", output->pixels[i][j]);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

int main(int argc, char** argv) {
	if(argc != 6){
		fprintf(stderr, "Incorrect amount of arguments\n");
		exit(1);
	}
	amount = extract_int(argv[1]);
	char* in_file = argv[3];
	char* filter_file = argv[4];
	char* out_file = argv[5];
	if(strcmp(argv[2], "block") == 0){
		type = 0;
	}
	else if(strcmp(argv[2], "interleaved") == 0){
		type = 1;
	}
	else{
		fprintf(stderr, "Wrong type\n");
		exit(1);
	}

	input = read_img(in_file);
	fil = read_filter(filter_file);

	output = malloc(sizeof(image));
	output->w = input->w;
	output->h = input->h;
	output->m = input->m;
	output->pixels = calloc(sizeof(float*), output->h);
	for(int i=0; i<output->h; i++){
		output->pixels[i] = calloc(sizeof(float), output->w);
	}


	long long start_time = get_time();
	pthread_t threads[amount];
	int threads_id[amount];
	for(int i=0; i<amount; i++){
		threads_id[i] = i+1;
		pthread_create(&threads[i], NULL, (void *(*)(void *)) apply_filter, (void *) &threads_id[i]);
	}

	for(int i=0; i<amount; i++){
		long long result;
		pthread_join(threads[i], (void*) &result);
		printf("Thread %i finished after %lld us\n", i+1, result);
	}
	printf("Time of whole process: %lld us\n", get_time()-start_time);
	save_to_file(out_file);

	for(int i=0; i<input->h; i++){
		free(input->pixels[i]);
	}
	free(input->pixels);
	free(input);
	for(int i=0; i<output->h; i++){
		free(output->pixels[i]);
	}
	free(output->pixels);
	free(output);
//	for(int i=0; i<fil->c; i++){
//		free(fil->pixels[i]);
//	}
	free(fil->pixels);
	free(fil);
	return 0;
}