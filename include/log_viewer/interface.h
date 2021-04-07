#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>
#include <stdlib.h>

#include "tab_manager/tab_manager.h"
#include "context/context.h"

#define OPENED_MAX 20

enum interface_opcodes 
{
	IFACE_NOOP,
	IFACE_EXIT,
	IFACE_RESIZE,
	IFACE_TAB_ADDED,
	IFACE_TAB_HANDLE
};

typedef struct interface
{
	WINDOW * tabs_window;
	WINDOW * help_window;
	tab_manager_t * tab_manager;
	context_t * context;

	bool auto_refresh;
	bool color;

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
void interface_draw_borders(WINDOW * win, char * title, int position, int col_size, bool draw_box);

// Main function
void interface_main(interface_t * this);
void interface_run(interface_t * interface);

// Input processing functions
size_t interface_handle_input(interface_t * interface, size_t input);
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