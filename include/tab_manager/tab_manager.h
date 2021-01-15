#ifndef TAB_MANAGER_H
#define TAB_MANAGER_H

#include "log_viewer/interface.h"
#include "input_window/input_window.h"
#include "tab.h"

#include <stdlib.h>
#include <ncurses.h>

int tab_manager_get_lines(FILE * file);
void tab_manager_print_tabs(interface_t * this);
void tab_manager_calc_tab_display_limits(interface_t * this);
void tab_manager_add_tab(interface_t * this, char * name, char* file, char * regex);
void tab_manager_add_tab_popup(interface_t * this);
void tab_manager_refresh_tab(interface_t * this);
void tab_manager_refresh_all_tabs(interface_t * this);
FILE * tab_manager_open_file(tab_t * current_tab);

#endif // TAB_MANAGER_H