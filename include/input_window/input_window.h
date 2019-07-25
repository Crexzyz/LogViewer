#ifndef INPUT_WINDOW_H
#define INPUT_WINDOW_H

#include "log_viewer/interface.h"

#include <ncurses.h>
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

/**
 * @brief      Calculates the size of a window based on the defined window usage.
 * 
 * @param      number  The rows or columns of the terminal screen
 *
 * @return     The soze of the rows or columns based on the requested screen usage.
 */
#define CALCULATE_USAGE(number) number * (double)SCREEN_USAGE/100.0

/**
 * @brief      Calculates the coordinates where the window is going to be placed.
 *
 * @param      full_size   The size of the terminal screen
 * @param      usage_size  The size of the window
 *
 * @return     The coordinate where the window must start drawing.
 */
#define CALCULATE_COORDS(full_size, usage_size) (full_size - usage_size)/2

/**
 * @brief      Sets 0 in all the buffers of the window input struct
 *
 * @param      this  The input window instance
 *
 * @return     none
 */
#define CLEAN_BUFFERS(this) bzero(this->file_name, MAX_FILE_NAME);\
							bzero(this->tab_name, MAX_TAB_NAME);\
							bzero(this->regex, MAX_REGEX);

/**
 * @brief      Calculates the position where some text needs to be placed to be centered
 *
 * @param      col_size  The column size of the window
 * @param      title     The title of the window
 *
 * @return     The x coordinate where the title must be printed
 */
#define CENTER_TEXT(col_size, title) (col_size - strlen(title))/2

/**
 * @brief      The size types that can be requested when calling input_window_get_size_by_type()
 */
enum size_types
{
	BUFFER,
	TEXT
};

typedef struct input_window
{
	/// Amount of rows the terminal screen has
	int rows;
	/// Amount of rows the window occupies
	int window_rows;

	/// Amount of columns the terminal screen has
	int cols;
	/// Amount of columns the window occupies
	int window_cols;

	/// Ncurses window pointer
	WINDOW * window;

	/// Buffer for the file name
	char * file_name;
	/// Buffer for the tab name
	char * tab_name;
	/// Buffer for the grep regular expression
	char * regex;

	/// Pointer to the interface struct. Used is resizing is needed
	interface_t * interface;
	
} input_window_t;

/**
 * @brief      Creates an input window in heap memory.
 *
 * @return     A pointer of an input window allocated in heap memory.
 * @remarks    Data allocated by this call must be freed.
 */
input_window_t * input_window_create();

/**
 * @brief      Frees allocated memory in heap memory.
 * 
 * @param      this The input window instance that allocated memory.
 */
void input_window_destroy(input_window_t * this);

/**
 * @brief      Initializes input window variables.
 * 
 * It also allocates memory for each buffer the input window has.
 *
 * @param      this	      The (uninitialized) input window instance
 * @param[in]  interface  A pointer to the interface instance
 * @param[in]  rows       The rows the terminal screen has 
 * @param[in]  cols       The columns the terminal screen has
 */
void input_window_init(input_window_t * this, interface_t * interface, int rows, int cols);

/**
 * @brief      Prints descriptive text in the blank window
 *
 * @param      this The input window instance
 */
void input_window_init_text(input_window_t * this);

/**
 * @brief      Reads the input for each buffer
 *
 * @param      this The input window instance
 */
void input_window_get_input(input_window_t * this);

/**
 * @brief      Reads and stores text in a specific buffer
 *
 * @param      this		   The input window instance
 * @param      buffer      A pointer to the buffer where the user is going to write
 * @param[in]  cursor_row  The row where the cursor is going to be positioned
 * @param[in]  cursor_col  The col where the cursor is going to be positioned
 *
 * @return     True if the user needs to write on the previous buffer, false otherwise
 */
bool input_window_read_text(input_window_t * this, char * buffer, int cursor_row, int cursor_col);

/**
 * @brief      Gets the size of some struct member depending on which type of size is needed
 * If the type is BUFFER, the function returns the buffer size of the requested buffer.
 * If the type is TEXT, the function returns the string length of the requested buffer.
 * 
 * @param      this 	  The input window instance
 * @param      buffer     A pointer to some buffer inside the window input struct
 * @param[in]  type       The length type that is needed to be returned
 *
 * @return     The length of a buffer depending of the return type requested
 */
int input_window_get_size_by_type(input_window_t * this, char * buffer, int type);

/**
 * @brief      Resizes all the windows
 *
 * @param      this The input window instance
 */
void input_window_resize(input_window_t * this);

/**
 * @brief      Returns the tab name saved on the last input
 *
 * @param      this The input window instance
 *
 * @return     A pointer to the tab name buffer
 */
static inline char * input_window_get_tab_name(input_window_t * this) { return this->tab_name; }

/**
 * @brief      Returns the file name saved on the last input
 *
 * @param      this The input window instance
 *
 * @return     A pointer to the file name buffer
 */
static inline char * input_window_get_file_name(input_window_t * this) { return this->file_name; }

/**
 * @brief      Returns the regular expression saved on the last input
 *
 * @param      this The input window instance
 *
 * @return     A pointer to the regular expression buffer
 */
static inline char * input_window_get_regex(input_window_t * this) { return this->regex; }


#endif //INPUT_WINDOW_H