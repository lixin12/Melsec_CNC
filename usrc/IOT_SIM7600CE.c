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
static char IMEI[20] = {0};

void SIM7600CE_Init(void)
{
    int res = 0;
    int failCnt = 0;
    do
    {
        dms_init();
        res = get_imei(IMEI);
        if(0==res)
        {
            zlg_debug("IMIE : %s\r\n",IMEI);
        }
        else
        {
            failCnt++;
            zlg_debug("IMIE : ERROR!\r\n");
            if(failCnt>10)
            {
                zlg_debug("IMIE : ERROR more than 10!\r\n");
                return;
            }
        }
    }while(0!=res);
    
}
char* Get_IMEI(void)
{
    return IMEI;
}

_IOT_SIM7600CE IOT_SIM7600CE = 
{
	SIM7600CE_Init,
    Get_IMEI
};

