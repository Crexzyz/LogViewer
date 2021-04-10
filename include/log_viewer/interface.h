#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>
#include <stdlib.h>

#include "tab_manager/tab_manager.h"
#include "context/context.h"

#define OPENED_MAX 20
#define IFACE_TITLE "Log Viewer"

enum interface_opcodes 
{
	IFACE_NOOP,
	IFACE_EXIT,
	IFACE_RESIZE,
	IFACE_TAB_ADDED,
	IFACE_SKIP_TAB_MGR,
	IFACE_TIMEOUT
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

// Main function
void interface_run(interface_t * interface);

// Input processing functions
size_t interface_handle_input(interface_t * interface, size_t input);

// Refreshing functions
void interface_refresh_status_bar(interface_t * this);

// Window functions
void interface_open_help(interface_t * interface);

// Data functions
void interface_toggle_color(interface_t * interface);
void interface_toggle_autorefresh(interface_t * interface);

// Resizing functions
void interface_resize_windows(interface_t * this);
void interface_resize_window(WINDOW * window, char * title, int position, int lines, int columns, bool draw_box);
 
#endif // INTERFACE_H