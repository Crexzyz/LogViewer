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
	if(this->tab_amount <= 0)
		return;
		
	const int printable_chars = this->x_max-2;

	tab_manager_calc_tab_display_limits(this);

	int print_index = 1;
	for (int tab = this->tab_display_start; tab < this->tab_display_end; ++tab)
	{
		if(tab == this->active_tab)
		{
			wattron(this->tabs_window, COLOR_PAIR(HIGHLIGHT_CYAN));
				mvwprintw(this->tabs_window, 1, print_index, "%s ", this->tabs[tab]->name);
			wattroff(this->tabs_window, COLOR_PAIR(HIGHLIGHT_CYAN));
		}
		else if(tab < this->tab_amount)
		{
			wattron(this->tabs_window, COLOR_PAIR(HIGHLIGHT_WHITE));
				mvwprintw(this->tabs_window, 1, print_index, "%s ", this->tabs[tab]->name);
			wattroff(this->tabs_window, COLOR_PAIR(HIGHLIGHT_WHITE));
		}
		print_index += strlen(this->tabs[tab]->name) + 1;
	}

	// Clean unused tab space
	for (int i = print_index; i < printable_chars; ++i)
		mvwprintw(this->tabs_window, 1, i, " ");

	wrefresh(this->tabs_window);
}

void tab_manager_calc_tab_display_limits(interface_t * this)
{
	// 1: Left window border

	// -2: Window borders
	const int printable_chars = this->x_max-2;
	// Check if active tab fits in screen
	bool tab_displayed = false;
	while(!tab_displayed)
	{
		int printed_chars = 1;
		// Check how many tabs fit in the screen with the previous limits
		for (int tab = this->tab_display_start; tab < this->tab_amount; ++tab)
		{
			int tab_size = strlen(this->tabs[tab]->name) + 1;
			if(tab_size + printed_chars < printable_chars)
			{
				this->tab_display_end = tab+1;
				printed_chars += tab_size;
			}
		}	

		// If the active tab is not displayed within the current limits, change them and recalculate
		if(this->active_tab < this->tab_display_start)
		{
			int old_start = this->tab_display_start;
			this->tab_display_start = this->active_tab;
			this->tab_display_end = this->tab_display_end - (old_start - this->tab_display_start);
		} 
		else if(this->active_tab >= this->tab_display_end)
		{
			int old_end = this->tab_display_end;
			this->tab_display_end = this->active_tab+1;
			this->tab_display_start = this->tab_display_start + (this->tab_display_end - old_end); 
		}
		else
			tab_displayed = true;
	}

	mvwprintw(this->tabs_window, 0, 0, "AT:%2d, Start:%2d, End:%2d", this->active_tab, this->tab_display_start, this->tab_display_end);
}

void tab_manager_add_tab_popup(interface_t * this)
{
	input_window_t input;
	input_window_init(&input, this, this->y_max, this->x_max);
	input_window_get_input(&input);
	tab_manager_add_tab(this, input_window_get_tab_name(&input), input_window_get_file_name(&input), input_window_get_regex(&input));

	input_window_destroy(&input);
}

void tab_manager_add_tab(interface_t * this, char * name, char* file_name, char * regex)
{
	if(this->tab_amount == OPENED_MAX)
		return;

	if(file_name[0] == 0)
		return;

	tab_t * tab = malloc( sizeof(tab_t) );
	bzero(tab->regex, MAX_REGEX);

	tab->name[MAX_TAB_NAME] = 0;
	tab->file[MAX_FILE_NAME] = 0;

	strncpy(tab->name, name, MAX_TAB_NAME);
	strncpy(tab->file, file_name, MAX_FILE_NAME);

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
		strncpy(tab->regex, regex, MAX_REGEX);
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
		// Change this size later
		char command[600];
		bzero(command, 600);
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

	fclose(file);

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
