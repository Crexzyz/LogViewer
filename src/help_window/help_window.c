#include "help_window/help_window.h"

#include <stdlib.h>

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
    box(window->window, 0, 0);
}

void help_window_show(help_window_t * window)
{
    wrefresh(window->window);
}

void help_window_listen_keys(help_window_t * window)
{

}