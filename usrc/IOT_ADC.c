#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include "dirent.h"
#include "log.h"
#include "type.h"

/*****************************************************************************
* Function Name : read_adc
* Description   : read adc value 
* Input			: int ch

* Output        : None
* Return        : 0:success, -1:failed
* Auther        : simcom
* Date          : 2018.04.24
*****************************************************************************/
int read_adc(int ch)
{
	char *adcdir = "/sys/devices/";
	char adc1_path[50]={0x00};
	char adc2_path[50]={0x00};
		//get the path
	DIR *dirptr = NULL;	
	unsigned int a1;
	int fd,readcount;
	char readbuf[35];
	char *v_rest;
	char vol[5]={0};
	int voltage = 0;

	struct dirent *entry;
	if((dirptr = opendir(adcdir)) == NULL)
	{
		zlg_debug("open adc dir error! \r\n");
		return -1;
	}else
	{
		while(entry = readdir(dirptr))
		{
			if(strstr(entry->d_name, "qpnp-vadc-") != NULL)
			{
				strcpy(adc1_path,adcdir);
				strcat(adc1_path,entry->d_name);
				strcat(adc1_path,"/mpp6");

				strcpy(adc2_path,adcdir);
				strcat(adc2_path,entry->d_name);
				strcat(adc2_path,"/pa_therm1");
				break;
			}
		}
		closedir(dirptr);
	}

	if(ch == 1)
		fd = open(adc1_path,O_RDONLY);
	else if(ch == 2)
		fd = open(adc2_path,O_RDONLY);
	
	if(fd < 0)
	{
		zlg_debug("read_adc%d_value open adc file error!!!!",ch);
		return -1;
	}
	readcount = read(fd,readbuf, 35);
	if(readcount <= 0)
	{
		zlg_debug("read_adc%d_value read error!!!",ch);
		return -1;
			
	}else
	{
		v_rest = strstr(readbuf, "V:");
		(void)strncpy(vol,v_rest+2,4);
		vol[4] = '\0';
		voltage = atoi(vol);
		//printf("voltage:%dMV\r\n",voltage);
	}		
	close(fd);
	return voltage;
}

_IOT_ADC IOT_ADC = 
{
	read_adc
};
