#ifndef _OAT_UPGRADE_H_
#define _OAT_UPGRADE_H_

#include <sys/time.h>
#include <sys/select.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include "common.h"
#include <syscall.h>
#include <sys/ioctl.h>
#include <linux/reboot.h>

int ota_downloadFile(char *FTP_IP, int Port, char *FTP_UsrName, char *FTP_Password,char *fileName, char *destination);
int ota_update();

#endif
