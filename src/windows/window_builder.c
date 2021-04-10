#include "windows/window_builder.h"
#include "utils.h"

#include <string.h>

WINDOW * win_builder_newwin(size_t rows, size_t cols, size_t row_start, size_t col_start)
{
    return newwin(rows, cols, row_start, col_start);
}

WINDOW * win_builder_set_box(WINDOW * window)
{
    box(window, 0, 0);
    return window;
}

WINDOW * win_builder_set_title(WINDOW * window, char * title, size_t position, size_t screen_cols)
{
    wattron(window, COLOR_PAIR(HIGHLIGHT_WHITE));
    switch (position)
    {
        case RIGHT:
            mvwprintw(window, 0, RIGHT_TEXT(screen_cols, title), title);
            break;
        case CENTER:
            mvwprintw(window, 0, CENTER_TEXT(screen_cols, title), title);
            break;
        default:
            mvwprintw(window, 0, 1, title);
            break;
    }
    wattroff(window, COLOR_PAIR(HIGHLIGHT_WHITE));
    return window;
}

WINDOW * win_builder_refresh(WINDOW * window)
{
    wrefresh(window);
    return window;
}

WINDOW * win_builder_set_timeout(WINDOW * window, int delay, bool enabled)
{
    if(!window)
        return NULL;

    if(enabled)
    {
        notimeout(window, false);
        wtimeout(window, delay);
    }
    else
    {
        notimeout(window, true);
    }

    return window;
}