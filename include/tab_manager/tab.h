#ifndef TAB_H
#define TAB_H

#include <ncurses.h>
#include <string.h>
#include <regex.h>

/// File name buffer size
#define TAB_MAX_FILE_NAME 256

/// Tab name buffer size
#define TAB_MAX_TAB_NAME 20

/// Regular expression buffer size
#define TAB_MAX_REGEX_SIZE 256

#define TAB_KEYWORD_INFO "INFO"
#define TAB_KEYWORD_ERROR "ERROR"

typedef struct tab
{
    char name[TAB_MAX_TAB_NAME + 1];
    char file[TAB_MAX_FILE_NAME + 1];
    regex_t  * regex;

    size_t cols;
    size_t rows;
    size_t curr_row;
    size_t last_row;
    bool has_regex;
    bool color;

    WINDOW * window;
} tab_t;

// Init and destroy functions
tab_t * tab_create(char * name, char * file, char * regex, size_t cols,
                   size_t rows);
void tab_init(tab_t * tab, char * name, char * file, char * regex,
              size_t cols, size_t rows);
void tab_destroy(tab_t * tab);

void tab_destroy_contents(tab_t * tab);
static inline void tab_destroy_contents_cast(void * tab) { tab_destroy_contents((tab_t *)tab); };

void tab_resize(tab_t * tab, size_t rows, size_t cols);

// Print
void tab_print(tab_t * tab);
void tab_print_cast(void * tab);

// Color functions
int tab_get_line_color(tab_t * tab, char * line);
void tab_toggle_color(tab_t * tab);

// Data functions
void tab_set_regex(tab_t * tab, char * regex);
static inline void tab_set_lines(tab_t * tab, size_t rows) { tab->rows = rows; }
static inline void tab_set_name(tab_t * tab, char * name) { strncpy(tab->name, name, TAB_MAX_TAB_NAME); }
static inline void tab_set_file_name(tab_t * tab, char * file) { strncpy(tab->file, file, TAB_MAX_FILE_NAME); }


#endif