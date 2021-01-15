#include "tab_manager/tab_manager.h"
#include "utils.h"
#include "tab.h"

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
		size_t color = 0;

		if(tab == this->active_tab)
			color = COLOR_PAIR(HIGHLIGHT_CYAN);
		else if(tab < this->tab_amount)
			color = COLOR_PAIR(HIGHLIGHT_WHITE);

		wattron(this->tabs_window, color);
			mvwprintw(this->tabs_window, 1, print_index, "%s ", this->tabs[tab]->name);
		wattroff(this->tabs_window, color);

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
	// File name and existence validations
	if(this->tab_amount == OPENED_MAX) return; // TODO: Popup

	if(file_name[0] == 0) return;

	if(name[0] == 0) name = "<No name>";
	
	tab_t * tab = tab_create();
	tab_set_name(tab, name);
	tab_set_file_name(tab, file_name);

	FILE * file = tab_manager_open_file(tab);
	
	if(file == NULL)
	{
		tab_destroy(tab);
		return;
	}

	// Count file's lines
	int lines = tab_manager_get_lines(file) + 2;
	fclose(file);

	// Smaller size than screen
	if (lines < this->y_max - 2 - HELP_TAB_SIZE) 
		lines = this->y_max;

	tab_set_lines(tab, lines);

	if ( (regex != 0 && regex[0] == 0) || regex == 0)
		tab->has_regex = false;
	else
		tab_set_regex(tab, regex);

	tab_add_pad(tab, this->x_max, tab->rows);

	this->tabs[this->tab_amount] = tab;
	++this->tab_amount;
	tab_manager_print_tabs(this);
	
	// Update newly added tab and set pointer back to the current tab
	size_t active_aux = this->active_tab;
	this->active_tab = this->tab_amount-1;
	tab_manager_refresh_tab(this);
	this->active_tab = active_aux;
}

FILE * tab_manager_open_file(tab_t * current_tab)
{
	FILE * file = 0;

	if(current_tab->has_regex)
	{
		// Change this size later
		char command[600];
		bzero(command, 600);
		sprintf(command, "grep \'%s\' %s > .grepresult", current_tab->regex, current_tab->file);
		file = popen((const char *)command, "r");
		pclose(file);

		file = fopen(".grepresult", "r");
	}
	else
	{
	 	file = fopen(current_tab->file, "r");
	}

	if(file == NULL)
	{
		mvprintw(0,0, "fail");
		refresh();
	}

	return file;
}


void tab_manager_refresh_tab(interface_t * this)
{
	tab_t * current_tab = this->tabs[this->active_tab];

	FILE * file = tab_manager_open_file(current_tab);

	if(file)
	{
		tab_print(current_tab, this->color, file);

		fclose(file);
	}
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
