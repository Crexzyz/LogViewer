#ifndef HELP_WINDOW_H
#define HELP_WINDOW_H

#include <ncurses.h>

typedef struct help_window 
{
    /// ncurses window
    WINDOW * window;

    size_t cols;
    size_t rows;

    size_t start_col;
    size_t start_row;

} help_window_t;

#define HELP_OPTIONS_COUNT 11 
#define MAX_HELP_TEXT_SIZE 30

help_window_t * help_window_create(size_t rows, size_t cols, size_t start_row, size_t start_col);
void help_window_destroy(help_window_t * window);
void help_window_init(help_window_t * window, size_t rows, size_t cols, size_t start_row, size_t start_col);
void help_window_show(help_window_t * window);
void help_window_listen_keys(help_window_t * window);

#endif