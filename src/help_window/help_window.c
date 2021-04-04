#include "help_window/help_window.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

const char HELP_WINDOW_TEXTS[HELP_OPTIONS_COUNT][MAX_HELP_TEXT_SIZE] =
{
    "ctrl + o: open file",
    "ctrl + e: exit",
    "F5: refresh",
    "R: refresh all",
    "Left arrow: left tab",
    "Right arrow: right tab",
    "Up arrow: scroll up",
    "Down arrow: scroll down",
    "End: end of file",
    "c: toggle color",
    "r: toggle auto refresh",
    "q: exit this help",
};

help_window_t * help_window_create(size_t rows, size_t cols, size_t start_row, size_t start_col)
{
    help_window_t * window = malloc(sizeof(help_window_t));
    help_window_init(window, rows, cols, start_row, start_col);
    return window;
}

void help_window_destroy(help_window_t * window)
{
    delwin(window->window);
    free(window);
}

void help_window_init(help_window_t * window, size_t rows, size_t cols, size_t start_row, size_t start_col)
{
    window->rows = rows;
    window->cols = cols;
    window->start_row = start_row;
    window->start_col = start_col;

    window->window = newwin(rows, cols, start_row, start_col);
    keypad(window->window, TRUE);
}

void help_window_show(help_window_t * window)
{
    box(window->window, 0, 0);

    wattron(window->window, COLOR_PAIR(HIGHLIGHT_WHITE));
    mvwprintw(window->window, 0, CENTER_TEXT(window->cols, "Help"), "Help");
    wattroff(window->window, COLOR_PAIR(HIGHLIGHT_WHITE));

    for(size_t text = 0; text < HELP_OPTIONS_COUNT; ++text)
    {
        mvwprintw(window->window, text + 1, 1, HELP_WINDOW_TEXTS[text]);
    }

    wrefresh(window->window);
}

void help_window_listen_keys(help_window_t * window)
{
    //Create input engine that every window asks for something
    while(true)
    {
        unsigned int input = wgetch(window->window);

        mvwprintw(window->window, 0, 0, "%3d", input);

        if(input == 'q')
            break;
    }

    keypad(window->window, FALSE);
}