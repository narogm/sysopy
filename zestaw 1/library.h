#ifndef LIBRARY_H
#define LIBRARY_H

typedef struct file{
    char name[64];
    char location[512];
} sys_file;

void set_location(sys_file * file, const char* location);
void set_name(sys_file * file, const char* name);

char** create_table(int size);
int find_file(sys_file * file, const char *tmp_location);
int insert_to_table(char** table, int table_size, const char* file_name);
void remove_file(char** file_table, int index);
void clear_table(char** file_table, int size);

#endif
