#ifndef INPUT_WINDOW_H
#define INPUT_WINDOW_H

#include <ncurses.h>
#include <form.h>
#include <string.h>

/// Window title
#define TITLE "Open file"
/// Percentage of the full screen that is going to be covered by the window
#define SCREEN_USAGE 40
/// File name buffer size
#define MAX_FILE_NAME 256
/// Tab name buffer size
#define MAX_TAB_NAME 20
/// Regular expression buffer size
#define MAX_REGEX 256
#define IW_TAB_NAME_LABEL "Tab name"
#define IW_FILE_NAME_LABEL "File name"
#define IW_REGEX_LABEL "Regex"
#define IW_FIELDS_AMOUNT 6

/**
 * @brief      Calculates the size of a window based on the defined window usage.
 * @param      number  The rows or columns of the terminal screen
 * @return     The soze of the rows or columns based on the requested screen usage.
 */
#define CALCULATE_USAGE(number) number * (double)SCREEN_USAGE / 100.0

/**
 * @brief      Calculates the coordinates where the window is going to be placed.
 * @param      full_size   The size of the terminal screen
 * @param      usage_size  The size of the window
 * @return     The coordinate where the window must start drawing.
 */
#define CALCULATE_COORDS(full_size, usage_size) (full_size - usage_size) / 2

typedef struct input_window
{
	FORM * form;
	FIELD * fields[IW_FIELDS_AMOUNT + 1];
	WINDOW * form_win;
	WINDOW * form_sub;
} input_window_t;

void input_window_init(input_window_t * iw);
input_window_t * input_window_create();
void input_window_destroy(input_window_t * iw);
void input_window_show(input_window_t * iw);
void input_window_handle_keys(input_window_t * iw, int ch);

#endif //INPUT_WINDOW_H
