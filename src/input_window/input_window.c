#include "input_window/input_window.h"
#include "utils.h"
#include <stdlib.h>
#include <ctype.h>

#define ROWS 24
#define COLS 80

input_window_t * input_window_create(size_t rows, size_t cols)
{
    input_window_t * iw = malloc(sizeof(input_window_t));
    if(iw)
        input_window_init(iw, rows, cols);
    return iw;
}

void input_window_init(input_window_t * iw, size_t rows, size_t cols)
{
    iw->fields[IW_FIELDS_AMOUNT] = NULL;
    for(size_t field = 0; field < IW_FIELDS_AMOUNT; field += 2)
    {
        // Label
        iw->fields[field] = new_field(1, 9, field, 0, 0, 0);
        // Input box
        iw->fields[field + 1] = new_field(1, IW_INPUT_SIZE, field, 10, 0, 0);
    }

    set_field_buffer(iw->fields[0], 0, IW_TAB_NAME_LABEL);
    set_field_buffer(iw->fields[2], 0, IW_FILE_NAME_LABEL);
    set_field_buffer(iw->fields[4], 0, IW_REGEX_LABEL);

    set_field_opts(iw->fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(iw->fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(iw->fields[4], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);

    
    set_field_opts(iw->fields[IW_TAB_INDEX], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    set_field_opts(iw->fields[IW_FILE_INDEX], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    set_field_opts(iw->fields[IW_REGEX_INDEX], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

    set_field_back(iw->fields[IW_FILE_INDEX], COLOR_PAIR(HIGHLIGHT_WHITE));
    set_field_back(iw->fields[IW_REGEX_INDEX], COLOR_PAIR(HIGHLIGHT_WHITE));

    iw->form = new_form(iw->fields);

    int fixed_rows = 0;
    int fixed_cols = 0;
    scale_form(iw->form, &fixed_rows, &fixed_cols);
    fixed_rows += 4;
    fixed_cols += 4;

    size_t start_row = CENTER_WINDOW(rows, fixed_rows);
    size_t start_col = CENTER_WINDOW(cols, fixed_cols);

    iw->form_win = newwin(fixed_rows, fixed_cols, start_row, start_col);
    keypad(iw->form_win, true);
    
    set_form_win(iw->form, iw->form_win);
    set_form_sub(iw->form, derwin(iw->form_win, fixed_rows - 4,
                                  fixed_cols - 4, 2, 1));

    box(iw->form_win, 0, 0);

    wattron(iw->form_win, COLOR_PAIR(HIGHLIGHT_WHITE));
    mvwprintw(iw->form_win, 0, CENTER_TEXT(fixed_cols, TITLE), TITLE);
    wattroff(iw->form_win, COLOR_PAIR(HIGHLIGHT_WHITE));
}

void input_window_destroy(input_window_t * iw)
{
    curs_set(0);
    if(!iw)
        return;
        
    unpost_form(iw->form);
    free_form(iw->form);
    for(size_t field = 0; field < IW_FIELDS_AMOUNT; ++field)
        free_field(iw->fields[field]);
    delwin(iw->form_win);
    free(iw);
}

void input_window_show(input_window_t * iw)
{
    if(!iw)
        return;

    // TODO: Move cursor to first input
    post_form(iw->form);
    wrefresh(iw->form_win);
    refresh();
    curs_set(1);

    int ch = 0;
    while((ch = getch()) != '\n')
    {
        input_window_handle_keys(iw, ch); 
    }
    
    // Save current field changes
    form_driver(iw->form, REQ_END_LINE);
}

void input_window_handle_keys(input_window_t * iw, int ch)
{
    switch(ch)
    {
        case '\t':
        case KEY_DOWN:
            form_driver(iw->form, REQ_NEXT_FIELD);
            form_driver(iw->form, REQ_END_LINE);
            input_window_update_active_field(iw);
            break;
        case KEY_UP:
            form_driver(iw->form, REQ_PREV_FIELD);
            form_driver(iw->form, REQ_END_LINE);
            input_window_update_active_field(iw);
            break;
        case KEY_LEFT:
            form_driver(iw->form, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            form_driver(iw->form, REQ_NEXT_CHAR);
            break;
        case KEY_BACKSPACE:
        case 127:
            form_driver(iw->form, REQ_DEL_PREV);
            break;
        case KEY_DC:
            form_driver(iw->form, REQ_DEL_CHAR);
            break;
        case 383: // Shift + Delete
            form_driver(iw->form, REQ_CLR_FIELD);
            break;
        default:
            form_driver(iw->form, ch);
            break;
    }
    
    wrefresh(iw->form_win);
}

char * input_window_get_field_data(input_window_t * iw, size_t index) 
{
    if(iw) 
        return field_buffer(iw->fields[index], 0);
    
    return NULL;
}

void input_window_update_active_field(input_window_t * iw)
{
    FIELD * active = current_field(iw->form);
    for(size_t field = 1; field < IW_FIELDS_AMOUNT; field += 2)
    {
        size_t color = COLOR_PAIR(HIGHLIGHT_WHITE);
        if(active == iw->fields[field])
            color = COLOR_PAIR(HIGHLIGHT_BLACK);
        
        set_field_back(iw->fields[field], color);
    }
}

void rtrim_field(char * string, size_t dyn_len)
{
    size_t real_len = 0;
    size_t last_char = 0;
    bool empty = true;
    
    // Find last character in the dynamic-sized field
    while(true)
    {   
        real_len += dyn_len;
        for(size_t i = 0; i < real_len; ++i)
        {
            if(string[i] == '\0')
                break;

            if(!isspace(string[i]))
            {
                last_char = i;
                empty = false;
            }
        }

        if(last_char < real_len - 1)
            break;
    }

    if(empty)
    {
        string[0] = '\0';
        return;
    }    

    // Clean all the whitespaces
    for(size_t i = real_len - 1; i > last_char; --i)
    {
        string[i] = '\0';
    }
}
