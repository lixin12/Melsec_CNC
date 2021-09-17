#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include "type.h"


void plog(const char *format, ...) ;
void plognotime(const char *format, ...); 
#define LOG_LENGTH  (1024*1000*5)

#if ZLG_DEBUG == 4
#define zlg_debug(fmt, args...) do{plog("%s(%d)-%s: ",__FILE__,__LINE__,__FUNCTION__);plognotime(fmt,##args);}while(0)
#elif ZLG_DEBUG == 2
#define zlg_debug(fmt, args...) do{printf("%s(%d)-%s: ",__FILE__,__LINE__,__FUNCTION__);printf(fmt,##args);}while(0)
#elif ZLG_DEBUG == 3
#define zlg_debug(fmt, args...) do{plog(fmt,##args);}while(0)
#elif ZLG_DEBUG == 1
#define zlg_debug(fmt, args...) do{printf(fmt,##args);}while(0)
#else
#define zlg_debug(fmt, args...) do{}while(0)
#endif

#endif
