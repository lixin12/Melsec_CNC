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
		case 1000000:
		case 1152000:
		case 1500000:
		case 2000000:
		case 3000000:
		case 3200000:
		case 3686400:
			return 1;
	}
	return 0;
}
static int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop,unsigned int cflag)
{
	struct termios newtio, oldtio;
	if(tcgetattr(fd, &oldtio) != 0)
	{
		perror("setupserial 1\n");
		printf("errno=%d\n", errno);
		//printf("errno=%s\n", strerror(errno));
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
		case 1000000:
			cfsetispeed(&newtio, B1000000);
			cfsetospeed(&newtio, B1000000);
			break;
		case 1152000:
			cfsetispeed(&newtio, B1152000);
			cfsetospeed(&newtio, B1152000);
			break;
		case 1500000:
			cfsetispeed(&newtio, B1500000);
			cfsetospeed(&newtio, B1500000);
			break;
		case 2000000:
			cfsetispeed(&newtio, B2000000);
			cfsetospeed(&newtio, B2000000);
			break;
		case 3000000:
			cfsetispeed(&newtio, B3000000);
			cfsetospeed(&newtio, B3000000);
			break;
		case 3200000:
			cfsetispeed(&newtio, B200);
			cfsetospeed(&newtio, B200);
			break;
		case 3686400:
			cfsetispeed(&newtio, B150);
			cfsetospeed(&newtio, B150);
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
		perror("com set error\n");
		return -1;
	}

	return 0;
}
static inline char * getdevname(char *path)
{
	char *ptr;

	if (path == NULL)
		return NULL;

	ptr = path;

	while (*ptr != '\0') {
		if ((*ptr == '\\') || (*ptr == '/'))
			path = ptr + 1;

		ptr++;
	}

	return path;
}

int uart_wakeup(char* devpath,unsigned char bwakeup)
{
	int ufd = -1;
	char powrpath [256] = {0};
	sprintf(powrpath,"/sys/class/tty/%s/device/power/control",getdevname(devpath));
	ufd = open(powrpath, O_WRONLY);
	if (ufd < 0) {
		printf("%s error %d\n", powrpath,ufd);
		return -1;
	}else{
		if(bwakeup){
			write(ufd,"on" ,2 );
		}else{
			write(ufd,"auto" ,4 );
		}
		close(ufd);
	}
	return 0;
}
/*
ret = read(fd,&scan_string[read_index],sizeof(scan_string)-read_index);
if(ret <= 0){
	continue;
}
write(fd,&scan_string[read_index],ret);
if(cncar = strchr(&scan_string[read_index],'\n')){
	read_index = ret-(cncar - &scan_string[read_index]) + 1;
	*cncar = '\0';
	printf(scan_string);
	printf("\n");
	strncmp(scan_string,cncar + 1,read_index);
}else{
	read_index += ret;
}
*/
void uart_main(void)
{
    char scan_string[100] = {0};
	int read_index = 0;
	char devpath[100]="/dev/ttyHS1";
	int uart_baudrate = 115200;
	int fd = -1;
	struct pollfd port_pollfd[2];
	FILE* fserial = NULL;
 	int ret = -1;
	
_start_:
	printf("\nPlease enter device (/dev/ttyHS1):");
	fgets(scan_string, sizeof(scan_string), stdin);
	if(scan_string[0] != '\n'){
		char* strip_s = &scan_string[0];
		while((*strip_s != '\r') && (*strip_s != '\n')) strip_s ++;
		*strip_s = '\0';
		if(0 != access(scan_string, F_OK)){
			printf("device %s not exist\n",scan_string);
			goto _start_;
		}
		strncpy(devpath,scan_string,sizeof(devpath));
	}
	
	printf("\nPlease input bitrate (115200):");
	fgets(scan_string, sizeof(devpath), stdin);
	if(scan_string[0] != '\n'){
		int nbitrate = atoi(scan_string);
		if(!bitrate_is_ok(nbitrate)){
			printf("%d bitrate not support\n",nbitrate);
			goto _start_;
		}
		uart_baudrate = nbitrate;
	}
	
	fd = open(devpath, O_RDWR|O_NOCTTY | O_NDELAY |O_NONBLOCK);
	if (fd < 0){
		perror("can't open device\n");
		return;
	}
	uart_wakeup(devpath,1);
	printf("uart init %s,speed,%d\n",devpath,uart_baudrate);
	set_opt(fd, uart_baudrate, 8, 'N', 1,0);
	port_pollfd[0].fd = fd;
	fserial = fdopen(fd, "r+");
	port_pollfd[0].events = POLLIN;
	port_pollfd[0].revents = 0;

	port_pollfd[1].fd = STDIN_FILENO;
	port_pollfd[1].events = POLLIN;
	port_pollfd[1].revents = 0;
	printf("Quit:+++exit\n");
	while(1){
 		printf("$>");
		poll(port_pollfd,2,-1);
		if(port_pollfd[0].revents & POLLIN){
			char cchar = 0;
			unsigned char bwaitn = 0; 
			port_pollfd[0].revents = 0;
			while(1){
				ret = read(fd,&cchar,1);
				if(ret <= 0){
					continue;
				}
				write(fd,&cchar,1);
				scan_string[read_index ++] = cchar;
				if((cchar == '\r') || (cchar == '\n')){
					scan_string[read_index ++] = '\n';
					scan_string[read_index ++] = 0;
					printf(scan_string);
					write(fd,"\n",1);
					read_index = 0;
					break;
				}
				if(read_index == sizeof(scan_string) -1)
				{
					printf(scan_string);
					read_index = 0;
				}
			}
		}
		if(port_pollfd[1].revents & POLLIN){
			port_pollfd[1].revents = 0;
			if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
			{
				continue;
			}
			if(!strncmp(scan_string,"+++exit",7))
				break;
			fputs(scan_string,fserial);
		}
	}
	fclose(fserial);
	close(fd);
	uart_wakeup(devpath,0);
}

