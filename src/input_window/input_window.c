#include "input_window/input_window.h"
#include "utils.h"
#include <stdlib.h>

#define ROWS 24
#define COLS 80

input_window_t * input_window_create()
{
    input_window_t * iw = malloc(sizeof(input_window_t));
    if(iw)
        input_window_init(iw);
    return iw;
}

void input_window_init(input_window_t * iw)
{
    iw->form_win = newwin(ROWS, COLS, 0, 0);
    box(iw->form_win, 0, 0);
    wattron(iw->form_win, COLOR_PAIR(HIGHLIGHT_WHITE));
    mvwprintw(iw->form_win, 0, CENTER_TEXT(COLS, TITLE), TITLE);
    wattroff(iw->form_win, COLOR_PAIR(HIGHLIGHT_WHITE));

    iw->fields[IW_FIELDS_AMOUNT] = NULL;
    for(size_t field = 0; field < IW_FIELDS_AMOUNT; field += 2)
    {
        // Label
        iw->fields[field] = new_field(1, 10, field, 0, 0, 0);
        // Input box
        iw->fields[field + 1] = new_field(1, 40, field, 15, 0, 0);
    }

    set_field_buffer(iw->fields[0], 0, IW_TAB_NAME_LABEL);
    set_field_buffer(iw->fields[2], 0, IW_FILE_NAME_LABEL);
    set_field_buffer(iw->fields[4], 0, IW_REGEX_LABEL);

    set_field_opts(iw->fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(iw->fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(iw->fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(iw->fields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    set_field_opts(iw->fields[4], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(iw->fields[5], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	set_field_back(iw->fields[1], A_UNDERLINE);
	set_field_back(iw->fields[3], A_UNDERLINE);
    set_field_back(iw->fields[5], A_UNDERLINE);

    iw->form = new_form(iw->fields);

    set_form_win(iw->form, iw->form_win);
    set_form_sub(iw->form, derwin(iw->form_win, 18, 76, 1, 1));
    post_form(iw->form);

}

void input_window_destroy(input_window_t * iw)
{
    curs_set(0);
    if(iw)
    {
        unpost_form(iw->form);
        free_form(iw->form);
        for(size_t field = 0; field < IW_FIELDS_AMOUNT; ++field)
            free_field(iw->fields[field]);
        delwin(iw->form_win);
        free(iw);
    }
}

void input_window_show(input_window_t * iw)
{
    curs_set(1);
    keypad(iw->form_win, true);
    refresh();
    wrefresh(iw->form_win);

    int ch = 0;
    while( (ch = getch()) != KEY_F(1) )
       input_window_handle_keys(iw, ch); 
}

void input_window_handle_keys(input_window_t * iw, int ch)
{
    switch(ch)
    {
        case KEY_DOWN:
			form_driver(iw->form, REQ_NEXT_FIELD);
			form_driver(iw->form, REQ_END_LINE);
			break;
		case KEY_UP:
			form_driver(iw->form, REQ_PREV_FIELD);
			form_driver(iw->form, REQ_END_LINE);
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
		default:
			form_driver(iw->form, ch);
			break;
    }

    wrefresh(iw->form_win);
}