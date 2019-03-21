#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <memory.h>
#include <ftw.h>
#include <dirent.h>
#include <fcntl.h>

char* global_comparison_sign;
time_t global_date;

void getFileAttributes(const char* path, const struct stat *stat){
    printf("path: %s\ntype: ",path);

    if (S_ISREG(stat->st_mode))
        printf("file\n");
    else if(S_ISDIR(stat->st_mode))
        printf("dir\n");
    else if(S_ISCHR(stat->st_mode))
        printf("char dev\n");
    else if(S_ISBLK(stat->st_mode))
        printf("block dev\n");
    else if(S_ISFIFO(stat->st_mode))
        printf("fifo\n");
    else if(S_ISLNK(stat->st_mode))
        printf("slink\n");
    else if(S_ISSOCK(stat->st_mode))
        printf("socket\n");
    else
        fprintf(stderr,"unknown format of file\n");

    printf("size: %ld\n",stat->st_size);
    char tmp[20];
    strftime(tmp,20,"%d-%m-%Y",localtime(&stat->st_atime));
    printf("last accesed: %s\n",tmp);
    strftime(tmp,20,"%d-%m-%Y",localtime(&stat->st_mtime));
    printf("last modified: %s\n",tmp);

}

int compareDate(time_t file_modification_time, time_t date, const char *sign){
    if(strcmp(sign,"<") == 0){
        return (file_modification_time < date);
    }
    else if(strcmp(sign,">") == 0){
        return (file_modification_time > date);
    }
    else if(strcmp(sign,"=") == 0){
        return (file_modification_time == date);
    }
    else{
        fprintf(stderr, "Unknown comparison sign\n");
        exit(1);
    }
}

int showFileInfo(const char* path, const struct stat *stat, int typeflag, struct FTW *ftwbuf){
    if(ftwbuf->level == 0){
        return 0;
    }
    if(!compareDate(stat->st_mtime, global_date, global_comparison_sign))
        return 0;
    else
        getFileAttributes(path,stat);
    return 0;
}

void findStat(char* path){
    DIR *directory = opendir(path);
    struct dirent *file;
    struct stat stat;

    while( (file = readdir(directory))){
        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name,"..") == 0)
            continue;
        char next_path[256];
        strcpy(next_path,path);
        strcat(next_path,"/");
        strcat(next_path,file->d_name);
        fstatat(dirfd(directory), file->d_name, &stat, AT_SYMLINK_NOFOLLOW);
        if(stat.st_mode & S_IFDIR){
            findStat(next_path);
        }
        if(compareDate(stat.st_mtime,global_date,global_comparison_sign)){
            getFileAttributes(path, &stat);
        }

    }
    closedir(directory);
}

time_t getTime(char* date){
    struct tm timeTM;
    memset(&timeTM, 0, sizeof(struct tm));
    if(strptime(date, "%d-%m-%Y", &timeTM) == NULL){
        fprintf(stderr, "Incorrect format of date\n");
        return 1;
    }
    time_t time = mktime(&timeTM);
    return time;
}

int main(int argc, char** argv) {
    if(argc != 5){
        fprintf(stderr, "Incorrect amount of arguments\n");
        return 1;
    }
    char* path = realpath(argv[1],NULL);
    if(!path){
        fprintf(stderr,"Incorrect path\n");
        return 1;
    }
    global_comparison_sign = argv[2];
    global_date = getTime(argv[3]);
    char* type = argv[4];
    if(strcmp(type,"stat") == 0){
        findStat(path);
    }
    else if(strcmp(type,"nftw") == 0){
        nftw(path, showFileInfo, 100, FTW_PHYS);
    }
    else{
        fprintf(stderr,"Incorrect type\n");
        return 1;
    }
    free(path);
    return 0;
}