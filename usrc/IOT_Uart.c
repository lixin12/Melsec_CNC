#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <sys/syslog.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <linux/input.h>
#include "log.h"
//#include "type.h"

int bitrate_is_ok(int nSpeed)
{
	switch(nSpeed)
	{
		case 300:
		case 600:
		case 1200:
		case 2400:
		case 4800:
		case 9600:
		case 19200:
		case 38400:
		case 57600:
		case 115200:
		case 230400:
		case 460800:
		case 921600:
			return 1;
	}
	return 0;
}
static int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop,unsigned int cflag)
{
	struct termios newtio, oldtio;
	if(tcgetattr(fd, &oldtio) != 0)
	{
		zlg_debug("setupserial 1\n");
		zlg_debug("errno=%d\n", errno);
		//zlg_debug("errno=%s\n", strerror(errno));
		return -1;
	}
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag |=CLOCAL | CREAD |cflag;
	//newtio.c_cflag &= ~CSIZE;
	switch(nBits)
	{
		case 7:
			newtio.c_cflag |=CS7;
			break;
		case 8:
			newtio.c_cflag |=CS8;
			break;
	}
	switch(nEvent)
	{
		case 'O':
			newtio.c_cflag |=PARENB;
			newtio.c_cflag |=PARODD;
			newtio.c_iflag |=(INPCK);
			break;
		case 'P':
			newtio.c_cflag |=PARENB;
			newtio.c_cflag |=CMSPAR;
			newtio.c_iflag |=(INPCK);
			break;
		case 'E':
			newtio.c_iflag |=(INPCK);
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~ (PARODD|CMSPAR);
			break;
		case 'N':
			newtio.c_cflag &= ~PARENB;
			break;
	}

	switch(nSpeed)
	{
		case 300:
			cfsetispeed(&newtio, B300);
			cfsetospeed(&newtio, B300);
			break;
		case 600:
			cfsetispeed(&newtio, B600);
			cfsetospeed(&newtio, B600);
			break;
		case 1200:
			cfsetispeed(&newtio, B1200);
			cfsetospeed(&newtio, B1200);
			break;
		case 2400:
			cfsetispeed(&newtio, B2400);
			cfsetospeed(&newtio, B2400);
			break;
		case 4800:
			cfsetispeed(&newtio, B4800);
			cfsetospeed(&newtio, B4800);
			break;
		case 9600:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
		case 19200:
			cfsetispeed(&newtio, B19200);
			cfsetospeed(&newtio, B19200);
			break;
		case 38400:
			cfsetispeed(&newtio, B38400);
			cfsetospeed(&newtio, B38400);
			break;
		case 57600:
			cfsetispeed(&newtio, B57600);
			cfsetospeed(&newtio, B57600);
			break;
		case 115200:
			cfsetispeed(&newtio, B115200);
			cfsetospeed(&newtio, B115200);
			break;
		case 230400:
			cfsetispeed(&newtio, B230400);
			cfsetospeed(&newtio, B230400);
			break;
		case 460800:
			cfsetispeed(&newtio, B460800);
			cfsetospeed(&newtio, B460800);
			break;
		case 921600:
			cfsetispeed(&newtio, B921600);
			cfsetospeed(&newtio, B921600);
			break;
		default:
			cfsetispeed(&newtio, B115200);
			cfsetospeed(&newtio, B115200);
			break;
	}
	if( nStop == 1)
		newtio.c_cflag &= ~CSTOPB;
	else if(nStop == 2)
		newtio.c_cflag |=CSTOPB;

	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 1;
	tcflush(fd, TCIFLUSH);
	tcflush(fd, TCOFLUSH);
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		zlg_debug("com set error\n");
		return -1;
	}

	return 0;
}
static int fd = -1;
struct pollfd port_pollfd[1];
char devpath[100]="/dev/ttyHSL1";
char uart_revData[2048] = {0};
int uart_Init(int baudrate)
{
    fd = open(devpath, O_RDWR|O_NOCTTY | O_NDELAY |O_NONBLOCK);
	if (fd < 0){
		zlg_debug("can't open device\n");
		return -1;
	}
    zlg_debug("uart init %s,speed,%d\n",devpath,baudrate);
	set_opt(fd, baudrate, 8, 'N', 1,0);
	port_pollfd[0].fd = fd;
	port_pollfd[0].events = POLLIN;
	port_pollfd[0].revents = 0;
	return 0;
}

char* uart_revDataTimeOut(int baud,int timeout)
{
    int res,ret;
    int totalCnt = 0;
	if(fd>0){}
	else
	{
		uart_Init(baud);
	}
    while(1)
    {
        ret = poll(port_pollfd,1,timeout);
        if(ret==0)//超时
        {
			uart_revData[totalCnt+2] = 0;
			uart_revData[0] = (totalCnt>>8)&0xff;
			uart_revData[1] = totalCnt&0xff;
			return uart_revData;
        }
        else if(ret>0)//接收到数据
        {
			if(port_pollfd[0].revents & POLLIN)
			{
				res = read(fd,uart_revData+totalCnt+2,sizeof(uart_revData)-totalCnt-2);
				if(res < 0)
				{
					close(fd);
					return 0;// read error
				}
				else
				{
					totalCnt += res;
				}
			}
        }
        else//出错
        {
			close(fd);
			return 0;
        }
    }
}
void uart_dataLog(char* tag,char* data,int length)
{
	int i = 0;
	char buf[1024] = {0};
	for(i=0 ; i<length ; i++)
	{
		sprintf(buf+i*3,"%2x ",data[i]);
	}
	sprintf(buf+i*3,"\r\n");
	zlg_debug(buf);
}
int uart_SendData(int baud,char* data,int length)
{
	if(fd>0)
	{}
	else
	{
		uart_Init(baud);
	}
	//uart_dataLog("",data,length);
	return write(fd,data,length);
}


int test485(u32 baud)
{
	int i = 0;
	char* data = 0;
	uart_SendData(baud,"1234",strlen("1234"));

	data = uart_revDataTimeOut(baud,2000);
	if(data!=0)
	{
		if((data[0] | (data[1]<<8))>0)
		{
			for(i=0 ; i<(data[0] | (data[1]<<8)) ; i++)
			{
				printf("%2x ",data[2+i]);
			}
			printf("\r\n");
			uart_SendData(baud,data+2,(data[0] | (data[1]<<8)));
		}
	}

	return 0;
}

_IOT_UART IOT_Uart = 
{
	uart_revDataTimeOut,
	uart_SendData,
	uart_dataLog,
	test485
};
