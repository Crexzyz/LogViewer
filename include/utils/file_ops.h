#ifndef FILE_OPS_H
#define FILE_OPS_H

FILE * file_ops_open_file(char* path);
FILE * file_ops_open_at_line(char* path, size_t line);
char * file_ops_get_line(FILE * file, char * regex);

#endif