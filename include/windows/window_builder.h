#ifndef WINDOW_BUILDER_H
#define WINDOW_BUILDER_H

#include <ncurses.h>

#define WIN_DEFAULT_DELAY 1000

typedef struct win_builder_data {
    size_t rows;
    size_t cols;
    size_t row_start;
    size_t col_start;
    size_t delay;
    size_t screen_cols;
    size_t position;
    char * title;
    bool box;
} win_builder_data_t;

WINDOW * win_builder_create(win_builder_data_t * data);

WINDOW * win_builder_newwin(win_builder_data_t * data);

WINDOW * win_builder_set_box(WINDOW * window);

WINDOW * win_builder_set_title(WINDOW * window, win_builder_data_t * data);

WINDOW * win_builder_refresh(WINDOW * window);

WINDOW * win_builder_set_timeout(WINDOW * window, win_builder_data_t * data);

#endif