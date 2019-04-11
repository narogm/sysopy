#include <stdio.h>
#include <stdlib.h>

int lines_amount=0;
const int line_size=200;

int get_number_of_lines(char* path){
	FILE* file;
	if((file = fopen(path,"r")) == NULL){
		fprintf(stderr, "Unable to open file\n");
		exit(1);
	}
	int amount=0;
	size_t size =0;
	char* record;
	while(getline(&record,&size,file) != -1){
		amount++;
	}
	fclose(file);
	return amount;
}

char** get_file_context(char* path){
	lines_amount = get_number_of_lines(path);
	char** programs = malloc(sizeof(char*)*lines_amount);
	FILE * file;
	if((file = fopen(path,"r")) == NULL){
		fprintf(stderr, "Unable to open file\n");
		exit(1);
	}
	size_t size = 0;
	for(int i=0;i<lines_amount;i++){
		programs[i] = malloc(sizeof(char) * line_size);
		getline(&programs[i],&size,file);
	}
	fclose(file);
	return programs;
}

int main(int argc, char** argv) {
	if(argc != 2){
		fprintf(stderr, "Incorrect amount of arguments\n");
		exit(1);
	}
	char* path = argv[1];
	char** programs = get_file_context(path);
//	for(int i=0;i<lines_amount;i++){
//		printf("%s\n",programs[i]);
//	}

	return 0;
}