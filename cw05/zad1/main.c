#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>

const int ARGS_LIMIT = 10;
int prev_fd[2]={-1,-1};

void execute_command(char* command){
	char* arg = strtok(command, " ");
	char* args[ARGS_LIMIT];
	char* com = arg;
	int i=0;
	while(arg != NULL){
		args[i] = arg;
		arg = strtok(NULL, " ");
		i++;
	}
	int curr_fd[2];
	pipe(curr_fd);
	if(fork()==0){
		if(prev_fd[0] != -1){
			dup2(prev_fd[0], STDIN_FILENO);
			close(prev_fd[0]);
			close(prev_fd[1]);
		}
		dup2(curr_fd[1], STDOUT_FILENO);
		close(curr_fd[0]);
		close(curr_fd[1]);
		execvp(com, args);
		exit(0);
	}
	else {
		if(prev_fd[0] != -1){
			close(prev_fd[0]);
			close(prev_fd[1]);
		}
		prev_fd[0] = curr_fd[0];
		prev_fd[1] = curr_fd[1];
	}
}

void parse_and_execute(char* line){
	char * command = strtok(line,"|");
	int amount = 0;
	while(command != NULL){
		execute_command(command);
		command = strtok(NULL,"|");
		amount++;
	}
	while(amount > 0){
		wait(NULL);
		amount--;
	}
	close(prev_fd[0]);
	close(prev_fd[1]);
}

void get_commands_from_file(char* path){
	FILE * file;
	if((file = fopen(path,"r")) == NULL){
		fprintf(stderr, "Unable to open file\n");
		exit(1);
	}
	size_t size = 0;
	char* line;
	int read;
	while((read = getline(&line,&size,file)) != -1){
		parse_and_execute(line);
	}
	fclose(file);
}

int main(int argc, char** argv) {
	if(argc != 2){
		fprintf(stderr, "Incorrect amount of arguments\n");
		exit(1);
	}
	char* path = argv[1];
	get_commands_from_file(path);

	return 0;
}