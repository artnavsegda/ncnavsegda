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

struct afframestruct {
        unsigned char coefficentamp3;
        unsigned char coefficentamp2;
        unsigned char coefficentamp1;
        short dark_current;
        short biasamp3;
        short biasamp2;
        short biasamp1;
	short switch_threshhold;
	short coefficentcalib;
	unsigned char zerobias;
};

struct aeframestruct {
	unsigned char timeaverage;
	unsigned char timezero;
	unsigned char timecumulation;
	unsigned char timeintegration;
	short lowerthreshold;
	short upperthreshold;
};

struct a0framestruct {
	char devicenumber[4];
};

struct a1framestruct {
	char calibrationdate[24];
};

struct adframestruct {
	short operatinglamp1;
	short operatinglamp2;
};

struct aaframestruct {
	short linearization;
};

struct a2framestruct {
	unsigned char language;
	unsigned char configuration;
};

struct a7framestruct {
	unsigned char devicetype;
};

struct a4framestruct {
	short valveoff;
	short valveon;
};

struct a6framestruct {
	short fractional;
	short main;
};

struct frame75struct {
	short sensivity;
	short outputwindow;
	short windowoffset;
};

struct frame76struct {
	short pmtsupplyshift;
};

struct frame77struct {
	short pmtsupplysensivity;
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
	struct afframestruct afframe;
	struct aeframestruct aeframe;
	struct a0framestruct a0frame;
	struct a1framestruct a1frame;
	struct adframestruct adframe;
	struct aaframestruct aaframe;
	struct a2framestruct a2frame;
	struct a7framestruct a7frame;
	struct a4framestruct a4frame;
	struct a6framestruct a6frame;
	struct frame75struct frame75;
	struct frame76struct frame76;
	struct frame77struct frame77;
	long a5counter = 0;
	long afcounter = 0;
	long aecounter = 0;
	long a0counter = 0;
	long a1counter = 0;
	long adcounter = 0;
	long aacounter = 0;
	long a2counter = 0;
	long a7counter = 0;
	long a4counter = 0;
	long a6counter = 0;
	long counter75 = 0;
	long counter76 = 0;
	long counter77 = 0;

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
				if (!readframe(fd,16,&afframe))
				{
					mvprintw(1,0,"%6ld 0xAF %6d %6d %6d %6d %6d %6d %6d %6d %6d %6d\n", afcounter++,
						afframe.coefficentamp3,
						afframe.coefficentamp2,
						afframe.coefficentamp1,
						afframe.dark_current,
						afframe.biasamp3,
						afframe.biasamp2,
						afframe.biasamp1,
						afframe.switch_threshhold,
						afframe.coefficentcalib,
						afframe.zerobias);
					refresh();
				}
			case 0xAE:
				if (!readframe(fd,8,&aeframe))
				{
					mvprintw(2,0,"%6ld 0xAE %6d %6d %6d %6d %6d %6d\n", aecounter++,
							aeframe.timeaverage,
							aeframe.timezero,
							aeframe.timecumulation,
							aeframe.timeintegration,
							aeframe.lowerthreshold,
							aeframe.upperthreshold);
				}
			break;
			case 0xA0:
				if (!readframe(fd,4,&a0frame))
				{
					mvprintw(3,0,"%6ld 0xA0 %s\n", a0counter++,
							a0frame.devicenumber);
				}
			break;
			case 0xA1:
				if (!readframe(fd,25,&a1frame))
				{
					mvprintw(4,0,"%6ld 0xA1 %s\n", a1counter++,
							a1frame.calibrationdate);
				}
			break;
			case 0xAD:
				if (!readframe(fd,4,&adframe))
				{
					mvprintw(5,0,"%6ld 0xAD %d %d\n", adcounter++,
							adframe.operatinglamp1,
							adframe.operatinglamp2);
				}
			break;
			case 0xAA:
				if (!readframe(fd,2,&aaframe))
				{
					mvprintw(6,0,"%6ld 0xAA %d\n", aacounter++,
							aaframe.linearization);
				}
			break;
			case 0xA2:
				if (!readframe(fd,2,&a2frame))
				{
					mvprintw(7,0,"%6ld 0xA2 %d %d\n", a2counter++,
							a2frame.language,
							a2frame.configuration);
				}
			break;
			case 0xA7:
				if (!readframe(fd,1,&a7frame))
				{
					mvprintw(8,0,"%6ld 0xA7 %d\n", a7counter++,
							a7frame.devicetype);
				}
			break;
			case 0xA4:
				if (!readframe(fd,4,&a4frame))
				{
					mvprintw(9,0,"%6ld 0xA4 %d %d\n", a4counter++,
							a4frame.valveoff,
							a4frame.valveon);
				}
			break;
			case 0xA6:
				if (!readframe(fd,4,&a6frame))
				{
					mvprintw(10,0,"%6ld 0xA6 %d\n", a6counter++,
							a6frame.fractional,
							a6frame.main);
				}
			break;
			case 0x75:
				if (!readframe(fd,6,&frame75))
				{
					mvprintw(11,0,"%6ld 0xA6 %d %d %d\n", counter75++,
							frame75.sensivity,
							frame75.outputwindow,
							frame75.windowoffset);
				}
			break;
			case 0x76:
				if (!readframe(fd,2,&frame76))
				{
					mvprintw(12,0,"%6ld 0xA6 %d\n", counter76++,
							frame76.pmtsupplyshift);
				}
			break;
			case 0x77:
				if (!readframe(fd,2,&frame77))
				{
					mvprintw(13,0,"%6ld 0xA6 %d\n", counter77++,
							frame77.pmtsupplysensivity);
				}
			break;
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
