#ifndef WINDOW_BUILDER_H
#define WINDOW_BUILDER_H

#include <ncurses.h>

WINDOW * win_builder_newwin(size_t rows, size_t cols,
                            size_t row_start, size_t col_start);

WINDOW * win_builder_set_box(WINDOW * window);

WINDOW * win_builder_set_title(WINDOW * window, char * title,
                               size_t position, size_t screen_cols);

WINDOW * win_builder_refresh(WINDOW * window);

#endif