#include "tab_manager/tab.h"
#include "utils/utils.h"
#include "utils/file_ops.h"

#include <stdlib.h>

tab_t * tab_create(char * name, char * file, char * regex, size_t cols,
                   size_t rows)
{
    tab_t * tab = malloc(sizeof(tab_t));
    tab_init(tab, name, file, regex, cols, rows);
    return tab;
}

void tab_init(tab_t * tab, char * name, char * file, char * regex,
              size_t cols, size_t rows)
{
    if(!tab)
        return;

    bzero(tab, sizeof(tab_t));

    if(!name || strnlen(name, TAB_MAX_FILE_NAME) == 0)
        name = "<No name>";
    else
        tab_set_name(tab, name);

    tab_set_file_name(tab, file);

    if(regex)
        tab_set_regex(tab, regex);

    tab->cols = cols;
    tab->rows = rows;
    tab->color = true;

    tab->window = newwin(rows, cols, 2, 1);
    keypad(tab->window, TRUE);
}

void tab_destroy(tab_t * tab)
{
    if(tab)
    {
        if(tab->window)
            delwin(tab->window);

        free(tab);
    }
}

void tab_add_pad(tab_t * tab, size_t cols, size_t rows)
{
    if(tab)
    {
        tab->cols = cols;
        tab->rows = rows;

        tab->window = newpad(rows, cols);
        keypad(tab->window, TRUE);
        scrollok(tab->window, TRUE);
    }
}

int tab_get_line_color(tab_t * tab, char * line)
{
    if(!tab->color)
        return HIGHLIGHT_NONE;

    if(strncmp(line, TAB_KEYWORD_INFO, strlen(TAB_KEYWORD_INFO)) == 0)
        return COLOR_PAIR(HIGHLIGHT_YEL);
    else if(strncmp(line, TAB_KEYWORD_ERROR, strlen(TAB_KEYWORD_ERROR)) == 0)
        return COLOR_PAIR(HIGHLIGHT_ERROR);

    return HIGHLIGHT_NONE;
}

void tab_print(tab_t * tab)
{
    FILE * file = file_ops_open_at_line(tab->file, tab->curr_row);
    if(!file)
        return; // Print file error

    // Move cursor to the window's start
    wmove(tab->window, 0, 0);
    char buffer[FILE_OPS_BUFF_SIZE];

	for(size_t line = 0; line < tab->rows; ++line)
	{
        memset(buffer, 0, FILE_OPS_BUFF_SIZE);
	 	int status = file_ops_get_line(file, tab->regex, buffer);

        if(status == FILE_OPS_EOF)
        {
            wprintw(tab->window, "%s\n", "~");
            continue;
        }

        int color_code = tab_get_line_color(tab, buffer);
        if(color_code != HIGHLIGHT_NONE)
            wattron(tab->window, color_code);

        wprintw(tab->window, "%s", buffer);
        wattroff(tab->window, color_code);
	}

    fclose(file);
}

void tab_toggle_color(tab_t * tab)
{
    if(!tab)
        return;

    tab->color = !tab->color;
}
