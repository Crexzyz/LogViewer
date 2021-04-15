#include "utils/file_ops.h"

#include <regex.h>
#include <string.h>

FILE * file_ops_open_file(char* path)
{
    if(!path)
        return NULL;

    FILE * file = fopen(path, FILE_OPS_READ_MODE);
    return file;
}


FILE * file_ops_open_at_line(char* path, size_t line)
{
    if(!path)
        return NULL;

    FILE * file = file_ops_open_file(path);

    int c = 0;
    size_t lines = 0;
    while ((c = fgetc(file)) != EOF)
    {
        if(c == '\n')
            ++lines;

        if(lines == line - 1)
            break;
    }

    return file;
}


int file_ops_get_line(FILE * file, char * pattern, char * buffer)
{
    if(!file)
        return FILE_OPS_FILE_ERROR;

    char * res = fgets(buffer, FILE_OPS_BUFF_SIZE, file);

    if(res == NULL) // EOF
        return FILE_OPS_EOF;

    if(!pattern)
        return FILE_OPS_OK;

    regex_t regex;
    int error = regcomp(&regex, pattern, 0);
    if(error != 0)
        return FILE_OPS_REGEX_ERR;

    error = regexec(&regex, buffer, 0, NULL, 0);

    // No match, clean buffer
    if(error != 0) 
        bzero(buffer, FILE_OPS_BUFF_SIZE); 

    return FILE_OPS_OK;
}