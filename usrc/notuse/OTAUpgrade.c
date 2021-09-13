#include "OTAUpgrade.h"

/*****************************************************************************
* Function Name : downloadFile
* Description   : 下载文件
* Input			: int flag,
*                 char *fileName;
*                 char *destination,

* Output        : None
* Return        : 0:success, -1:failed
* Auther        : simcom
* Date          : 2018.04.14
*****************************************************************************/
#if 0
int ota_downloadFile(char *FTP_IP, int Port, char *FTP_UsrName, char *FTP_Password,char *fileName, char *destination)
{
    int Pasvfd;
    int PasvPort;
    char PasvIp[32];

    memset(PasvIp, 0, sizeof(PasvIp));
    ftp_init(FTP_IP, Port);
    //FTPClient ftpClient("FTP_IP", "Port");
    if(loginFTPServer(FTP_UsrName, FTP_Password) == -1)
        return -1;
    if(setFTPServerToPasvMode(PasvIp, &PasvPort) == -1)
        return -1;
    if(connectPasvServer(PasvIp, PasvPort, &Pasvfd) != -1)
    {
        if((fileName != NULL) && (destination != NULL))
        {
            printf("download lte version fileName:%s, destination:%s\n",fileName, destination);
            if(getFile(Pasvfd, fileName, destination) != 0)
            {
                printf("download fail\n");
            }

            quitFTP();
        }
        else
        {
            return -1;
        }
    }

    return 0;
}
#endif

void* do_enter_recovery_reset(void* args)
{   
    sleep(2);    
    syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_RESTART2, "recovery");
}

static void do_enter_backup_reset(void)
{    
    sleep(2);    
    system("reboot");
}

static int enter_recovery_mode_delay(void)
{
    pthread_t id;
    int ret;
    ret = pthread_create(&id, NULL, do_enter_recovery_reset, NULL);
	
    return 0;
}

int ota_update()
{
    printf("system_update in\n");	//set flag to send DM session(non fota update).	FILE  *fp = NULL;	//system("mkdir -p /data/dme");
    //system("touch /data/dme/start_DM_session");
    fp = fopen("/data/dme/start_DM_session", "w+");	
    if(fp == NULL)    {        return -1;    }
  
    else
  
    {
    
        fclose(fp);
    system("sync");		
        /*modify to not delete temp files		
        system("rm /data/dme/DMS.tre");		
        system("rm /data/dme/DevDetail.tre");	
        system("rm /data/dme/DevInfo.tre");		
        system("rm /data/dme/aclData.acl");		
        system("rm /data/dme/eventlist.cfg");		
        system("rm /data/dme/init_active_date_file");		
        system("rm /data/dme/rs_log.txt");		
        system("rm /data/dme/session.log");*/		
    }    system("echo off > /sys/power/autosleep"); 	
    (void)enter_recovery_mode_delay();	
    return 0;	
}

