#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <linux/input.h>
#include <linux/ioctl.h>
#include <pthread.h>
#include "log.h"
#define MAX_BUF 128

int gpio_file_create(int gpio)
{
	int sfd = -1;
	char checkstr[50] = {0};
	char configstr[10] = {0};
	int reto,len;

	sprintf(checkstr,"/sys/class/gpio/gpio%d/value",gpio);
	if(0 == access(checkstr, F_OK)){
		return 0;
	}

	sfd = open("/sys/class/gpio/export",O_WRONLY);
	if(sfd < 0){
		zlg_debug("open file error,%d\n",sfd);
		return sfd;
	}

	len = sprintf(configstr,"%d",gpio);
	reto = write(sfd,configstr,len);

	if(reto != len){
		zlg_debug("create gpio(%d) files:%d,%d,%d\n",gpio,__LINE__,len,reto);
		return reto;
	}
	usleep(1000);
	reto = access(checkstr, F_OK);
	if(0 > reto){
		zlg_debug("%s:%d,gpio file(%s)not exist\n",__FUNCTION__,__LINE__,checkstr);
	}

	close(sfd);
	return reto;
}
int gpio_set_dir(unsigned int gpio, unsigned int out_flag)
{
	int fd;
	char buf[MAX_BUF];
 
	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/direction", gpio);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		zlg_debug("gpio(%d)/direction\n",gpio);
		return fd;
	}
 
	if (out_flag)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);
 
	close(fd);
	return 0;
}
int gpio_set_value(unsigned int gpio, unsigned int value)
{
	int fd;
	char buf[MAX_BUF];
 
	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", gpio);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		zlg_debug("gpio/set-value\n");
		return fd;
	}
 
	if (value)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);
 
	close(fd);
	return 0;
}
#define DI1_GPIO      (26)
int gpio_get_value(unsigned int gpio)
{
    int fd;
	char buf[MAX_BUF];
    char gpio_value;
    int res = 0;
	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", gpio);
	fd = open(buf, O_RDONLY|O_NONBLOCK);
	if (fd < 0) {
		perror("gpio/set-value\n");
		return fd;
	}
	res = read(fd, &gpio_value, 1);
    if(res<0)
    {
        zlg_debug("read gpio value error : %d\r\n",res);
        return res;
    }
	close(fd);
    if(gpio_value==0 || gpio_value=='0')
    {
        return 0;
    }
    if(gpio_value==1 || gpio_value=='1')
    {
        return 1;
    }
}

#define LED_LTE_GPIO  (18)
#define LED_LED1_GPIO (78)
#define LED_LED2_GPIO (79)
#define WATCHDOG_GPIO (77)
#define DO1_GPIO      (76)

#define DI1_GPIO      (26)

typedef enum{IN=0,OUT=1}DIR_TYPE;

int IOT_GPIO_Init(void)
{
    if(0!=gpio_file_create(LED_LTE_GPIO)) 
    {
        zlg_debug("gpio_file_create(LED_LTE_GPIO) error");
        return -1;
    }
    if(0!=gpio_set_dir(LED_LTE_GPIO,OUT))
    {
        zlg_debug("gpio_set_dir(LED_LTE_GPIO,OUT) error");
        return -1;
    }

    if(0!=gpio_file_create(LED_LED1_GPIO)) 
    {
        zlg_debug("gpio_file_create(LED_LED1_GPIO) error");
        return -1;
    }
    if(0!=gpio_set_dir(LED_LED1_GPIO,OUT))
    {
        zlg_debug("gpio_set_dir(LED_LED1_GPIO,OUT) error");
        return -1;
    }
    if(0!=gpio_file_create(LED_LED2_GPIO)) 
    {
        zlg_debug("gpio_file_create(LED_LED2_GPIO) error");
        return -1;
    }
    if(0!=gpio_set_dir(LED_LED2_GPIO,OUT))
    {
        zlg_debug("gpio_set_dir(LED_LED2_GPIO,OUT) error");
        return -1;
    }

    if(0!=gpio_file_create(WATCHDOG_GPIO)) 
    {
        zlg_debug("gpio_file_create(WATCHDOG_GPIO) error");
        return -1;
    }
    if(0!=gpio_set_dir(WATCHDOG_GPIO,OUT))
    {
        zlg_debug("gpio_set_dir(WATCHDOG_GPIO,OUT) error");
        return -1;
    }

    if(0!=gpio_file_create(DO1_GPIO)) 
    {
        zlg_debug("gpio_file_create(DO1_GPIO) error");
        return -1;
    }
    if(0!=gpio_set_dir(DO1_GPIO,OUT))
    {
        zlg_debug("gpio_set_dir(DO1_GPIO,OUT) error");
        return -1;
    }

    if(0!=gpio_file_create(DI1_GPIO)) 
    {
        zlg_debug("gpio_file_create(DI1_GPIO) error");
        return -1;
    }
    if(0!=gpio_set_dir(DI1_GPIO,IN))
    {
        zlg_debug("gpio_set_dir(DI1_GPIO,IN) error");
        return -1;
    }

    gpio_set_value(LED_LTE_GPIO,1);
    gpio_set_value(LED_LED1_GPIO,1);
    gpio_set_value(LED_LED2_GPIO,1);
    gpio_set_value(WATCHDOG_GPIO,0);
    gpio_set_value(DO1_GPIO,0);
//void nas_dinit();
//int get_SignalStrength(int *level, int* mode);
}
void Feed_Watchdog(void)
{
    gpio_set_value(WATCHDOG_GPIO,0);
    usleep(1000);
    gpio_set_value(WATCHDOG_GPIO,1);
    usleep(10000);
    gpio_set_value(WATCHDOG_GPIO,0);
}
_IOT_LED IOT_LED=
{
    IOT_GPIO_Init,
    gpio_set_value,
    gpio_get_value,
    Feed_Watchdog,
    LED_LTE_GPIO,
    LED_LED1_GPIO,
    LED_LED2_GPIO,
    WATCHDOG_GPIO,
    DO1_GPIO     ,
    DI1_GPIO     
};


