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

void input_window_init(input_window_t * this, int rows, int cols)
{
	this->rows = rows;
	this->cols = cols;

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
	// Turn on cursor
	curs_set(1);
	// Enable keypad on input window
	keypad(this->window, TRUE);
	// Cursor's starting column
	const int cursor_scol = cursor_col;
	// Cursor's column position from start
	int cursor_cpos = 0;
	// Cursor's starting row
	const int cursor_srow = cursor_row;
	// Cursor's row position from start
	int cursor_rpos = 0;
	// Buffer's used size
	int used_size = strlen(buffer);
	// Get buffer size based on the pointer
	int buffer_size = input_window_get_size_by_type(this, buffer, BUFFER);
	// Buffer index
	int buffer_index = 0;
	// When cursor movement is not needed
	bool move_cursor = true;

	while(true)
	{
		move_cursor = true;
		// Place cursor in current position
		wmove(this->window, cursor_row+cursor_rpos, cursor_col+cursor_cpos);
		int input = wgetch(this->window);

		if(input == KEY_RESIZE)
		{
			// FIX RESIZING
			input_window_resize(this, this->window_rows, this->window_cols);
			move_cursor = false; // Do not move cursor
		}
		else if(input >= 32 && input <= 126) // ASCII text, "standard" input
		{
			if(buffer[buffer_index] == 0)
				++used_size;

			buffer[buffer_index] = input;
			mvwprintw(this->window, cursor_row+cursor_rpos, cursor_col+cursor_cpos, (const char*)&input);
			if(used_size < buffer_size)
				++buffer_index;
		}
		else if(input == 10 || input == 9) // Enter || tab
		{
			if(buffer[buffer_index] != 0)
			{
				int buf_len = strlen(buffer);
				if(buf_len < buffer_size-1)
					buffer[buf_len] = 0;
				else
					buffer[buffer_size-1] = 0;
			}
			else
				buffer[buffer_index] = 0;

			break;
		}
		else if(input == KEY_BACKSPACE) // Backspace
		{
			buffer_index = buffer_index > 0 ? buffer_index-1 : 0;
			if(buffer[buffer_index] != 0)
				used_size = used_size-1 < 0 ? 0 : used_size-1;
			buffer[buffer_index] = 0;

			--cursor_cpos;

			input = 32; // input = (ascii) blank space
			if(cursor_cpos < 0 && cursor_rpos > 0)
				mvwprintw(this->window, cursor_row+cursor_rpos-1, this->window_cols-2, (const char*)&input);
			else
				mvwprintw(this->window, cursor_row+cursor_rpos, cursor_col+cursor_cpos, (const char*)&input);

			move_cursor = false;
		}
		else if(input == KEY_LEFT)
		{
			buffer_index = buffer_index > 0 ? buffer_index-1 : 0;
			--cursor_cpos;
			move_cursor = false;
		}
		else if(input == 360) // End key
		{
			buffer_index = cursor_cpos = strlen(buffer)-1;
		} 
		else if(input == 262) // Home key, go to the first char
		{
			cursor_col = cursor_scol;
			cursor_row = cursor_srow;
			buffer_index = cursor_cpos = cursor_rpos = 0;
			move_cursor = false;
		}
		else if(input == KEY_RIGHT)
		{
			++cursor_cpos;
			if(buffer_index < buffer_size)
				++buffer_index;
			move_cursor = false;
		}
		else if(input == KEY_DOWN)
			break;
		else if(input == 353 || input == KEY_UP) // shift + tab
			return true;


		if(move_cursor)
			++cursor_cpos; // Move cursor to the left

		// If cursor reaches the end of the window
		if(cursor_col+cursor_cpos >= this->window_cols-1)
		{
			// Move cursor
			cursor_cpos = (cursor_col+cursor_cpos) - (this->window_cols-1);
			// Cursor starts now in a blank line
			cursor_col = 1;
			// Move cursor down
			++cursor_rpos;
		}

		// If cursor is behind the correct position, move it one row up
		if(cursor_cpos < 0)
		{
			if(cursor_rpos == 0) // Trivial case, there is no input
				cursor_cpos = 0;
			else // We need to move the cursor up
			{
				--cursor_rpos;

				cursor_col = cursor_rpos == 0 ? cursor_scol : cursor_col-1;
				cursor_cpos = this->window_cols-2 - cursor_col;
			}
		}

	} 

	curs_set(0);
	return false;
}

void input_window_resize(input_window_t * this, size_t max_rows, size_t max_cols)
{
	wclear(this->window);

	this->rows = max_rows;
	this->cols = max_cols;

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