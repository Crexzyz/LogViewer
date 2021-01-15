#ifndef TAB_H
#define TAB_H

#include <ncurses.h>
#include <string.h>

/// File name buffer size
#define TAB_MAX_FILE_NAME 256

/// Tab name buffer size
#define TAB_MAX_TAB_NAME 20

/// Regular expression buffer size
#define TAB_MAX_REGEX_SIZE 256

typedef struct tab
{
	char name[TAB_MAX_FILE_NAME + 1];
	char file[TAB_MAX_TAB_NAME + 1];
	char regex[TAB_MAX_REGEX_SIZE + 1];
		
	WINDOW * window;
	size_t last_row;
	size_t rows;
	bool has_regex;

} tab_t;

tab_t * tab_create();
void tab_destroy(tab_t * tab);
void tab_init(tab_t * tab);

static inline void tab_set_name(tab_t * tab, char * name) { strncpy(tab->name, name, TAB_MAX_FILE_NAME); }
static inline void tab_set_file_name(tab_t * tab, char * file) { strncpy(tab->file, file, TAB_MAX_TAB_NAME); }
static inline void tab_set_regex(tab_t * tab, char * regex) { tab->has_regex = true; strncpy(tab->regex, regex, TAB_MAX_REGEX_SIZE); }


#endif