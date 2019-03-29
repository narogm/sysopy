#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <zconf.h>
#include <string.h>

int extract_int(char *str) {
    char c;
    int i=0;
    while((c = str[i++]) != '\0'){
        if(48 > c || c > 57){
            fprintf(stderr, "Given argument is not an integer\n");
            exit(1);
        }
    }
    return atoi(str);
}

char* get_current_date(){
    char* date = malloc(sizeof(char) * 20);
    struct tm *tmTime;
    time_t t;
    t = time(NULL);
    tmTime = localtime(&t);
    strftime(date, 20, "%Y-%m-%d_%H-%M-%S", tmTime);
    return date;
}

char* get_random_record(int len){
	char *result = malloc(sizeof(char)*len+1);
	for(int i = 0; i < len; i++){
		result[i] = (char) (rand()%(122-97+1)+97);
	}
	result[len] = '\0';
	return result;
}

void modify_file(char** argv){
    char* file_name = argv[1];
    int pmin = extract_int(argv[2]);
    int pmax = extract_int(argv[3]);
    int bytes = extract_int(argv[4]);
    if(pmin > pmax){
        fprintf(stderr, "pmin can not be greater than pmax\n");
        exit(1);
    }
    while (1){
		char new_line[1024];
		pid_t pid = getpid();
		int rand_time = rand()%(pmax-pmin+1)+pmin;
		char *date = get_current_date();
		char *random_record = get_random_record(bytes);
		sprintf(new_line,"%i %i %s %s\n", pid,rand_time,date,random_record);
		free(date);
		free(random_record);
		FILE* file;
		if((file = fopen(file_name,"a")) == NULL){
			fprintf(stderr,"Unable to open the file\n");
			exit(1);
		}
		fwrite(new_line, sizeof(char), strlen(new_line), file);
		fclose(file);
		sleep(rand_time);
	}

}

int main(int argc, char** argv){
    if(argc != 5){
        fprintf(stderr, "Incorrect amount of arguments\n");
        return 1;
    }
    modify_file(argv);
}