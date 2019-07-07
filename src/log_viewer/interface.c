#include "interface.h"

#include <string.h>

#define HIGHLIGHT_ERROR 1
#define HIGHLIGHT_WHITE 2
#define HIGHLIGHT_YEL 3
#define HIGHLIGHT_BLACK 4
#define HIGHLIGHT_CYAN 5

#define HELP_TAB_SIZE 2

#define CENTER(col_size, title) col_size/2 - strlen(title)/2
#define RIGHT(col_size, title) col_size - strlen(title)-2

interface_t * interface_create()
{
	interface_t * this = malloc(sizeof(interface_t));
	return this;
}

void interface_destroy(interface_t * this)
{
	delwin(this->tabs_window);
	delwin(this->help_window);
	delwin(this->pad_window);

	for (int i = 0; i < this->tab_amount; ++i)
	{
		if(this->tabs[i] != 0)
		{
			delwin(this->tabs[i]->window);
			free(this->tabs[i]);
		}
	}


	free(this);
}

void interface_init(interface_t * this)
{
	this->tab_amount = 0;
	this->active_tab = 0;
	this->tabs_text_size = 0;
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

	this->tabs_window = interface_new_boxed_window(this->y_max-HELP_TAB_SIZE, this->x_max, 0, 0, "Refractil Log Viewer", CENTER);
	this->help_window = interface_new_window(HELP_TAB_SIZE, this->x_max, this->y_max-HELP_TAB_SIZE, 0);

	this->pad_window = newpad(this->y_max, this->x_max);

	interface_help_window_init(this);
}

WINDOW * interface_new_boxed_window(int row_size, int col_size, int y_start, int x_start, char * title, int position)
{
	WINDOW * win = newwin(row_size, col_size, y_start, x_start);
	box(win, 0, 0);
	if(title != NULL)
	{
		if(position == LEFT)
			mvwprintw(win, 0, 1, title);
		else if(position == CENTER)
		{
			wattron(win, COLOR_PAIR(HIGHLIGHT_WHITE));
			mvwprintw(win, 0, CENTER(col_size, title), title);
			wattroff(win, COLOR_PAIR(HIGHLIGHT_WHITE));
		}
		else if(position == RIGHT)
			mvwprintw(win, 0, RIGHT(col_size, title), title);
	}

	return win;
}

WINDOW * interface_new_window(int row_size, int col_size, int y_start, int x_start)
{
	return newwin(row_size, col_size, y_start, x_start);
}

void interface_refresh_all(interface_t * this)
{
	refresh();
	wrefresh(this->tabs_window);
	wrefresh(this->help_window);
}

void interface_print_tabs(interface_t * this)
{
	int tabs_max = (this->x_max-2) / 16 + 1;
	int tab_index = this->active_tab / tabs_max;
	int i = tab_index * tabs_max;

	while(i / tabs_max == tab_index)
	{
		if(i == this->active_tab)
		{
			wattron(this->tabs_window, COLOR_PAIR(HIGHLIGHT_CYAN));
				mvwprintw(this->tabs_window, 1, (i%tabs_max)*15+1, " %-15s ", this->tabs[i]->name);
			wattroff(this->tabs_window, COLOR_PAIR(HIGHLIGHT_CYAN));
		}
		else if(i < this->tab_amount)
		{
			wattron(this->tabs_window, COLOR_PAIR(HIGHLIGHT_WHITE));
				mvwprintw(this->tabs_window, 1, (i%tabs_max)*15+1, " %-15s ", this->tabs[i]->name);
			wattroff(this->tabs_window, COLOR_PAIR(HIGHLIGHT_WHITE));
		}
		else
		{
			mvwprintw(this->tabs_window, 1, (i%tabs_max)*15+1, " %-15s ", " ");	
		}

		++i;
	}

	wrefresh(this->tabs_window);

}

int interface_get_lines(FILE * file)
{
	char c = 0;
	int count = 0;
	for (c = fgetc(file); c != EOF; c = fgetc(file)) 
        if (c == '\n') // Increment count if this character is newline 
            count = count + 1; 

    rewind(file);
    return count;
}

void interface_add_tab(interface_t * this, char * name, char* file_name, char * regex)
{
	if(this->tab_amount == OPENED_MAX)
		return;

	tab_t * tab = malloc( sizeof(tab_t) );
	bzero(tab->regex, REGEX_MAX);

	this->tabs_text_size += 2 + strlen(name);

	tab->name[NAME_MAX] = 0;
	tab->file[FILE_MAX] = 0;

	strncpy(tab->name, name, NAME_MAX);
	strncpy(tab->file, file_name, FILE_MAX);

	tab->last_row = 0;
	tab->window = 0;
	tab->rows = 0;
	
	keypad(tab->window, TRUE);

	if ( (regex != 0 && regex[0] == 0) || regex == 0)
	{
		tab->has_regex = false;
	}
	else
	{
		tab->has_regex = true;
		strncpy(tab->regex, regex, REGEX_MAX);
	}

	this->tabs[this->tab_amount] = tab;
	++this->tab_amount;
	interface_print_tabs(this);

	int active_aux = this->active_tab;
	this->active_tab = this->tab_amount-1;
	interface_refresh_tab(this);

	this->active_tab = active_aux;

	wrefresh(tab->window);
}

void interface_help_window_init(interface_t * this)
{
	wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, "^o");
	wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, " Open file ");

	wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, "^e");
	wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, " Exit ");

	wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, "F5");
	wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, " Refresh ");

	wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, "R");
	wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, " Refresh all ");

	wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, "<-");
	wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, " Left tab ");

	wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, "->");
	wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, " Right tab ");

	wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, "^");
	wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, " Scroll up ");

	wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, "v");
	wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, " Scroll down ");

	wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, "End");
	wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, " End of file ");

	wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, "c");
	wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, " Toggle color ");

	wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, "r");
	wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	wprintw(this->help_window, " Toggle auto refresh ");

	interface_update_help_status(this);
}

void interface_update_help_status(interface_t * this)
{
	wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	mvwprintw(this->help_window, 1, 0, "Color:"); 
	wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	mvwprintw(this->help_window, 1, 7, "%s", this->color ? " enabled " : " disabled ");

	wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	mvwprintw(this->help_window, 1, 17, "Auto refresh:");
	wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));
	mvwprintw(this->help_window, 1, 30, "%s", this->auto_refresh ? " enabled " : " disabled ");

	wrefresh(this->help_window);
}

void interface_add_tab_popup(interface_t * this)
{
	WINDOW * popup = interface_new_boxed_window(8, this->x_max/4, this->y_max/3, this->x_max/4+9, "Open file", CENTER);
	mvwprintw(popup, 1, 1, "Tab name: ");
	mvwprintw(popup, 3, 1, "File name: ");
	mvwprintw(popup, 5, 1, "Regex: ");
	wrefresh(popup);

	char name_buffer[15];
	char file_name[40];
	char regex_buffer[40];

	bzero(name_buffer, 15);
	bzero(file_name, 40);
	bzero(regex_buffer, 40);

	while(true)
	{
		interface_window_input(this, popup, name_buffer, 14, 1, strlen("Tab name: ")+1);

		bool go_back = interface_window_input(this, popup, file_name, 39, 3, strlen("File name: ")+1 );

		if(!go_back)
			break;
	}

	interface_window_input(this, popup, regex_buffer, 39, 5, strlen("Regex: ")+1 );

	interface_add_tab(this, name_buffer, file_name, regex_buffer);

	delwin(popup);	
}

bool interface_window_input(interface_t * this, WINDOW * window, char * buffer, size_t size, size_t row_pos, size_t col_pos)
{
	curs_set(1);
	keypad(window, TRUE);
	int i = 0;
	int total = 0;
	while(total < size)
	{
		if(col_pos + i > this->x_max/4-2)
		{
			++row_pos;
			col_pos = 1;
			i = 0;
		}

		wmove(window, row_pos, col_pos+i);
		int input = wgetch(window);

		if(input >= 32 && input <= 126)
		{
			buffer[i] = input;
			mvwprintw(window, row_pos, col_pos+i, (const char*)&input);
			++total;
		}
		else if(input == KEY_BACKSPACE) // Backspace
		{
			i = i == 0 ? 1 : i;
			buffer[i-1] = 0;

			input = 32;
			mvwprintw(window, row_pos, col_pos+i-1, (const char*)&input);
			i = i-2 < 0 ? -1 : i-2;
			total = total-1 < 0 ? 0 : total-1;
		}
		else if(input == 10 || input == 9) // Enter || tab
		{
			buffer[i] = 0;
			break;
		}
		else if(input == 353) // shift + tab
			return true;

		++i;

	}
	curs_set(0);
	return false;
}

void interface_refresh_all_tabs(interface_t * this)
{
	size_t active_tab_aux = this->active_tab;
	for (size_t tab = 0; tab < this->tab_amount; ++tab)
	{
		this->active_tab = tab;
		interface_refresh_tab(this);
	}

	this->active_tab = active_tab_aux;
	wrefresh(this->tabs[this->active_tab]->window);
}

void interface_refresh_tab(interface_t * this)
{
	delwin(this->tabs[this->active_tab]->window);

	FILE * file = 0;

	if(this->tabs[this->active_tab]->has_regex)
	{
		char command[256];
		bzero(command, 256);
		sprintf(command, "grep \'%s\' %s > .grepresult", this->tabs[this->active_tab]->regex, this->tabs[this->active_tab]->file);
		file = popen((const char *)command, "r");
		pclose(file);

		file = fopen(".grepresult", "r");
	}
	else
	 	file = fopen(this->tabs[this->active_tab]->file, "r");


	if(file == NULL)
		return;

	int lines = interface_get_lines(file) + 2;
	if (lines < this->y_max-2-HELP_TAB_SIZE)
		lines = this->y_max;

	this->tabs[this->active_tab]->window = newpad(lines, this->x_max);
	keypad(this->tabs[this->active_tab]->window, TRUE);
	this->tabs[this->active_tab]->rows = lines;

	char buffer[this->x_max];
	
	for(int line = 0; line < lines; ++line)
	{
	 	fgets(buffer, this->x_max, file);
		buffer[ strlen(buffer)+1 ] = 0;

		if(strncmp(buffer, "INFO", 4) == 0 && this->color)
		{
			wattron(this->tabs[this->active_tab]->window, COLOR_PAIR(HIGHLIGHT_YEL));
			wprintw(this->tabs[this->active_tab]->window, "%s", buffer);
			wattroff(this->tabs[this->active_tab]->window, COLOR_PAIR(HIGHLIGHT_YEL));
		}
		else if(strncmp(buffer, "ERROR", 5) == 0 && this->color)
		{
			wattron(this->tabs[this->active_tab]->window, COLOR_PAIR(HIGHLIGHT_ERROR));
			wprintw(this->tabs[this->active_tab]->window, "%s", buffer);
			wattroff(this->tabs[this->active_tab]->window, COLOR_PAIR(HIGHLIGHT_ERROR));	
		}
		else
			wprintw(this->tabs[this->active_tab]->window, "%s", buffer);

		memset(buffer, 0, this->x_max);
	}

	if (this->tabs[this->active_tab]->has_regex)
		fclose(file);
	else
		pclose(file);
	wprintw(this->tabs[this->active_tab]->window, "\n");
}

void interface_main(interface_t * this)
{
	// Ventana, fila del pad, col del pad, fila de la ventana, col de pantalla, max de filas a refrescar, max de cols a refrescar
	int row = 0;
    fd_set fds;
    int maxfd;
    maxfd = 0;

    struct timeval timer;
    timer.tv_sec = 3;
    timer.tv_usec = 0;

	keypad(this->tabs_window, TRUE);
	while(true)
	{
		interface_update_help_status(this);
		// Print window data
		if(this->tab_amount > 0)
		{
			row = this->tabs[this->active_tab]->last_row;
			prefresh(this->tabs[this->active_tab]->window, row, 0, 2, 1, this->y_max-2-HELP_TAB_SIZE, this->x_max-2);
		}

		if (this->auto_refresh)
		{
	        FD_ZERO(&fds);
	        FD_SET(0, &fds); 
	        select(maxfd+1, &fds, NULL, NULL, &timer); 
	        if (FD_ISSET(0, &fds))
	        {
	        }
	        else
	        {
	        	if(this->tab_amount > 0)
	        		interface_refresh_tab(this);
	        	continue;
	        }
		}

		unsigned int input = wgetch(this->tabs_window);
		mvwprintw(this->help_window, 0, this->x_max-4, "%3d", input);

		if(input == 15) // ctrl + o
		{
			// abrir ventana
			interface_add_tab_popup(this);
			continue;
		}
		else if(input == 'c')
		{
			this->color = !this->color;
			if(this->tab_amount > 0)
				interface_refresh_tab(this);
			continue;
		}
		else if(input == 'r')
		{
			this->auto_refresh = !this->auto_refresh;
			continue;
		}
		else if(input == 5) // ctrl + e
			break;

		if(this->tab_amount > 0)
		{
			if(input == KEY_RIGHT)
			{
				this->active_tab = (this->active_tab+1) % this->tab_amount;
				interface_print_tabs(this);
			}
			else if(input == KEY_LEFT)
			{
				this->active_tab = this->active_tab - 1 < 0 ? this->tab_amount-1 : this->active_tab-1; 
				interface_print_tabs(this);	
			}
			else if(input == KEY_UP)
			{
				row = row-1 < 0 ? -1 : row-1;
				this->tabs[this->active_tab]->last_row = row;
			}
			else if(input == 269) // F5
			{
				interface_refresh_tab(this);
				wrefresh(this->tabs[this->active_tab]->window);
			}
			else if(input == 'R') // shift R
				interface_refresh_all_tabs(this);
			else if(input == 360) // end
				this->tabs[this->active_tab]->last_row = this->tabs[this->active_tab]->rows - this->y_max+2+HELP_TAB_SIZE;
			else
			{
				++row;
				this->tabs[this->active_tab]->last_row = row;
			}
		}
	}
}


