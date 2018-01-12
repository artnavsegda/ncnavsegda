#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <ncurses.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <libkern/OSByteOrder.h>
#include "modbus.h"

unsigned short CRC16 (const unsigned char *nData, unsigned short wLength);

int fd;

void gameover(void)
{
        endwin();
	close(fd);
}

int main(int argc, char *argv[])
{
	mbframestruct mbframe = {
		.unitid = 50,
		.fncode = 3,
		.data.askreadregs.firstreg = htons(360),
		.data.askreadregs.regnumber = htons(2)
	};
	mbframestruct askframe;
        struct termios tio = {
                .c_cflag = B9600 | CS8 | CLOCAL | CREAD,
                .c_iflag = IGNPAR,
                .c_oflag = 0,
                .c_lflag = 0,
                .c_cc[VTIME] = 1,
                .c_cc[VMIN] = 50
        };

	if (argc == 1)
	{
                printf("name serial port\n");
                exit(1);
        }

	fd = open(argv[1], O_RDWR | O_NOCTTY );
        if (fd  == -1)
        {
                perror("cannot open serial port");
                exit(1);
        }
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&tio);

	mbframe.checksum = CRC16((unsigned char *)&mbframe,6);

	printf("checksum %02X\n",mbframe.checksum);

	initscr();
	noecho();
	curs_set(FALSE);
	keypad(stdscr, TRUE);
	atexit(gameover);

	while (1)
	{
	clear();
  	ssize_t numwrite1 = write(fd,&mbframe,6);
  	if (numwrite1 == -1)
  	{
  		perror("send request error");
  		exit(1);
  	}
  	ssize_t numwrite2 = write(fd,&mbframe.checksum,2);
  	if (numwrite2 == -1)
  	{
  		perror("send checksum error");
  		exit(1);
  	}
  	mvprintw(1,1,"send %zd request and %zd checksum ok\n", numwrite1, numwrite2);

  	ssize_t numread = read(fd,&askframe,255);
  	if (numread == -1)
  	{
  		perror("recv error");
  		close(fd);
  		return 1;
  	}
  	else
  	{
                mvprintw(7,7,"recieved CRC 0x%04hX\n",((short *)&askframe.data.reqread.bytes)[1+((numread-6))/2]);
                mvprintw(8,8,"calculated CRC 0x%04hX\n",CRC16((unsigned char *)&askframe,numread-2));

		((short *)&askframe.data.reqread.bytes)[0] = OSSwapHostToBigInt16(((short *)&askframe.data.reqread.bytes)[0]);
		((short *)&askframe.data.reqread.bytes)[1] = OSSwapHostToBigInt16(((short *)&askframe.data.reqread.bytes)[1]);

		mvprintw(10,10,"0x%04hX 0x%04hX",((short *)&askframe.data.reqread.bytes)[0], ((short *)&askframe.data.reqread.bytes)[1]);
		mvprintw(11,11,"0x%08X",((int *)&askframe.data.reqread.bytes)[0]);
		mvprintw(12,12,"%f",((float *)&askframe.data.reqread.bytes)[0]);

  		mvprintw(2,2,"recv %zd bytes\n",numread);
  		mvprintw(3,3,"Unit id: %d\n", askframe.unitid);
  		mvprintw(4,4,"Function code: %d\n", askframe.fncode);
                  switch(askframe.fncode)
                  {
                  case 1:
                  case 2:
                          mvprintw(5,5,"number of bytes containing bit values: %d\n",askframe.data.reqread.bytestofollow);
                  break;
                  case 3:
                  case 4:
                          mvprintw(5,5,"number of registers: %d\n",askframe.data.reqread.bytestofollow/2);
                  break;
                  case 5:
                  case 6:
                          mvprintw(5,5,"register number %u\n", ntohs(askframe.data.writereg.regaddress));
                          mvprintw(6,6,"register value %u\n", ntohs(askframe.data.writereg.regvalue));
                  break;
                  case 15:
                  case 16:
                          mvprintw(5,5,"first register %u\n", ntohs(askframe.data.writemulticoil.firstreg));
                          mvprintw(6,6,"registers amount %u\n", ntohs(askframe.data.writemulticoil.regnumber));
                  break;
                  default:
                          mvprintw(5,5,"unknown function number\n");
                  break;
                  }
  	}
	refresh();
  	sleep(1);
	}

	close(fd);

	return 0;
}
