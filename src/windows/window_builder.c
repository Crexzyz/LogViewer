#include "windows/window_builder.h"
#include "utils/utils.h"

#include <string.h>

WINDOW * win_builder_create(win_builder_data_t * data)
{
    WINDOW * win = newwin(data->rows, data->cols,
                          data->row_start, data->col_start);
    
    if(data->box)
        win_builder_set_box(win);

    if(data->title)
        win_builder_set_title(win, data);

    if(data->delay > 0)
        win_builder_set_timeout(win, data);

    return win;
}

WINDOW * win_builder_newwin(win_builder_data_t * data)
{
    return newwin(data->rows, data->cols, 
                  data->row_start, data->col_start);
}

WINDOW * win_builder_set_box(WINDOW * window)
{
    box(window, 0, 0);
    return window;
}

WINDOW * win_builder_set_title(WINDOW * window, win_builder_data_t * data)
{
    wattron(window, COLOR_PAIR(HIGHLIGHT_WHITE));
    switch (data->position)
    {
        case RIGHT:
            mvwprintw(window, 0, RIGHT_TEXT(data->screen_cols, data->title),
                      data->title);
            break;
        case CENTER:
            mvwprintw(window, 0, CENTER_TEXT(data->screen_cols, data->title),
                      data->title);
            break;
        default:
            mvwprintw(window, 0, 1, data->title);
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

WINDOW * win_builder_set_timeout(WINDOW * window, win_builder_data_t * data)
{
    if(!window)
        return NULL;

    if(data->delay > 0)
    {
        notimeout(window, false);
        wtimeout(window, data->delay);
    }
    else
    {
        notimeout(window, true);
    }

    return window;
}