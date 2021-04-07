#include "log_viewer/interface.h"
#include "tab_manager/tab_manager.h"
#include "windows/window_builder.h"
#include "context/context.h"
#include "utils.h"

#include <ncurses.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	// Do not show cursor
	curs_set(0);
	if(has_colors() == FALSE)
	{	
		endwin();
		printf("Your terminal does not support color\n");
		return 1;
	}
	// Use colors
	start_color();
	use_default_colors();

	interface_t * interface = interface_create();
	interface_init(interface);

	if(argc > 1)
		for (int i = 1; i < argc; ++i)
			tab_manager_add_tab(interface->tab_manager, interface->tabs_window, (char*)argv[i], (char*)argv[i], 0);

	interface_run(interface);

	interface_destroy(interface);
	endwin();
	
	return 0;
}
