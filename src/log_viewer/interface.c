#include "log_viewer/interface.h"
#include "windows/help_window.h"
#include "windows/window_builder.h"
#include "utils.h"

#include <string.h>
#include <time.h>

const char INTERFACE_SYMBOLS[STATUS_SYMBOLS][STATUS_SYMBOLS_SIZE] = 
{
    "C\0", "R\0"
};

interface_t * interface_create()
{
    interface_t * this = malloc(sizeof(interface_t));
    return this;
}

void interface_destroy(interface_t * this)
{
    if(this)
    {
        delwin(this->tabs_window);
        delwin(this->help_window);	
        tab_manager_destroy(this->tab_manager);
        free(this);
    }
}

void interface_init(interface_t * this)
{
    bzero(this, sizeof(interface_t));

    this->color = true;
    this->auto_refresh = true;

    init_color(COLOR_BLACK, 0,0,0);
    init_pair(HIGHLIGHT_ERROR, COLOR_WHITE, COLOR_RED);
    init_pair(HIGHLIGHT_WHITE, COLOR_BLACK, COLOR_WHITE);
    init_pair(HIGHLIGHT_YEL, COLOR_BLACK, COLOR_YELLOW);
    init_pair(HIGHLIGHT_BLACK, COLOR_WHITE, COLOR_BLACK);
    init_pair(HIGHLIGHT_CYAN, COLOR_BLACK, COLOR_CYAN);

    this->context = context_create();
    context_set_dimensions(this->context);

    this->tab_manager = tab_manager_create(this->context);

    this->tabs_window = win_builder_set_title(
        win_builder_set_box(
            win_builder_newwin(this->context->screen_rows - HELP_TAB_SIZE,
                               this->context->screen_cols, 0, 0)
        ),
        "Log Viewer", CENTER, this->context->screen_cols
    );

    this->help_window = win_builder_newwin(HELP_TAB_SIZE, this->context->screen_cols,
                                           this->context->screen_rows - HELP_TAB_SIZE, 0);
}

WINDOW * interface_new_boxed_window(int row_size, int col_size, int y_start, int x_start, char * title, int position)
{
    WINDOW * win = newwin(row_size, col_size, y_start, x_start);
    interface_draw_borders(win, title, position, col_size, true);
    return win;
}

void interface_draw_borders(WINDOW * win, char * title, int position, int col_size, bool draw_box)
{
    if(draw_box)
        box(win, 0, 0);

    if(title != NULL)
    {
        if(position == LEFT)
            mvwprintw(win, 0, 1, title);
        else if(position == CENTER)
        {
            wattron(win, COLOR_PAIR(HIGHLIGHT_WHITE));
            mvwprintw(win, 0, CENTER_TEXT(col_size, title), title);
            wattroff(win, COLOR_PAIR(HIGHLIGHT_WHITE));
        }
        else if(position == RIGHT)
            mvwprintw(win, 0, RIGHT_TEXT(col_size, title), title);
    }
}

WINDOW * interface_new_window(int row_size, int col_size, int y_start, int x_start)
{
    return newwin(row_size, col_size, y_start, x_start);
}

void interface_refresh_all(interface_t * this)
{
    wrefresh(this->tabs_window);
    wrefresh(this->help_window);
    refresh();
}

void interface_refresh_status_bar(interface_t * this)
{
    bool status[2] = {this->color, this->auto_refresh};

    for(size_t symbol = 0; symbol < STATUS_SYMBOLS; ++symbol)
    {
        if(status[symbol])
            wattron(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));

        mvwprintw(this->help_window, 0, symbol, INTERFACE_SYMBOLS[symbol]);

        wattroff(this->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));	
    }

    wrefresh(this->help_window);
}

void interface_resize_windows(interface_t * this)
{
    // Get new dimensions
    context_set_dimensions(this->context);

    // Resize main window
    interface_resize_window(this->tabs_window, "Log Viewer", CENTER,
                            this->context->screen_rows - HELP_TAB_SIZE,
                            this->context->screen_cols, true);

    // Resize help window
    mvwin(this->help_window, this->context->screen_rows - HELP_TAB_SIZE, 0);
    interface_resize_window(this->help_window, 0, 0, HELP_TAB_SIZE,
                            this->context->screen_cols, false);
}

void interface_resize_window(WINDOW * window, char * title, int position, int lines, int columns, bool draw_box)
{
    if(!window)
        return;

    wclear(window);
    wresize(window, lines, columns);

    if(draw_box)
        win_builder_set_box(window);

    if(title)
        win_builder_set_title(window, title, position, columns);

    win_builder_refresh(window);
}

void interface_run(interface_t * iface)
{
    while(true)
    {
        tab_manager_print_tabs(iface->tab_manager, iface->tabs_window);
        tab_manager_print_active(iface->tab_manager, iface->tabs_window);
        interface_refresh_status_bar(iface);

        size_t input = wgetch(iface->tabs_window);
        size_t opcode = interface_handle_input(iface, input);
        mvwprintw(iface->help_window, 0,
                  iface->context->screen_cols - 4, "%3d", input);
        wrefresh(iface->help_window);

        if(opcode == IFACE_EXIT)
        {
            break;
        }
        else if(opcode == IFACE_RESIZE)
        {
            interface_resize_windows(iface);
        }
        else if(opcode == IFACE_NOOP)
        {
            tab_manager_handle_input(iface->tab_manager, input);
        }
    }
}

size_t interface_handle_input(interface_t * interface, size_t input)
{
    if(input == 5) // ctrl + e
    {
        return IFACE_EXIT;
    }
    else if (input == KEY_RESIZE)
    {
        return IFACE_RESIZE;
    }
    else if(input == 15) // ctrl + o
    {
        tab_manager_add_tab_popup(interface->tab_manager, interface->tabs_window);
        return IFACE_TAB_ADDED;
    }
    else if(input == 8) // ctrl + h
    {
        interface_open_help(interface);
        return IFACE_SKIP_TAB_MGR;
    }
    else if(input == 'c')
    {
        interface_toggle_color(interface);
        return IFACE_SKIP_TAB_MGR;
    }
    else if(input == 'r')
    {
        interface_toggle_autorefresh(interface);
        return IFACE_SKIP_TAB_MGR;
    }

    return IFACE_NOOP;
}

void interface_toggle_color(interface_t * iface)
{
    iface->color = !iface->color;
}

void interface_toggle_autorefresh(interface_t * iface)
{
    iface->auto_refresh = !iface->auto_refresh;
}

void interface_open_help(interface_t * interface)
{
    help_window_t * hw = help_window_create(interface->context->screen_rows,
                                            interface->context->screen_cols,
                                            0, 0);

    help_window_show(hw);
    help_window_listen_keys(hw);
    wclear(hw->window);
    wrefresh(hw->window);
    help_window_destroy(hw);

    interface_draw_borders(interface->tabs_window, "Log Viewer",
                           CENTER, interface->context->screen_cols,
                           true);

    wrefresh(interface->tabs_window);
}

int interface_process_auto_refresh(interface_t * this, bool resized)
{
    if (this->auto_refresh)
    {
        if(!resized)
        {
            fd_set fds;
            int maxfd = 0;

            // Set a 1 second timer
            struct timeval timer;
            timer.tv_sec = 1;
            // Clean fd set
               FD_ZERO(&fds);
               // Add stdin to set
            FD_SET(0, &fds); 
            // Wait 1 second for user input
            select(maxfd+1, &fds, NULL, NULL, &timer); 
            // (Conditional not really necessary) If stdin is in set, update current tab
            if (!FD_ISSET(0, &fds))
            {
                if(this->tab_manager->tab_amount > 0)
                    tab_manager_refresh_tab(this->tab_manager, this->color);
            }

            return 0;
        }
    }

    return 1;
}

int interface_process_tab_options(interface_t * this, int input, int row)
{
    if(this->tab_manager->tab_amount > 0)
    {
        tab_manager_t * tm = this->tab_manager;
        tab_t * active_tab = tm->tabs[tm->active_tab];

        if(input == KEY_RIGHT)
        {
            tm->active_tab = (tm->active_tab + 1) % tm->tab_amount;
            tab_manager_print_tabs(this->tab_manager, this->tabs_window);
        }
        else if(input == KEY_LEFT)
        {
            tm->active_tab = tm->active_tab == 0 ? tm->tab_amount - 1 : tm->active_tab - 1; 
            tab_manager_print_tabs(this->tab_manager, this->tabs_window);	
        }
        else if(input == KEY_UP)
        {
            row = row-1 < 0 ? -1 : row-1;
            active_tab->last_row = row;
        }
        else if(input == 269) // F5
        {
            tab_manager_refresh_tab(this->tab_manager, this->color);
            wrefresh(active_tab->window);
        }
        else if(input == 'R') // shift R
            tab_manager_refresh_all_tabs(this->tab_manager, this->color);
        else if(input == 360) // end
            active_tab->last_row = active_tab->rows - this->context->screen_rows+2+HELP_TAB_SIZE;
        else
        {
            ++row;
            active_tab->last_row = row;
        }
    }
    return row;
}