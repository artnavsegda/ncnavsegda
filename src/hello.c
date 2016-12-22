#include <ncurses.h>
#include <stdlib.h>

int main(void)
{
	int x=10,y=10;
	initscr();
	noecho();
	curs_set(FALSE);
	keypad(stdscr, TRUE);
	atexit(endwin);

	while(1)
	{
		clear();
		mvprintw(y, x, "Hello World");
		refresh();
		switch (getch())
		{
			case KEY_RESIZE:
			break;
			case KEY_UP:
				y--;
			break;
			case KEY_DOWN:
				y++;
			break;
			case KEY_RIGHT:
				x++;
			break;
			case KEY_LEFT:
				x--;
			break;
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
