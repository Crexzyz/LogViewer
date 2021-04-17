#include "tab_manager/tab.h"
#include "tab_manager/tab_manager.h"
#include "windows/input_window.h"
#include "utils/utils.h"

#include <string.h>

tab_manager_t * tab_manager_create(context_t * context)
{
    tab_manager_t * tm = malloc(sizeof(tab_manager_t));
    tab_manager_init(tm, context);
    return tm;
}

void tab_manager_init(tab_manager_t * tm, context_t * context)
{
    if(tm)
    {
        bzero(tm, sizeof(tab_manager_t));
        tm->context = context;
        tm->tab_list = list_create();
    }
}

void tab_manager_destroy(tab_manager_t * tm)
{
    if(tm)
    {
        list_for_each(tm->tab_list, tab_destroy_contents_cast);
        list_destroy(tm->tab_list);
        free(tm);
    }
}

void tab_manager_handle_input(tab_manager_t * tm, size_t input)
{
    if(!tm)
        return;

    if(tab_manager_tabs_amount(tm) == 0)
        return;

    tab_t * curr_tab = tab_manager_get_active_tab(tm);

    if(!curr_tab)
        return;

    if(input == KEY_RIGHT || input == 67)
    {
        tm->active_tab = (tm->active_tab + 1) % tm->tab_list->size;
    }
    else if(input == KEY_LEFT || input == 68)
    {
        tm->active_tab = tm->active_tab == 0 ? tm->tab_list->size - 1 : tm->active_tab - 1;
    }
    else if(input == KEY_UP || input == 65)
    {
        if(curr_tab->curr_row != 0)
            curr_tab->curr_row -= 1;
    }
    else if(input == KEY_DOWN || input == 66)
    {
        if(curr_tab->last_row > 1)
            curr_tab->curr_row += 1;
    }
    else if(input == 'R')
    {
        tab_print(tab_manager_get_active_tab(tm));
    }
    else if(input == 18) // ctrl + r
    {
        tab_manager_refresh_all_tabs(tm);
    }
    else if(input == 360 || input == 70) // end
    {
        curr_tab->curr_row = curr_tab->rows - tm->context->screen_rows
                             + HELP_TAB_SIZE + 3;
    }
}

tab_t * tab_manager_get_active_tab(tab_manager_t * tm)
{
    if(!tm)
        return NULL;
    
    list_node_t * tab_node = list_get_at_index(tm->tab_list, tm->active_tab);

    if(tab_node)
        return tab_node->data;

    return NULL;
}

void tab_manager_print_active(tab_manager_t * tm, WINDOW * target_window)
{
    if(!tm || !target_window)
        return;

    if(tm->tab_list->size == 0)
        return;

    tab_t * tab = tab_manager_get_active_tab(tm);

    if(!tab)
        return;

    tab_print(tab);
    wrefresh(tab->window);
}

void tab_manager_print_tabs(tab_manager_t * this, WINDOW * tabs_window)
{
    // Clear closed tabs indicator
    if(this->tab_list->size == 0)
    {
        for(size_t i = 1; i < this->context->screen_cols - 1 ; ++i )
            mvwprintw(tabs_window, 1, i, " ");
        return;
    }

    // If first time updating limits
    if (this->tab_display_start != 0 && this->tab_display_end != 0)
        tab_manager_update_limits(this);
    
    const size_t printable_chars = this->context->screen_cols - 2;
    size_t print_index = 1;

    // Actual printing
    size_t tab = 0;
    list_node_t * tab_node = list_get_at_index(this->tab_list, this->tab_display_start);
    for(tab = this->tab_display_start; tab < tab_manager_tabs_amount(this); ++tab)
    {
        tab_t * curr_tab = tab_node->data;
        size_t color = this->active_tab == tab ? COLOR_PAIR(HIGHLIGHT_CYAN)
                                               : COLOR_PAIR(HIGHLIGHT_WHITE);

        size_t tab_name_size = strnlen(curr_tab->name, TAB_MAX_TAB_NAME);

        if(print_index + tab_name_size <= printable_chars )
        {
            wattron(tabs_window, color);
            mvwprintw(tabs_window, 1, print_index, "%s ", curr_tab->name);
            wattroff(tabs_window, color);
        }
        else
        {
            break; // No more space to print
        }

        print_index += tab_name_size + 1;
        tab_node = tab_node->next;
    }

    this->tab_display_end = tab;

    // Clean unused tab space
    for(size_t leftover = print_index; leftover < this->context->screen_cols - 1; ++leftover )
        mvwprintw(tabs_window, 1, leftover, " ");
        
    wrefresh(tabs_window);
}

void tab_manager_update_limits(tab_manager_t * this)
{
    if(this->active_tab < this->tab_display_start)
    {
        this->tab_display_start = this->active_tab;
        this->tab_display_end -= 1;
    }
    else if (this->active_tab >= this->tab_display_end)
    {
        this->tab_display_start += (this->active_tab - this->tab_display_end) + 1;
        this->tab_display_end = this->active_tab + 1;
    }
}

void tab_manager_add_tab_popup(tab_manager_t * this)
{
    input_window_t * iw = input_window_create(this->context);
    input_window_show(iw);

    char * tab_name = input_window_get_field_data(iw, IW_TAB_INDEX);
    char * file_name = input_window_get_field_data(iw, IW_FILE_INDEX);
    char * regex = input_window_get_field_data(iw, IW_REGEX_INDEX);

    rtrim_field(tab_name, IW_INPUT_SIZE);
    rtrim_field(file_name, IW_INPUT_SIZE);
    rtrim_field(regex, IW_INPUT_SIZE);

    tab_manager_add_tab(this, tab_name, file_name, regex);

    input_window_destroy(iw);
}

void tab_manager_add_tab(tab_manager_t * this, char * name, char* file_name, char * regex)
{
    if(file_name[0] == 0) 
        return;

    tab_t tab;
    tab_init(&tab, name, file_name, regex,
             this->context->screen_cols - 2,
             this->context->screen_rows - HELP_TAB_SIZE - 3);

    list_append(this->tab_list, &tab, sizeof(tab_t));
}

void tab_manager_refresh_all_tabs(tab_manager_t * this)
{
    list_for_each(this->tab_list, tab_print_cast);
}

void tab_manager_toggle_color(tab_manager_t * tm)
{
    if(!tm)
        return;

    if(tab_manager_tabs_amount(tm) == 0)
        return;

    tab_toggle_color(tab_manager_get_active_tab(tm));
}

bool tab_manager_get_color(tab_manager_t * tm)
{
    if(!tm)
        return true;
    
    tab_t * current = tab_manager_get_active_tab(tm);

    if(!current)
        return true;

    return current->color;
}

void tab_manager_close_tab(tab_manager_t * tm)
{
    if(!tm)
        return;

    list_node_t * tab_node = list_delete(tm->tab_list, tm->active_tab);

    if(!tab_node)
        return;

    tab_destroy_contents(tab_node->data);
    list_node_destroy(tab_node);

    if(tm->tab_list->size == 0)
        tm->active_tab = 0;
    
    if(tm->active_tab > 0)
        tm->active_tab -= 1;
}

size_t tab_manager_tabs_amount(tab_manager_t * tm)
{
    if(!tm)
        return 0;

    return tm->tab_list->size;
}
