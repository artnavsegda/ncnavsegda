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
	int position_shift;
	int rx_shift;

	while(1)
	{
		clear();
		mvprintw(0,0,"%d record read",position);

		/*for (int i=0;i<COLS/2;i++)
			mvprintw(10,i*2,"%02X",datamassive[x+i].recbyte);*/

		mvprintw(10,0,"RX0");
		mvprintw(11,0,"RX1");
		mvprintw(12,0,"RX2");

		rx_shift = datamassive[x].rxnum;
		mvprintw(10+rx_shift,10,"%02X",datamassive[x].recbyte);

		for (int i=1;i<20;i++)
		{
			position_shift = datamassive[x+i].millis - datamassive[x].millis;
			rx_shift = datamassive[x+i].rxnum;

			mvprintw(10+rx_shift,10+(i*2)+position_shift,"%02X",datamassive[x+i].recbyte);
		}

		/*position_shift = datamassive[x+1].millis - datamassive[x].millis;

		mvprintw(10,12+position_shift,"%02X",datamassive[x+1].recbyte);

		position_shift = datamassive[x+2].millis - datamassive[x].millis;

		mvprintw(10,14+position_shift,"%02X",datamassive[x+2].recbyte);*/

		refresh();

		switch(getch())
		{
			case 'q':
				exit(0);
			break;
			case KEY_RIGHT:
				x++;
			break;
			case KEY_LEFT:
				if (x>0)
					x--;
			break;
			default:
				refresh();
			break;
		}

	}
	endwin();

	return 0;
}
