#ifndef TAB_MANAGER_H
#define TAB_MANAGER_H

#include <stdlib.h>
#include <ncurses.h>

#include "tab.h"
#include "context/context.h"
#include "utils/list.h"

#define HELP_TAB_SIZE 1

#define TM_FIX_SCR_COLS(cols) cols - 2
#define TM_FIX_SCR_ROWS(rows) rows - HELP_TAB_SIZE - 3

typedef struct tab_manager {
    context_t * context;
    list_t * tab_list;
	size_t active_tab;
    size_t tab_display_start;
	size_t tab_display_end;
} tab_manager_t;

// Tab init and destroy functions
tab_manager_t * tab_manager_create(context_t * context);
void tab_manager_init(tab_manager_t * tm, context_t * context);
void tab_manager_destroy(tab_manager_t * tm);

// Input functions
void tab_manager_handle_input(tab_manager_t * tm, size_t input);

// Utility functions
tab_t * tab_manager_get_active_tab(tab_manager_t * tm);
size_t tab_manager_tabs_amount(tab_manager_t * tm);

// Print functions
void tab_manager_print_tabs(tab_manager_t * tm, WINDOW * target_window);
void tab_manager_print_active(tab_manager_t * tm, WINDOW * target_window);
void tab_manager_refresh_all_tabs(tab_manager_t * this);

// Calculation functions
void tab_manager_update_limits(tab_manager_t * this);

// Tab functions
void tab_manager_add_tab(tab_manager_t * this, char * name, char* file_name, char * regex);
void tab_manager_add_tab_popup(tab_manager_t * this);
void tab_manager_toggle_color(tab_manager_t * tm);
bool tab_manager_get_color(tab_manager_t * tm);
void tab_manager_close_tab(tab_manager_t * tm);
void tab_manager_resize(tab_manager_t * tm);

#endif // TAB_MANAGER_H