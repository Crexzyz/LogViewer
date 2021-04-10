#include "tab_manager/tab_manager.h"
#include "windows/input_window.h"
#include "utils.h"

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
    }
}

void tab_manager_destroy(tab_manager_t * tm)
{
    if(tm)
    {
        for (size_t tab = 0; tab < tm->tab_amount; ++tab)
        {
            if(tm->tabs[tab] != 0)
            {
                tab_destroy(tm->tabs[tab]);
            }
        }
        free(tm);
    }
}

void tab_manager_handle_input(tab_manager_t * tm, size_t input)
{
    if(!tm)
        return;

    if(tm->tab_amount == 0)
        return;

    tab_t * curr_tab = tab_manager_get_active_tab(tm);

    if(!curr_tab)
        return;

    if(input == KEY_RIGHT || input == 67)
    {
        tm->active_tab = (tm->active_tab + 1) % tm->tab_amount;
    }
    else if(input == KEY_LEFT || input == 68)
    {
        tm->active_tab = tm->active_tab == 0 ? tm->tab_amount - 1 : tm->active_tab - 1;
    }
    else if(input == KEY_UP || input == 65)
    {
        if(curr_tab->last_row != 0)
            curr_tab->last_row -= 1;
    }
    else if(input == KEY_DOWN || input == 66)
    {
        if(curr_tab->last_row < curr_tab->rows)
            curr_tab->last_row += 1;
    }
    else if(input == 'R')
    {
        tab_manager_refresh_tab(tm);
    }
    else if(input == 18) // ctrl + r
    {
        tab_manager_refresh_all_tabs(tm);
    }
    else if(input == 360 || input == 70) // end
    {
        curr_tab->last_row = curr_tab->rows - tm->context->screen_rows
                             + HELP_TAB_SIZE + 3;
    }
}

tab_t * tab_manager_get_active_tab(tab_manager_t * tm)
{
    if(!tm)
        return NULL;
    
    if(tm->tab_amount > 0)
        return tm->tabs[tm->active_tab];

    return NULL;
}

void tab_manager_print_active(tab_manager_t * tm, WINDOW * target_window)
{
    if(!tm || !target_window)
        return;

    if(tm->tab_amount == 0)
        return;

    tab_t * tab = tab_manager_get_active_tab(tm);

    if(!tab)
        return;

    tab_manager_refresh_tab(tm);

    prefresh(tab->window, // Window struct
             tab->last_row, // Pad's row
             0, // Pad's col
             2, // Screen row
             1, // Screen col
             tm->context->screen_rows - 2 - HELP_TAB_SIZE, // Rows to print
             tm->context->screen_cols - 2); // Cols to print
}

void tab_manager_print_tabs(tab_manager_t * this, WINDOW * tabs_window)
{
    if(this->tab_amount == 0)
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
    size_t tab = 0;

    // Actual printing
    for(tab = this->tab_display_start; tab < this->tab_amount; ++tab)
    {
        size_t color = this->active_tab == tab ? COLOR_PAIR(HIGHLIGHT_CYAN) : COLOR_PAIR(HIGHLIGHT_WHITE);
        size_t tab_name_size = strnlen(this->tabs[tab]->name, TAB_MAX_TAB_NAME);

        if(print_index + tab_name_size <= printable_chars )
        {
            wattron(tabs_window, color);
                mvwprintw(tabs_window, 1, print_index, "%s ", this->tabs[tab]->name);
            wattroff(tabs_window, color);
        }
        else
        {
            break; // No more space to print
        }

        print_index += tab_name_size + 1;
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
    // File name and existence validations
    if(this->tab_amount == TAB_MANAGER_MAX_TABS) 
        return; // TODO: Popup

    if(file_name[0] == 0) 
        return;

    name = name && name[0] == '\0' ? "<No name>" : name;
    
    tab_t * tab = tab_create();
    tab_set_name(tab, name);
    tab_set_file_name(tab, file_name);

    FILE * file = tab_manager_open_file(tab);
    
    if(file == NULL)
    {
        tab_destroy(tab);
        return;
    }

    // Count file's lines
    size_t lines = tab_manager_get_lines(file);
    fclose(file);

    // Smaller size than screen
    if (lines < this->context->screen_rows - 2 - HELP_TAB_SIZE) 
        lines = this->context->screen_rows;

    tab_set_lines(tab, lines);

    if (regex == NULL || regex[0] == '\0')
        tab->has_regex = false;
    else
        tab_set_regex(tab, regex);

    tab_add_pad(tab, this->context->screen_cols, tab->rows);

    this->tabs[this->tab_amount] = tab;
    ++this->tab_amount;
}

void tab_manager_refresh_tab(tab_manager_t * this)
{
    tab_t * current_tab = this->tabs[this->active_tab];

    if(!current_tab)
        return;

    FILE * file = tab_manager_open_file(current_tab);

    if(!file)
        return;

    tab_print(current_tab, file);
    fclose(file);
}

void tab_manager_refresh_all_tabs(tab_manager_t * this)
{
    size_t active_tab_aux = this->active_tab;
    for (size_t tab = 0; tab < this->tab_amount; ++tab)
    {
        this->active_tab = tab;
        tab_manager_refresh_tab(this);
    }

    this->active_tab = active_tab_aux;
    wrefresh(this->tabs[this->active_tab]->window);
}

FILE * tab_manager_open_file(tab_t * current_tab)
{
    FILE * file = 0;

    if(current_tab->has_regex)
    {
        // Change this size later
        char command[600];
        bzero(command, 600);
        sprintf(command, "grep \'%s\' %s > .grepresult", current_tab->regex, current_tab->file);
        file = popen((const char *)command, "r");
        pclose(file);

        file = fopen(".grepresult", "r");
    }
    else
    {
         file = fopen(current_tab->file, "r");
    }

    if(file == NULL)
    {
        mvprintw(0,0, "Failed to open [%s]", current_tab->file);
        refresh();
    }
    
    return file;
}

int tab_manager_get_lines(FILE * file)
{
    char c = 0;
    int count = 0;
    for (c = fgetc(file); c != EOF; c = fgetc(file)) 
        if (c == '\n') // Increment count if this character is newline 
            count = count + 1; 

    rewind(file);
    return count;
}

void tab_manager_toggle_color(tab_manager_t * tm)
{
    if(!tm)
        return;

    if(tm->tab_amount == 0)
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

    tab_t * current = tab_manager_get_active_tab(tm);

    if(!current)
        return;
        
    tab_destroy(current);

    // Rearrange pointers
    for(size_t tab = 0; tab < tm->tab_amount - 1; ++tab)
    {
        if(tab >= tm->active_tab)
        {
            tm->tabs[tab] = tm->tabs[tab + 1];
            tm->tabs[tab + 1] = NULL;
        }
    }

    tm->tab_amount -= 1;
}
