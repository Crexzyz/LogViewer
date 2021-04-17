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

    if(line == 0)
        return file;

    int c = 0;
    size_t line_count = 0;
    while ((c = fgetc(file)) != EOF)
    {
        if(c == '\n')
            ++line_count;

        if(line_count >= line)
            break;
    }

    return file;
}


int file_ops_get_line(FILE * file, regex_t * regex, char * buffer)
{
    if(!file)
        return FILE_OPS_FILE_ERROR;

    char * res = fgets(buffer, FILE_OPS_BUFF_SIZE, file);

    if(res == NULL) // EOF
        return FILE_OPS_EOF;

    if(!regex)
        return FILE_OPS_OK;

    int error = regexec(regex, buffer, 0, NULL, 0);

    // No match, clean buffer
    if(error != 0) 
        bzero(buffer, FILE_OPS_BUFF_SIZE); 

    return FILE_OPS_OK;
}