#include <ncurses.h>

int main(void)
{
	initscr();
	noecho();
	curs_set(FALSE);

	mvprintw(0,0, "Hello World");
	refresh();

	getch();
	endwin();

	return 0;
}
