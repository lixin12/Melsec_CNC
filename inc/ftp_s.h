#ifndef FTP_S_H
#define FTP_S_H
#include <stdio.h>

int ftp_get(char *url, char *rename, char *usr, char *pwd);
//FTPS
int ftps_get(char *url, char *rename, char *usr, char *pwd);

#endif