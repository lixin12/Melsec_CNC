#ifndef _AT_CONTROL_H_
#define _AT_CONTROL_H_
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include "common.h"

#define SIMCOM_AT_PORT    "/dev/smd8"
#define ARRAY_SIZE      100


    int atctrl_init();
	void atctrl_deinit();
	static void *timingTaskThread(void *args);
	
	/***************************************************************************
	* Function Name: sendATCmd 					
	* Function Introduction: send at command to LTE module
	* Parameter description:
	*     atCmd    : at command
	*     finalRsp : at command's response result.
	*     buff     : if need function return result, you should pass the buff,
	*                then the function will store the result into buff 
	*     buffSize : buff size, At least 100bits.
	*     timeout_ms: time out
	* Function return value:  -1: failed; 0: return error,
	*                         other: return data length.
	* Data : 2017.09.08									
	****************************************************************************/
	int sendATCmd(char* atCmd, char* finalRsp, char *buff, uint32_t buffSize, long timeout_ms);
	int getModuleRevision(char *pBuff, int size);
	int getCPIN();
	int getCSQ(uint8_t *rssi);
	int getCREG();
	int getICCID(char *pBuff, int size);
	int getIMEI(char *pBuff, int size);
	int getCIMI(char *pBuff, int size);
	int audioPlayTTS(char *tts);

#endif


