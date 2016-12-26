#include <ncurses.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>

struct a5framestruct {
	short currentpmt;
	short valueamp1;
	short valueamp2;
	short valueamp3;
	short battery;
	short autosampler;
	short temperature;
	short currentbattery;
	char reserved1;
	char reserved2;
	char reserved3;
	char reserved4;
	char voltagepmt;
};

unsigned char genchecksum(unsigned char *massive, int sizeofmassive)
{
        unsigned char checksum = 0;
        for (int i=0;i<sizeofmassive;i++)
                checksum = checksum + massive[i];
        return checksum;
}

int readframe(int fd, int framelength, void *frame)
{
	unsigned char checksum[1];
	read(fd,frame,framelength);
	read(fd,checksum,1);
	if (genchecksum(frame,framelength)==checksum[0])
		return 0;
	else
		return 1;
}

void sendcommand(int fd, unsigned char marker, unsigned char *frame, int framelength)
{
        unsigned char markerframe[1];
        unsigned char checksum[1];
        markerframe[0] = marker;
        write(fd,markerframe,1);
        usleep(20*1000);
        write(fd,frame,framelength);
        checksum[0] = genchecksum(frame,framelength);
        write(fd,checksum,1);
}

void transfermode(int fd)
{
        unsigned char frame[1];
        bzero(frame,1);
        frame[0] = 0x01;
        sendcommand(fd,0xCA,frame,1);
}

int main(int argc, char *argv[])
{
	unsigned char frame[22], confirm[1];
	struct termios tio = {
		.c_cflag = B9600 | CS8 | CLOCAL | CREAD,
		.c_iflag = IGNPAR,
		.c_oflag = 0,
		.c_lflag = 0,
		.c_cc[VTIME] = 100,
		.c_cc[VMIN] = 22
	};

	if (argc != 2)
	{
		printf("name serial port\n");
		exit(1);
	}

	int fd = open(argv[1], O_RDWR | O_NOCTTY);
	if (fd == -1)
	{
		perror("cannot open serial port");
		exit(1);
	}

	cfsetspeed(&tio,B9600);
	tcsetattr(fd,TCSANOW,&tio);

	transfermode(fd);

	int x=10,y=10;
	initscr();
	noecho();
	curs_set(FALSE);
	keypad(stdscr, TRUE);
	atexit(endwin);
	clear();
	refresh();

	struct a5framestruct a5frame;
	long a5counter = 0;

	while(1)
	{
		if (read(fd,frame,1)==1)
		{
			switch (frame[0])
			{
			case 0xA5:
				if (!readframe(fd,21,&a5frame))
				{
					mvprintw(0,0,"%6ld 0xA5 %6d %6d %6d %6d %6d %6d %6d %6d\n", a5counter++,
						a5frame.currentpmt,
                                                a5frame.valueamp1,
						a5frame.valueamp2,
						a5frame.valueamp3,
						a5frame.battery,
						a5frame.autosampler,
						a5frame.temperature,
						a5frame.currentbattery);
					refresh();
				}
			break;
			case 0xAF:
				if (readframe(fd
			case 0xCA:
			case 0xBE:
			case 0xBD:
			case 0xBB:
			case 0xBA:
			case 0xBF:
			case 0xB0:
			case 0xB2:
			case 0xB6:
			case 0xB7:
			case 0xB4:
			case 0xC0:
			case 0xE3:
			case 0xE5:
			case 0xE6:
				read(fd,confirm,1);
			break;
			default:
			break;
			}
		}
/*		switch (getch())
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
		*/
//		refresh();
	}

	endwin();

	return 0;
}
