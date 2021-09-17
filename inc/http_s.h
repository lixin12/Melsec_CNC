#ifndef HTTP_S_H
#define HTTP_S_H
#include <stdio.h>


int http_get(char *url, char *rename);
int https_get(char *url, char *rename, int ca_flag, char*capath, char *cainfo);
#endif

