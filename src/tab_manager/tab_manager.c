#include "tab_manager/tab_manager.h"
#include "log_viewer/interface.h"

#include <string.h>

int tab_manager_get_lines(FILE * file)
{
	char c = 0;
	int count = 0;
	for (c = fgetc(file); c != EOF; c = fgetc(file)) 
        if (c == '\n') // Increment count if this character is newline 
            count = count + 1; 

    rewind(file);
    return count;
}

void tab_manager_print_tabs(interface_t * this)
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

void tab_manager_add_tab_popup(interface_t * this)
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

	tab_manager_add_tab(this, name_buffer, file_name, regex_buffer);

	wclear(popup);
	wrefresh(popup);
	delwin(popup);	
}

void tab_manager_add_tab(interface_t * this, char * name, char* file_name, char * regex)
{
	if(this->tab_amount == OPENED_MAX)
		return;

	if(file_name[0] == 0)
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
	tab_manager_print_tabs(this);

	int active_aux = this->active_tab;
	this->active_tab = this->tab_amount-1;
	tab_manager_refresh_tab(this);

	this->active_tab = active_aux;

	wrefresh(tab->window);
}


void tab_manager_refresh_tab(interface_t * this)
{
	if(this->tab_amount <= 0 )
		return;
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

	int lines = tab_manager_get_lines(file) + 2;
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

void tab_manager_refresh_all_tabs(interface_t * this)
{
	size_t active_tab_aux = this->active_tab;
	for (size_t tab = 0; tab < this->tab_amount; ++tab)
	{
		this->active_tab = tab;
		tab_manager_refresh_tab(this);
	}

	this->active_tab = active_tab_aux;
	wrefresh(this->tabs[this->active_tab]->window);
}
