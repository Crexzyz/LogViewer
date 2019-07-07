#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>
#include <stdlib.h>

#define NAME_MAX 15
#define FILE_MAX 63
#define REGEX_MAX 63

#define OPENED_MAX 20

enum positions
{
	LEFT,
	CENTER,
	RIGHT
};

typedef struct tab
{
	char name[NAME_MAX+1];
	char file[FILE_MAX+1];
	char regex[REGEX_MAX+1];
		
	WINDOW * window;
	size_t last_row;
	size_t rows;
	bool has_regex;

} tab_t;

typedef struct interface
{
	WINDOW * tabs_window;
	WINDOW * help_window;
	WINDOW * pad_window;

	tab_t * tabs[OPENED_MAX];

	int tab_amount;
	int active_tab;

	int tabs_text_size;

	int y_max;
	int x_max;

	bool auto_refresh;
	bool color;
} interface_t;

interface_t * interface_create();
void interface_destroy(interface_t * this);

void interface_init(interface_t * this);
WINDOW * interface_new_boxed_window(int row_size, int col_size, int y_start, int x_start, char* name, int position);
WINDOW * interface_new_window(int row_size, int col_size, int y_start, int x_start);
void interface_help_window_init(interface_t * this);
void interface_update_help_status(interface_t * this);

void interface_main(interface_t * this);
void interface_refresh_all(interface_t * this);

int interface_get_lines(FILE * file);
void interface_print_tabs(interface_t * this);
void interface_add_tab(interface_t * this, char * name, char* file, char * regex);
void interface_add_tab_popup(interface_t * this);
void interface_refresh_tab(interface_t * this);
void interface_refresh_all_tabs(interface_t * this);

bool interface_window_input(interface_t * this, WINDOW * window, char * buffer, size_t size, size_t row_pos, size_t col_pos);


#endif // INTERFACE_H