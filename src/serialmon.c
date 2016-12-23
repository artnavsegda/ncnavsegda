#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

struct dataunit {
	unsigned char rxnum;
	unsigned char recbyte;
	unsigned long millis;
};

struct dataunit datamassive[10000];

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("enter file name\n");
		return 1;
	}

	FILE * datafile = fopen(argv[1],"r");
	if (datafile == NULL)
	{
		perror("open file");
		return 1;
	}

	int position = 0;
	while (fscanf(datafile,"%lu:RX%hhu:%hhX",&datamassive[position].millis,&datamassive[position].rxnum,&datamassive[position].recbyte) != EOF)
		position++;

/*	printf("%d record read\n",position);
	for (int i=0; i<position; i++)
		printf("%lu:RX%hhu:%hhX\n", datamassive[i].millis,datamassive[i].rxnum,datamassive[i].recbyte);*/

	initscr();
	noecho();
	curs_set(FALSE);
	keypad(stdscr, TRUE);
	atexit(endwin);

	int x = 0;

	while(1)
	{
		clear();
		mvprintw(0,0,"%d record read",position);
		mvprintw(10,10,"%x",datamassive[x].recbyte);
		refresh();

		switch(getch())
		{
			case 'q':
				exit(0);
			break;
			case KEY_RIGHT:
				x++;
			break;
			default:
				refresh();
			break;
		}

	}
	endwin();

	return 0;
}
