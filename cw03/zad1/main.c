#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <memory.h>
#include <ftw.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>

void findStat(char *path) {
	int pid;
	if ((pid = fork()) == 0) {
		DIR *directory = opendir(path);
		struct dirent *file;
		struct stat stat;

		while ((file = readdir(directory))) {
			if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
				continue;
			char next_path[256];
			strcpy(next_path, path);
			strcat(next_path, "/");
			strcat(next_path, file->d_name);
			fstatat(dirfd(directory), file->d_name, &stat, AT_SYMLINK_NOFOLLOW);
			if (stat.st_mode & S_IFDIR) {
				findStat(next_path);
			}
		}
		printf("\n%s   --- pid: %i  --- ppid:%i\n", path, getpid(), getppid());
		if (execlp("ls", "ls", "-l", path, NULL) == -1) {
			fprintf(stderr, "Unable to list");
			exit(1);
		}
		closedir(directory);
	}
	waitpid(pid, NULL, 0);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Incorrect amount of arguments\n");
		return 1;
	}
	char *path = realpath(argv[1], NULL);
	if (!path) {
		fprintf(stderr, "Incorrect path\n");
		return 1;
	}
	findStat(path);
	free(path);
	return 0;
}