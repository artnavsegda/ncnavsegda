#include <ncurses.h>
#include <stdlib.h>

int main(void)
{
	initscr();
	noecho();
	curs_set(FALSE);
	atexit(endwin);

	mvprintw(0,0, "Hello World");
	refresh();

	while(1)
	{
		switch (getch())
		{
			case 'q':
				exit(0);
			break;	
			default:
				refresh();
			break;
		}
	}

	endwin();

	return 0;
}
