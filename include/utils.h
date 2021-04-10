#ifndef UTILS_H
#define UTILS_H

#define HIGHLIGHT_NONE -1
#define HIGHLIGHT_ERROR 1
#define HIGHLIGHT_WHITE 2
#define HIGHLIGHT_YEL 3
#define HIGHLIGHT_BLACK 4
#define HIGHLIGHT_CYAN 5

#define CENTER_TEXT(col_size, title) (col_size - strlen(title)) / 2
#define RIGHT_TEXT(col_size, title) col_size - strlen(title) - 2

#define KEY_ESC 27

enum text_positions
{
	LEFT,
	CENTER,
	RIGHT
};

#endif