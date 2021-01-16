#include "tab.h"
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
    if(tab) 
        bzero(tab, sizeof(tab_t));
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

void tab_print(tab_t * tab, bool color, FILE * file)
{
    wmove(tab->window, 0, 0);

    char buffer[tab->cols];
	for(size_t line = 0; line < tab->rows; ++line)
	{
	 	fgets(buffer, tab->cols, file);
		buffer[ strlen(buffer)+1 ] = 0;

		if(strncmp(buffer, "INFO", 4) == 0 && color)
		{
			wattron(tab->window, COLOR_PAIR(HIGHLIGHT_YEL));
			wprintw(tab->window, "%s", buffer);
			wattroff(tab->window, COLOR_PAIR(HIGHLIGHT_YEL));
		}
		else if(strncmp(buffer, "ERROR", 5) == 0 && color)
		{
			wattron(tab->window, COLOR_PAIR(HIGHLIGHT_ERROR));
			wprintw(tab->window, "%s", buffer);
			wattroff(tab->window, COLOR_PAIR(HIGHLIGHT_ERROR));	
		}
		else
			wprintw(tab->window, "%s", buffer);

		memset(buffer, 0, tab->cols);
	}
}