#include "log.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
void tar_log_logic(void);
#define LOG_PATH  "/var/volatile/tmp/applog.txt"
static pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * safe_asprintf();
 */
int safe_asprintf(char **strp, const char *fmt, ...) 
{
    va_list ap;
    int retval;

    va_start(ap, fmt);
    retval = safe_vasprintf(strp, fmt, ap);
    va_end(ap);

    return retval;
}

/*
 * safe_vasprintf();
 */
int safe_vasprintf(char **strp, const char *fmt, va_list ap) 
{
    int retval;

    retval = vasprintf(strp, fmt, ap);
    if (retval == -1) 
    {
        printf("Failed to vasprintf: %s.  Bailing out\n", strerror(errno));
        return 1;
    }
    return retval;
}

/*
 * plog();
 */
void plog(const char *format, ...) 
{

    pthread_mutex_lock(&fileMutex);

    FILE *fp = NULL;
    va_list vlist;
    char *fmt = NULL;

    struct stat buf;
    int result;
    result =stat( LOG_PATH, &buf );
    if(0 == result)
    {
        if(buf.st_size>LOG_LENGTH)
        {
            //做一个日志管理处理
            tar_log_logic();
            remove(LOG_PATH);
        }
    }

    // Open debug info output file.
    if (!(fp = fopen(LOG_PATH, "a+"))) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    va_start(vlist, format);
    safe_vasprintf(&fmt, format, vlist);
    va_end(vlist);
    if (!fmt) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    time_t timep;
    struct tm *ptm = NULL;
    time(&timep);
    ptm = localtime(&timep);
    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] %s", 
            ptm->tm_year + 1900, 
            ptm->tm_mon + 1,
            ptm->tm_mday, 
            ptm->tm_hour, 
            ptm->tm_min, 
            ptm->tm_sec, 
            fmt);

    free(fmt);
    fsync(fileno(fp));
    fclose(fp);

    pthread_mutex_unlock(&fileMutex);
}

void plognotime(const char *format, ...) 
{
    pthread_mutex_lock(&fileMutex);

    FILE *fp = NULL;
    va_list vlist;
    char *fmt = NULL;

    struct stat buf;
    int result;
    result =stat( LOG_PATH, &buf );
    if(0 == result)
    {
        if(buf.st_size>LOG_LENGTH)
        {
            tar_log_logic();
            remove(LOG_PATH);
        }
    }

    // Open debug info output file.
    if (!(fp = fopen(LOG_PATH, "a+"))) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    va_start(vlist, format);
    safe_vasprintf(&fmt, format, vlist);
    va_end(vlist);
    if (!fmt) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    fprintf(fp, "%s", fmt);

    free(fmt);
    fsync(fileno(fp));
    fclose(fp);

    pthread_mutex_unlock(&fileMutex);
}

/*
 * pinfo();
 */
void pinfo(const char *format, ...) 
{
    pthread_mutex_lock(&fileMutex);

    FILE *fp = NULL;
    va_list vlist;
    char *fmt = NULL;

    struct stat buf;
    int result;
    result =stat( LOG_PATH, &buf );
    if(0 == result)
    {
        if(buf.st_size>LOG_LENGTH)
        {
            tar_log_logic();
            remove(LOG_PATH);
        }
    }

    // Open debug info output file.
    if (!(fp = fopen(LOG_PATH, "a+"))) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    va_start(vlist, format);
    safe_vasprintf(&fmt, format, vlist);
    va_end(vlist);
    if (!fmt) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    fprintf(fp, "%s", fmt);

    free(fmt);
    fsync(fileno(fp));
    fclose(fp);

    pthread_mutex_unlock(&fileMutex);
}
int my_system(char* cmd)
{
    FILE* fp = popen(cmd,"r");
    if(NULL == fp)
    {
        return -1;
    }
    pclose(fp);
}

int delFilebz2(char *basePath,int log_cnt)
{
    DIR *dir;
    struct dirent *ptr;
    char tmp[200] = {0};
    char logtmp[40] = {0};
    char cmd[128] = {'\0'}; 
    if ((dir=opendir(basePath)) == NULL)
    {
        zlg_debug("Open dir error...\r\n");
        exit(1);
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)    ///file
        {
            snprintf(tmp,199,"%s/%s",basePath,ptr->d_name);
            printf("%s\r\n",tmp);
            snprintf(logtmp,40,"log%d.tar.bz2",log_cnt);
            printf("%s\r\n",logtmp);
            if(strstr(tmp,logtmp))
            {
                snprintf(cmd,127, "rm -rf %s",tmp);
                printf("%s\r\n",cmd);
                my_system(cmd); 
            }
        }
    }
    closedir(dir);
    return 1;
}

/**
 *  函数名称：GetDateTime
 *  功能描述：取当前系统日期和时间
 *
 *  输出参数：char * psDateTime -- 系统日期时间,格式为yyymmddHHMMSS
 *  返回结果：0 -> 成功
 */
int  get_logName_time(char* psDateTime) 
{
    //zlg_debug("get_date_time\r\n");
    time_t nSeconds;
    struct tm * pTM;
    
    time(&nSeconds);
    pTM = gmtime(&nSeconds);
    //zlg_debug("nSeconds : %d\r\n",nSeconds);
    /* 系统日期和时间,格式: yyyymmddHHMMSS */
    sprintf(psDateTime, "%04d%02d%02d%02d%02d%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);
    return 0;
}



#define LOGGZ_PATH "/var/volatile/tmp/test.tar.gz"
static int LOG_CNT = 0;
void tar_log_logic(void)
{
	char cmd[128] = {'\0'}; 
    char timeTmp[30] = {0};
    snprintf(cmd,127, "tar -zcvf %s %s",LOGGZ_PATH,LOG_PATH);
    printf("%s\r\n",cmd);
    my_system(cmd); 

    delFilebz2("/media/ram",LOG_CNT%50);
    get_logName_time(timeTmp);
    snprintf(cmd,127, "tar -jcvf /media/ram/%s_log%d.tar.bz2 %s",timeTmp,LOG_CNT%50,LOGGZ_PATH);
    printf("%s\r\n",cmd);
    LOG_CNT++;
    my_system(cmd); 

    snprintf(cmd,127, "rm -rf %s",LOG_PATH);
    printf("%s\r\n",cmd);
    my_system(cmd); 

    snprintf(cmd,127, "rm -rf %s",LOGGZ_PATH);
    printf("%s\r\n",cmd);
    my_system(cmd); 
}
