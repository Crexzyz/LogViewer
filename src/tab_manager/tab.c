#include "tab_manager/tab.h"
#include "utils.h"

#include <stdlib.h>

tab_t * tab_create()
{
    tab_t * tab = malloc(sizeof(tab_t));
    tab_init(tab);
    return tab;
}

void tab_init(tab_t * tab)
{
    if(!tab)
        return;

    bzero(tab, sizeof(tab_t));
    tab->color = true;
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

int tab_get_line_color(char * line)
{
    if(strncmp(line, TAB_KEYWORD_INFO, strlen(TAB_KEYWORD_INFO)) == 0)
        return COLOR_PAIR(HIGHLIGHT_YEL);
    else if(strncmp(line, TAB_KEYWORD_ERROR, strlen(TAB_KEYWORD_ERROR)) == 0)
        return COLOR_PAIR(HIGHLIGHT_ERROR);

    return HIGHLIGHT_NONE;
}

void tab_print(tab_t * tab, FILE * file)
{
    wmove(tab->window, 0, 0);

    char buffer[tab->cols];
	for(size_t line = 0; line < tab->rows; ++line)
	{
        memset(buffer, 0, tab->cols);
	 	fgets(buffer, tab->cols, file);

        int color_code = tab_get_line_color(buffer);
        if(!tab->color || color_code == HIGHLIGHT_NONE)
        {
            wprintw(tab->window, "%s", buffer);
            continue;
        }

        wattron(tab->window, color_code);
        wprintw(tab->window, "%s", buffer);
        wattroff(tab->window, color_code);
	}
}

void tab_toggle_color(tab_t * tab)
{
    if(!tab)
        return;

    tab->color = !tab->color;
}
