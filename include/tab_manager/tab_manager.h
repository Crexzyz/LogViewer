#ifndef TAB_MANAGER_H
#define TAB_MANAGER_H

#include <stdlib.h>
#include <ncurses.h>

#include "tab.h"

#define TAB_MANAGER_MAX_TABS 20
#define HELP_TAB_SIZE 1

typedef struct tab_manager {
    tab_t * tabs[TAB_MANAGER_MAX_TABS];
    size_t tab_amount;
	size_t active_tab;
    size_t tab_display_start;
	size_t tab_display_end;
    size_t max_cols;
    size_t max_rows;
} tab_manager_t;

tab_manager_t * tab_manager_create(size_t max_cols, size_t max_rows);
void tab_manager_init(tab_manager_t * tm, size_t max_cols, size_t max_rows);
void tab_manager_destroy(tab_manager_t * tm);

void tab_manager_print_tabs(tab_manager_t * this, WINDOW * tabs_window);
void tab_manager_update_limits(tab_manager_t * this);
void tab_manager_add_tab(tab_manager_t * this, WINDOW * tab_win, char * name, char* file_name, char * regex);
void tab_manager_add_tab_popup(tab_manager_t * this, WINDOW * tab_win);
void tab_manager_refresh_tab(tab_manager_t * this, bool color);
void tab_manager_refresh_all_tabs(tab_manager_t * this, bool color);

FILE * tab_manager_open_file(tab_t * current_tab);
int tab_manager_get_lines(FILE * file);

#endif // TAB_MANAGER_H