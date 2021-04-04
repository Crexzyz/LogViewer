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
#define IW_FN_SIZE 256
/// Tab name buffer size
#define IW_TAB_SIZE 20
/// Regular expression buffer size
#define IW_REGEX_SIZE 256

#define IW_TAB_NAME_LABEL "Tab name"
#define IW_FILE_NAME_LABEL "File name"
#define IW_REGEX_LABEL "Regex"
#define IW_FIELDS_AMOUNT 6

#define IW_TAB_INDEX 1
#define IW_FILE_INDEX 3
#define IW_REGEX_INDEX 5
#define IW_INPUT_SIZE 20

/**
 * @brief      Calculates the size of a window based on the defined window usage.
 * @param      number  The rows or columns of the terminal screen
 * @return     The soze of the rows or columns based on the requested screen usage.
 */
#define IW_RESIZE(number, percentage) number * (double)percentage / 100.0

/**
 * @brief      Calculates the coordinates where the window is going to be placed.
 * @param      full_size   The size of the terminal screen
 * @param      usage_size  The size of the window
 * @return     The coordinate where the window must start drawing.
 */
#define CENTER_WINDOW(full_size, usage_size) (full_size - usage_size) / 2

typedef struct input_window
{
	FORM * form;
	FIELD * fields[IW_FIELDS_AMOUNT + 1];
	WINDOW * form_win;
} input_window_t;

void input_window_init(input_window_t * iw, size_t rows, size_t cols);
input_window_t * input_window_create(size_t rows, size_t cols);
void input_window_destroy(input_window_t * iw);
void input_window_show(input_window_t * iw);
void input_window_handle_keys(input_window_t * iw, int ch);
char * input_window_get_field_data(input_window_t * iw, size_t index);
void input_window_update_active_field(input_window_t * iw);
// Move to utils
void rtrim_field(char * string, size_t dyn_len);

#endif //INPUT_WINDOW_H
