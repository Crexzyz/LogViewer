#ifndef FILE_OPS_H
#define FILE_OPS_H

#define FILE_OPS_READ_MODE "r"
#define FILE_OPS_BUFF_SIZE 256
#define FILE_OPS_FILE_ERROR -1
#define FILE_OPS_OK 0
#define FILE_OPS_EOF 1
#define FILE_OPS_REGEX_ERR 2

#include <stdio.h>

FILE * file_ops_open_file(char* path);
FILE * file_ops_open_at_line(char* path, size_t line);
int file_ops_get_line(FILE * file, char * regex, char * buffer);

#endif