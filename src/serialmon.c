#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

struct dataunit {
	unsigned char rxnum;
	unsigned char recbyte;
	unsigned long millis;
};

struct dataunit datamassive[10000];

int main(void)
{
	int position = 0;
	while (scanf("%lu:RX%hhu:%hhX",&datamassive[position].millis,&datamassive[position].rxnum,&datamassive[position].recbyte) != EOF)
		position++;

/*	printf("%d record read\n",position);
	for (int i=0; i<position; i++)
		printf("%lu:RX%hhu:%hhX\n", datamassive[i].millis,datamassive[i].rxnum,datamassive[i].recbyte);*/

	initscr();
	noecho();
	curs_set(FALSE);
	keypad(stdscr, TRUE);
	atexit(endwin);

	while(1)
	{
		clear();
		mvprintw(0,0,"%d record read",position);
		mvprintw(10,10,"%x",datamassive[0].recbyte);
		refresh();

		switch(getch())
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
