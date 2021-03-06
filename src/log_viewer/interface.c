#include "log_viewer/interface.h"
#include "windows/help_window.h"
#include "windows/window_builder.h"
#include "utils/utils.h"

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
        context_destroy(this->context);
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

    win_builder_data_t tabs_data = {
        .rows = this->context->screen_rows - HELP_TAB_SIZE,
        .cols = this->context->screen_cols,
        .box = true,
        .title = IFACE_TITLE,
        .position = CENTER,
        .screen_cols = this->context->screen_cols,
        .delay = WIN_DEFAULT_DELAY
    };

    this->tabs_window = win_builder_create(&tabs_data);

    win_builder_data_t help_data = {
        .rows = HELP_TAB_SIZE,
        .cols = this->context->screen_cols,
        .row_start = this->context->screen_rows - HELP_TAB_SIZE,
        .col_start = 0,
        .screen_cols = this->context->screen_cols,
    };

    this->help_window = win_builder_create(&help_data);
}

void interface_refresh_status_bar(interface_t * iface)
{
    bool status[2] = {
        tab_manager_get_color(iface->tab_manager),
        iface->auto_refresh
    };

    for(size_t symbol = 0; symbol < STATUS_SYMBOLS; ++symbol)
    {
        if(status[symbol])
            wattron(iface->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));

        mvwprintw(iface->help_window, 0, symbol, INTERFACE_SYMBOLS[symbol]);

        wattroff(iface->help_window, COLOR_PAIR(HIGHLIGHT_WHITE));	
    }

    wrefresh(iface->help_window);
}

void interface_resize_windows(interface_t * this)
{
    // Get new dimensions
    context_set_dimensions(this->context);

    // Resize main window
    interface_resize_window(this->tabs_window, IFACE_TITLE, CENTER,
                            this->context->screen_rows - HELP_TAB_SIZE,
                            this->context->screen_cols, true);

    // Resize help window
    mvwin(this->help_window, this->context->screen_rows - HELP_TAB_SIZE, 0);
    interface_resize_window(this->help_window, 0, 0, HELP_TAB_SIZE,
                            this->context->screen_cols, false);

    tab_manager_resize(this->tab_manager);
}

void interface_resize_window(WINDOW * window, char * title, int position, int lines, int columns, bool draw_box)
{
    if(!window)
        return;

    wclear(window);
    wresize(window, lines, columns);
    
    win_builder_data_t title_data = {
        .title = title,
        .position = position,
        .screen_cols = columns
    };

    if(draw_box)
        win_builder_set_box(window);

    if(title)
        win_builder_set_title(window, &title_data);

    win_builder_refresh(window);
}

void interface_run(interface_t * iface)
{
    // First-time print to avoid empty screen before first delay
    tab_manager_print_tabs(iface->tab_manager, iface->tabs_window);
    tab_manager_print_active(iface->tab_manager, iface->tabs_window);

    while(true)
    {
        interface_refresh_status_bar(iface);

        size_t input = wgetch(iface->tabs_window);
        size_t opcode = interface_handle_input(iface, input);
        mvwprintw(iface->help_window, 0, iface->context->screen_cols - 3,
                  "%d", input);
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
            // It is needed to refresh the file when moving between tabs
            tab_manager_print_tabs(iface->tab_manager, iface->tabs_window);
            tab_manager_print_active(iface->tab_manager, iface->tabs_window);
            continue;
        }

        if(opcode == IFACE_TAB_CLOSED || iface->auto_refresh)
        {
            tab_manager_print_tabs(iface->tab_manager, iface->tabs_window);

            if(iface->tab_manager->tab_list->size == 0)
                interface_clear_content(iface);

            tab_manager_print_active(iface->tab_manager, iface->tabs_window);
        }
    }
}

size_t interface_handle_input(interface_t * interface, size_t input)
{
    if(input == 5) // ctrl + e
    {
        return IFACE_EXIT;
    }
    else if((int)input == -1) // timeout
    {
        return IFACE_TIMEOUT;
    }
    else if (input == KEY_RESIZE)
    {
        return IFACE_RESIZE;
    }
    else if(input == 15) // ctrl + o
    {
        tab_manager_add_tab_popup(interface->tab_manager);
        return IFACE_TAB_ADDED;
    }
    else if(input == 23) // ctrl + w
    {
        tab_manager_close_tab(interface->tab_manager);
        return IFACE_TAB_CLOSED;
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
    if(!iface)
        return;

    tab_manager_toggle_color(iface->tab_manager);
}

void interface_toggle_autorefresh(interface_t * iface)
{
    if(!iface)
        return;

    iface->auto_refresh = !iface->auto_refresh;

    win_builder_data_t data = { 
        .delay = iface->auto_refresh ? WIN_DEFAULT_DELAY : 0
    };

    win_builder_set_timeout(iface->tabs_window, &data);
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

    win_builder_data_t title_data = {
        .title = IFACE_TITLE,
        .position = CENTER,
        .screen_cols = interface->context->screen_cols,
    };

    win_builder_set_box(interface->tabs_window);
    win_builder_set_title(interface->tabs_window, &title_data);

    wrefresh(interface->tabs_window);
}

void interface_clear_content(interface_t * iface)
{
    for(size_t i = 1; i < iface->context->screen_rows - 2; ++i)
    {
        mvwprintw(iface->tabs_window, i, 1, "%*s",
                  iface->context->screen_cols - 2, "");
    }

    wrefresh(iface->tabs_window);
}
