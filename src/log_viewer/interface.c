#include "log_viewer/interface.h"
#include "tab_manager/tab_manager.h"
#include "help_window/help_window.h"
#include "utils.h"

#include <string.h>
#include <time.h>

const char INTERFACE_SYMBOLS[STATUS_SYMBOLS][STATUS_SYMBOLS_SIZE] = 
{
	"C\0", "R\0"
};

interface_t * interface_create()
{
	interface_t * this = malloc(sizeof(interface_t));
	return this;
}

void interface_destroy(interface_t * this)
{
	delwin(this->tabs_window);
	delwin(this->help_window);

	for (int i = 0; i < this->tab_amount; ++i)
	{
		if(this->tabs[i] != 0)
		{
			tab_destroy(this->tabs[i]);
		}
	}

	free(this);
}

void interface_init(interface_t * this)
{
	this->tab_amount = 0;
	this->active_tab = 0;
	this->tab_display_start = 0;
	this->tab_display_end = 0;
	this->color = true;
	this->auto_refresh = true;

	for (int i = 0; i < OPENED_MAX; ++i)
	{
		this->tabs[i] = 0;
	}

	init_color(COLOR_BLACK, 0,0,0);
	init_pair(HIGHLIGHT_ERROR, COLOR_WHITE, COLOR_RED);
	init_pair(HIGHLIGHT_WHITE, COLOR_BLACK, COLOR_WHITE);
	init_pair(HIGHLIGHT_YEL, COLOR_BLACK, COLOR_YELLOW);
	init_pair(HIGHLIGHT_BLACK, COLOR_WHITE, COLOR_BLACK);
	init_pair(HIGHLIGHT_CYAN, COLOR_BLACK, COLOR_CYAN);

	getmaxyx(stdscr, this->y_max, this->x_max);

	refresh();

	this->tabs_window = interface_new_boxed_window(this->y_max-HELP_TAB_SIZE, this->x_max, 0, 0, "Log Viewer", CENTER);
	this->help_window = interface_new_window(HELP_TAB_SIZE, this->x_max, this->y_max-HELP_TAB_SIZE, 0);

	interface_help_window_init(this);
}

WINDOW * interface_new_boxed_window(int row_size, int col_size, int y_start, int x_start, char * title, int position)
{
	WINDOW * win = newwin(row_size, col_size, y_start, x_start);
	interface_draw_borders(win, title, position, col_size, true);
	return win;
}

void interface_draw_borders(WINDOW * win, char * title, int position, int col_size, bool draw_box)
{
	if(draw_box)
		box(win, 0, 0);

	if(title != NULL)
	{
		if(position == LEFT)
			mvwprintw(win, 0, 1, title);
		else if(position == CENTER)
		{
			wattron(win, COLOR_PAIR(HIGHLIGHT_WHITE));
			mvwprintw(win, 0, CENTER_TEXT(col_size, title), title);
			wattroff(win, COLOR_PAIR(HIGHLIGHT_WHITE));
		}
		else if(position == RIGHT)
			mvwprintw(win, 0, RIGHT_TEXT(col_size, title), title);
	}
}

WINDOW * interface_new_window(int row_size, int col_size, int y_start, int x_start)
{
	return newwin(row_size, col_size, y_start, x_start);
}

void interface_refresh_all(interface_t * this)
{
	wrefresh(this->tabs_window);
	wrefresh(this->help_window);
	refresh();
}

void interface_help_window_init(interface_t * this)
{
	interface_update_help_status(this);
}

void interface_update_help_status(interface_t * this)
{
	bool status[2] = {this->color, this->auto_refresh};

	for(size_t symbol = 0; symbol < STATUS_SYMBOLS; ++symbol)
	{
		if(status[symbol])
			wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));

		mvwprintw(this->help_window, 0, symbol, INTERFACE_SYMBOLS[symbol]);

		wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));	
	}

	wrefresh(this->help_window);
}

void interface_resize_windows(interface_t * this)
{
	// Get new dimensions
	getmaxyx(stdscr, this->y_max, this->x_max);

	// Resize main window
	interface_resize_window(this->tabs_window, "Log Viewer", CENTER, this->y_max-HELP_TAB_SIZE, this->x_max, true);

	// Resize help window
	mvwin(this->help_window, this->y_max-HELP_TAB_SIZE, 0);
	interface_resize_window(this->help_window, 0, 0, HELP_TAB_SIZE, this->x_max, false);
	interface_help_window_init(this);

	// Refresh current tab
	tab_manager_refresh_tab(this);

	// Refresh tabs indicator
	tab_manager_print_tabs(this);

	// Refresh tab content
	if(this->tab_amount > 0)
	{
		prefresh(this->tabs[this->active_tab]->window, this->tabs[this->active_tab]->last_row, 0, 
			2, 1, this->y_max-2-HELP_TAB_SIZE, this->x_max-2);
	}
}

void interface_resize_window(WINDOW * window, char * title, int position, int lines, int columns, bool draw_box)
{
	wclear(window);
	wresize(window, lines, columns);
	interface_draw_borders(window, title, position, columns, draw_box);
	wrefresh(window);
}

void interface_main(interface_t * this)
{
	// Ventana, fila del pad, col del pad, fila de la ventana, col de pantalla, max de filas a refrescar, max de cols a refrescar
	int row = 0;
    bool resized = true;
    int opcode = -1;

	keypad(this->tabs_window, TRUE);
	while(true)
	{
		// Update color and auto-refresh status
		interface_update_help_status(this);

		// Update tab data if there are any
		if(this->tab_amount > 0)
		{
			row = this->tabs[this->active_tab]->last_row;
			prefresh(this->tabs[this->active_tab]->window, row, 0, 2, 1, this->y_max-2-HELP_TAB_SIZE, this->x_max-2);
		}

		// Auto refresh current tab
		opcode = interface_process_auto_refresh(this, resized);
		if(opcode == 0)
			continue;

		unsigned int input = wgetch(this->tabs_window);
		mvwprintw(this->help_window, 0, this->x_max-4, "%3d", input);

		// Process options-related keybindings
		opcode = interface_process_options(this, input, &resized);
		if(opcode == 0)
			continue;
		else if(opcode == 1)
			break;

		// Process tab-related keybindings
		row = interface_process_tab_options(this, input, row);
		
	}
}

int interface_process_auto_refresh(interface_t * this, bool resized)
{
	if (this->auto_refresh)
	{
	    if(!resized)
	    {
    	    fd_set fds;
    		int maxfd = 0;

    		// Set a 1 second timer
			struct timeval timer;
			timer.tv_sec = 1;
			// Clean fd set
	   		FD_ZERO(&fds);
	   		// Add stdin to set
	    	FD_SET(0, &fds); 
	    	// Wait 1 second for user input
	        select(maxfd+1, &fds, NULL, NULL, &timer); 
	        // (Conditional not really necessary) If stdin is in set, update current tab
	        if (!FD_ISSET(0, &fds))
	        {
	        	if(this->tab_amount > 0)
	        		tab_manager_refresh_tab(this);
	        }

	        return 0;
	    }
	}

	return 1;
}

int interface_process_options(interface_t * this, int input, bool * resized)
{
	if(input == KEY_RESIZE)
	{
		interface_resize_windows(this);
		*resized = true;
	}
	else if(input == 15) // ctrl + o
	{
		tab_manager_add_tab_popup(this);
	}
	else if(input == 'c')
	{
		this->color = !this->color;
		if(this->tab_amount > 0)
			tab_manager_refresh_tab(this);
	}
	else if(input == 'r')
	{
		this->auto_refresh = !this->auto_refresh;
	}
	else if(input == 5) // ctrl + e
	{
		return 1;
	}
	else if(input == 8) // ctrl + h
	{
		help_window_t * hw = help_window_create(this->y_max, this->x_max, 0, 0);

		help_window_show(hw);
		help_window_listen_keys(hw);
		help_window_destroy(hw);
		
		interface_draw_borders(this->tabs_window, "Log Viewer", CENTER, this->x_max, true);
		wrefresh(this->tabs_window);
	}
	else
		return 2;

	return 0;
}

int interface_process_tab_options(interface_t * this, int input, int row)
{
	if(this->tab_amount > 0)
	{
		if(input == KEY_RIGHT)
		{
			this->active_tab = (this->active_tab+1) % this->tab_amount;
			tab_manager_print_tabs(this);
		}
		else if(input == KEY_LEFT)
		{
			this->active_tab = this->active_tab - 1 < 0 ? this->tab_amount-1 : this->active_tab-1; 
			tab_manager_print_tabs(this);	
		}
		else if(input == KEY_UP)
		{
			row = row-1 < 0 ? -1 : row-1;
			this->tabs[this->active_tab]->last_row = row;
		}
		else if(input == 269) // F5
		{
			tab_manager_refresh_tab(this);
			wrefresh(this->tabs[this->active_tab]->window);
		}
		else if(input == 'R') // shift R
			tab_manager_refresh_all_tabs(this);
		else if(input == 360) // end
			this->tabs[this->active_tab]->last_row = this->tabs[this->active_tab]->rows - this->y_max+2+HELP_TAB_SIZE;
		else
		{
			++row;
			this->tabs[this->active_tab]->last_row = row;
		}
	}
	return row;
}