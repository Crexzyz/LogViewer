#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>
#include <stdlib.h>

#define OPENED_MAX 20

/// File name buffer size
#define MAX_FILE_NAME 256

/// Tab name buffer size
#define MAX_TAB_NAME 20

/// Regular expression buffer size
#define MAX_REGEX 256

#define HELP_TAB_SIZE 2

enum text_positions
{
	LEFT,
	CENTER,
	RIGHT
};

typedef struct tab
{
	char name[MAX_TAB_NAME+1];
	char file[MAX_FILE_NAME+1];
	char regex[MAX_REGEX+1];
		
	WINDOW * window;
	size_t last_row;
	size_t rows;
	bool has_regex;

} tab_t;

typedef struct interface
{
	WINDOW * tabs_window;
	WINDOW * help_window;

	tab_t * tabs[OPENED_MAX];

	int tab_amount;
	int active_tab;

	int y_max;
	int x_max;

	bool auto_refresh;
	bool color;

	int tab_display_start;
	int tab_display_end;
} interface_t;

#define STATUS_SYMBOLS 2
#define STATUS_SYMBOLS_SIZE 2

/**
 * @brief      Creates an instance of the whole interface in heap memory.
 *
 * @return     A pointer to the instance of the interface in heap memory.
 * @remarks    Data allocated by this call must be freed.
 */
interface_t * interface_create();
void interface_destroy(interface_t * this);

// Initialiaztion functions
void interface_init(interface_t * this);
WINDOW * interface_new_boxed_window(int row_size, int col_size, int y_start, int x_start, char* name, int position);
WINDOW * interface_new_window(int row_size, int col_size, int y_start, int x_start);
void interface_help_window_init(interface_t * this);
void interface_draw_borders(WINDOW * win, char * title, int position, int col_size, bool draw_box);

// Main function
void interface_main(interface_t * this);

// Input processing functions
int interface_process_auto_refresh(interface_t * this, bool resized);
int interface_process_options(interface_t * this, int input, bool * resized);
int interface_process_tab_options(interface_t * this, int input, int row);

// Tab refreshing functions
void interface_update_help_status(interface_t * this);
void interface_refresh_all(interface_t * this);

// Resizing functions
void interface_resize_windows(interface_t * this);
void interface_resize_window(WINDOW * window, char * title, int position, int lines, int columns, bool draw_box);
 
#endif // INTERFACE_H