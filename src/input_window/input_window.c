#include "input_window/input_window.h"

#include <stdlib.h>

#define TEXT_SIZE(buffer) input_window_get_size_by_type(this, buffer, TEXT)

input_window_t * input_window_create()
{
	input_window_t * this = malloc(sizeof(input_window_t));

	return this;
}

void input_window_destroy(input_window_t * this)
{
	free(this->file_name);
	free(this->tab_name);
	free(this->regex);

	wclear(this->window);
	wrefresh(this->window);
	delwin(this->window);
}

void input_window_init(input_window_t * this, interface_t * interface, int rows, int cols)
{
	this->rows = rows;
	this->cols = cols;
	this->interface = interface;

	this->window_rows = CALCULATE_USAGE(rows);
	this->window_cols = CALCULATE_USAGE(cols);

	this->file_name = malloc(MAX_FILE_NAME);
	this->tab_name = malloc(MAX_TAB_NAME);
	this->regex = malloc(MAX_REGEX);

	CLEAN_BUFFERS(this);

	this->window = newwin(this->window_rows, this->window_cols, 
		CALCULATE_COORDS(this->rows, this->window_rows), CALCULATE_COORDS(this->cols, this->window_cols));

	input_window_init_text(this);

	wrefresh(this->window);
}

void input_window_init_text(input_window_t * this)
{
	box(this->window, 0, 0);

	wattron(this->window, COLOR_PAIR(2));
	mvwprintw(this->window, 0, CENTER_TEXT(this->window_cols, TITLE), TITLE);
	wattroff(this->window, COLOR_PAIR(2));

	if(this->window_rows >= 5)
	{
		mvwprintw(this->window, 1, 1, "Tab name: ");
		mvwprintw(this->window, 3, 1, "File name: ");
		mvwprintw(this->window, 5, 1, "Regex: ");
	}
}

void input_window_get_input(input_window_t * this)
{
	bool go_back = false;
	char * buffer = 0;
	int buffer_type = 0;
	CLEAN_BUFFERS(this);

	while(buffer_type < 3)
	{	
		switch(buffer_type)
		{
			case 0: buffer = this->tab_name; break;
			case 1: buffer = this->file_name; break;
			case 2: buffer = this->regex; break;
			default: buffer = 0;
		}

		go_back = input_window_read_text(this, buffer, buffer_type*2+1, input_window_get_size_by_type(this, buffer, TEXT)+1);
		if(!go_back)
			++buffer_type;
		else
			buffer_type = buffer_type == 0 ? 0 : buffer_type - 1;
	}
}

bool input_window_read_text(input_window_t * this, char * buffer, int cursor_row, int cursor_col)
{
	curs_set(1);
	keypad(this->window, TRUE);
	int cursor_pos = 0;
	int total = 0;
	int buffer_size = input_window_get_size_by_type(this, buffer, BUFFER);

	while(total < buffer_size)
	{
		if(cursor_col + cursor_pos > this->window_cols-2)
		{
			++cursor_row;
			cursor_col = 1;
			cursor_pos = 0;
		}

		wmove(this->window, cursor_row, cursor_col+cursor_pos);
		int input = wgetch(this->window);

		if(input == KEY_RESIZE)
		{
			input_window_resize(this);
			--cursor_pos;
		}
		else if(input >= 32 && input <= 126) // ASCII text
		{
			buffer[cursor_pos] = input;
			mvwprintw(this->window, cursor_row, cursor_col+cursor_pos, (const char*)&input);
			++total;
		}
		else if(input == KEY_BACKSPACE) // Backspace
		{
			cursor_pos = cursor_pos == 0 ? 1 : cursor_pos;
			buffer[cursor_pos-1] = 0;

			input = 32;
			mvwprintw(this->window, cursor_row, cursor_col+cursor_pos-1, (const char*)&input);
			cursor_pos = cursor_pos-2 < 0 ? -1 : cursor_pos-2;
			total = total-1 < 0 ? 0 : total-1;
		}
		else if(input == 10 || input == 9) // Enter || tab
		{

			if(buffer[cursor_pos] != 0)
			{
				int buf_len = strlen(buffer);
				if(buf_len < buffer_size-1)
					buffer[buf_len] = 0;
				else
					buffer[buffer_size-1] = 0;
			}
			else
				buffer[cursor_pos] = 0;

			break;
		}
		else if(input == KEY_LEFT)
		{
			int new_pos = cursor_pos - 2 ; 
			cursor_pos = new_pos < 0 ? -1 : new_pos;
		}
		else if(input == KEY_DOWN)
			break;
		else if(input == 353 || input == KEY_UP) // shift + tab
			return true;
		else if(input == 360) // End key
			cursor_pos = strlen(buffer) - 1;
		else if(input == 262) // Home key
			cursor_pos = -1;

		++cursor_pos;
	}
	curs_set(0);
	return false;
}

void input_window_resize(input_window_t * this)
{
	interface_resize_windows(this->interface);

	wclear(this->window);

	this->rows = this->interface->y_max;
	this->cols = this->interface->x_max;

	this->window_rows = CALCULATE_USAGE(this->rows);
	this->window_cols = CALCULATE_USAGE(this->cols);

	wresize(this->window, this->window_rows, this->window_cols);

	mvwin(this->window, CALCULATE_COORDS(this->rows, this->window_rows), CALCULATE_COORDS(this->cols, this->window_cols));
	input_window_init_text(this);

	mvwprintw(this->window, 1, TEXT_SIZE(this->tab_name)+1, this->tab_name);
	mvwprintw(this->window, 3, TEXT_SIZE(this->file_name)+1, this->file_name);
	mvwprintw(this->window, 5, TEXT_SIZE(this->regex)+1, this->regex);

	wrefresh(this->window);
}

int input_window_get_size_by_type(input_window_t * this, char * buffer, int type)
{
	if(type == BUFFER)
	{
		if (buffer == this->tab_name)
			return MAX_TAB_NAME;
		else if (buffer == this->file_name)
			return MAX_FILE_NAME;
		else if(buffer == this->regex)
			return MAX_REGEX;
	}
	else if(type == TEXT)
	{
		if (buffer == this->tab_name)
			return strlen("Tab name: ");
		else if (buffer == this->file_name)
			return strlen("File name: ");
		else if(buffer == this->regex)
			return strlen("Regex: ");
	}

	return 0;
}