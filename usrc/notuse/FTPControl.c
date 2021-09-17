#include "FTPControl.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <pthread.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

static simcom_ftp_info ftp_info;
static pthread_mutex_t s_commandmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t s_commandcond = PTHREAD_COND_INITIALIZER;


int get_connect_status()
{
    int bConnected = 0;
    pthread_mutex_lock(&s_commandmutex);
    if(ftp_info.ctrl_sock > 0)
    {
        bConnected = 1;
    }
    pthread_mutex_unlock(&s_commandmutex);
    return bConnected;
}

int get_init_status()
{
    int bInit = 1;
    pthread_mutex_lock(&s_commandmutex);
    if(ftp_info.init == 0)
    {
        bInit = 0;
    }
    pthread_mutex_unlock(&s_commandmutex);
    return bInit;    
}

static int set_trans_status(int status)
{
    pthread_mutex_lock(&s_commandmutex);
    ftp_info.bTransOK = status ? 1 : 0;
    pthread_mutex_unlock(&s_commandmutex);
}

static int get_trans_status()
{
    int status;
    pthread_mutex_lock(&s_commandmutex);
    status = ftp_info.bTransOK;
    pthread_mutex_unlock(&s_commandmutex);
    return status;
}

static int get_description_string(char *data, int code, ftp_ctrl_resp_type *pctrl_resp)
{
    char flag[5] = {0};
    sprintf(flag, "%d ", code);
    char *p;
    int i;
    p = strstr(data, flag);
    if(p != NULL)
    {
        i = 0;
        p+=4;
        while(*p != 0x0D && *p != 0 && i < 1000)
        {
            pctrl_resp->resp_str[i] = *p;
            p++;
            i++;
        }
    }
}
static int process_ctrl_data(char *data)
{
    int i,j,k;
    int n;
    char tmp_buf[1024];
    ftp_ctrl_resp_type ctrl_resp;
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));
    int resp_code[MAX_RESP_CODE_NUM] = {0};
    char *p = data;
    int  bIgnore = 0;
    //printf("recv_ctrl_data: %s\n", data);
    for(i = 0; i < 10; i++)
    {   
        if(strlen(p) > 0)
        {
            sscanf(p, "%[^\r\n]", tmp_buf);
           // printf("%s\n", tmp_buf);
            
            sscanf(tmp_buf, "%d", &resp_code[i]);
            //printf("resp_code[%d]=%d\n",i, resp_code[i]);
        }
        p = strstr(p,"\r\n");
        if(p == NULL)
        {
            break;
        }
        
        p = p+=2;
    }
   
    
    j = 0;
    for(i = 0; i < 10; i++)
    {
        bIgnore = 0;
        if(resp_code[i] == 0)
        {
            break;
        }
        switch(resp_code[i])
        {
            case 220:
                bIgnore = 1;
                break;
            case 257:
                {
                    p = strstr(data,"257 ");
                    if(p != NULL)
                    {
                        k = 0;
                        p+=5;
                        while(*p != '\"')
                        {
                            ctrl_resp.resp_str[k] = *p;
                            p++;
                            k++;
                        }
                    }
                    else
                    {
                        ctrl_resp.resp_str[0] = '/';
                    }
                }
                break;
            case 211:
                bIgnore = 1;
                break;               
            case 212:
            case 213:
            case 227:
                get_description_string(data, resp_code[i], &ctrl_resp);
                break;
            case 226:
                set_trans_status(1);
                break;
            default:
                
                break;
        }

        if(bIgnore)
        {
            resp_code[i] = 0;
        }
        else
        {
            ctrl_resp.resp_code[j] = resp_code[i];
            j++;
        }
    }

    if(ctrl_resp.resp_code[0] > 0)
    {
        pthread_mutex_lock(&s_commandmutex);
        //printf("recv ctrl resp notify\n");
        memset(&ftp_info.ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));
        memcpy(&ftp_info.ctrl_resp, &ctrl_resp, sizeof(ftp_ctrl_resp_type));
        pthread_cond_signal(&s_commandcond);
        pthread_mutex_unlock(&s_commandmutex);
    }
}

static int ctrl_recv_fun(void *data)
{
    int ctrl_sock = ftp_info.ctrl_sock;
    int len;
    int selres;
    fd_set rfds, wfds;
    struct timeval tv;
    char buf[1024];
    FD_ZERO(&rfds);
    FD_SET(ctrl_sock, &rfds);
    pthread_detach(pthread_self());
    if(ctrl_sock < 0)
    {
        return -1;
    }
    
    while(1)
    {
        tv.tv_sec = 10;
        tv.tv_usec = 0;   
        FD_ZERO(&rfds);
        FD_SET(ctrl_sock, &rfds);    
        selres = select(ctrl_sock + 1, &rfds, NULL, NULL, &tv);

        if(selres == -1)
        {
            if(errno == EINTR || errno == EAGAIN)
            {
                continue;
            }
            else
            {
                //printf("ctrl_recv_fun select error\n");
                break;
            }
        }
        else if(selres == 0)
        {
            continue;
        }
        else
        {
            if (FD_ISSET(ctrl_sock, &rfds))
            {
                memset(buf, 0, sizeof(buf));
                len = recv(ctrl_sock, buf, 1024, 0);
                if(len == 0)
                {
                    printf("server socket closed\n");
                    break;
                }
                else
                {
                    process_ctrl_data(buf);
                }
            } 
            else
            {
                continue;
            }
        }     
    }
    close(ctrl_sock);
    pthread_mutex_lock(&s_commandmutex);
    ftp_info.ctrl_sock = -1;  
    pthread_mutex_unlock(&s_commandmutex);
    //printf("ctrl sock thread exit\n");
    return 0;
}

static int create_ctrl_recv_thread()
{
    pthread_t ctrl_recv_thread;

    if(0 != pthread_create(&ctrl_recv_thread, NULL,ctrl_recv_fun, NULL))
    {
        printf("create recv thread thread fail\n");
        return -1;
    }
    return 0;
}



int ftp_connect()
{
    struct sockaddr_in address;
    int ctrl_sock;
    int opvalue;
    socklen_t slen;
    int     flags;
    int     res;
    char *host = ftp_info.host;
    int port = ftp_info.port;
    int ret;

    printf("host: %s, port: %d, usr: %s pwd: %s\n", host, port, ftp_info.user, ftp_info.pwd);    
    memset(&address, 0, sizeof(address));
  
    if ((ctrl_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create socket fail\n");
        return -1;
    }
    //struct timeval timeo = {10, 0};
    //setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
    //setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));

    

    flags = fcntl(ctrl_sock, F_GETFL, 0);
    fcntl(ctrl_sock, F_SETFL, flags|O_NONBLOCK);

    address.sin_family = AF_INET;
    address.sin_port = htons((unsigned short)port);

    #if 0
    struct hostent* server = gethostbyname(host);
    if (!server)
    {
        printf("get ip fail\n");
        ftp_info.ctrl_sock = -1;
        close(ctrl_sock);
        return -1;
    }
    memcpy(&address.sin_addr.s_addr, server->h_addr, server->h_length);
    #else
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(host);
    #endif
    
    ret = connect(ctrl_sock, (struct sockaddr*) &address, sizeof(address));
    if(ret == 0)
    {
        ftp_info.ctrl_sock = ctrl_sock;
        printf("connect success immediately!\n");
        return 0;
    }
    
    else if (errno == EINPROGRESS)
    {
        int times = 0;

        //printf("errno == EINPROGRESS\n");
        printf("Connecting...\n");
        while (1)
        {
            fd_set rfds, wfds;
            struct timeval tv;        
            FD_ZERO(&rfds);
            FD_ZERO(&wfds);
            FD_SET(ctrl_sock, &rfds);
            FD_SET(ctrl_sock, &wfds);

            /* set select() time out */
            tv.tv_sec = 10;
            tv.tv_usec = 0;
            int selres = select(ctrl_sock + 1, &rfds, &wfds, NULL, &tv);
            if(selres == -1)
            {
                if(errno == EINTR || errno == EAGAIN)
                {
                    continue;
                }
                ret = -1;
                break;
            }
            else if(selres == 0)
            {
                printf("select time out\n");
                ret = -1;
                break;
            }
            else
            {
                if (FD_ISSET(ctrl_sock, &rfds) || FD_ISSET(ctrl_sock, &wfds))
                {
                    connect(ctrl_sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in));
                    if(errno == EISCONN)
                    {
                        printf("EISCONN connect success.\n");
                        ret = 0;
                    }
                    else if(errno == EINPROGRESS)
                    {
                        printf("Connecting.....\n");
                        continue;
                    }
                    else
                    {
                        printf("connect failed. errno = %d\n", errno);
                        //printf("read: %d  write: %d\n", FD_ISSET(ctrl_sock, &rfds) , FD_ISSET(ctrl_sock, &wfds));
                        ret = -1;
                    }
                }
                else
                {
                    ret = -1;
                }
            }
            break;
        }    
    }

    if(ret == 0)
    {
        ftp_info.ctrl_sock = ctrl_sock;
        create_ctrl_recv_thread();
        return 0;
    }
    else
    {
        ftp_info.ctrl_sock = -1;
        close(ctrl_sock);
        return -1;
    }
}


static int ftp_data_connect(char *host,int port)
{
    struct sockaddr_in address;
    int s, opvalue;
    socklen_t slen;
    
    opvalue = 8;
    slen = sizeof(opvalue);
    memset(&address, 0, sizeof(address));
    
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ||
        setsockopt(s, IPPROTO_IP, IP_TOS, &opvalue, slen) < 0)
        return -1;
    
    struct timeval timeo = {10, 0};
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));
    
    address.sin_family = AF_INET;
    address.sin_port = htons((unsigned short)port);
    #if 0
    struct hostent* server = gethostbyname(host);
    if (!server)
        return -1;
    memcpy(&address.sin_addr.s_addr, server->h_addr, server->h_length);
    #else
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(host);
    #endif
    
    if (connect(s, (struct sockaddr*) &address, sizeof(address)) == -1)
        return -1;
    
    return s;
}


static void get_abstime(int microseconds, struct timespec *abstime)
{
    struct timeval tv;
    long long absmsec;
    gettimeofday(&tv, NULL);
    absmsec = tv.tv_sec * 1000ll + tv.tv_usec / 1000ll;
    absmsec += microseconds;

    abstime->tv_sec = absmsec / 1000ll;
    abstime->tv_nsec = absmsec % 1000ll * 1000000ll;
}

static int ftp_sendcmd(char *cmd, int bWait, ftp_ctrl_resp_type *ctrl_resp)
{
    int         len;
    char        buf[512];
    ssize_t     r_len;
    struct timeval tv;      
    fd_set wfds; 
    int ret = -1;  
    int lenSend = 0;    
    int ctrl_sock = ftp_info.ctrl_sock;
    int sent_size = 0;
    int total_size = strlen(cmd);
    char *pcmd = cmd;
    int selres;
    struct timespec abstime;

    while(1)  
    {  
        FD_ZERO(&wfds);  
        FD_SET(ctrl_sock, &wfds);  
        tv.tv_sec = 5;  
        tv.tv_usec = 0;  
        selres = select(ctrl_sock + 1, NULL, &wfds, NULL, &tv);
        if(selres == -1)
        {
            if(errno == EINTR)
            {
                continue;
            }
            else if(errno == EAGAIN)
            {
                usleep(1000);
                continue;
            }
        }
        else if(selres == 0)
        {
            //timeout 
            printf("timeout\n");
            ret = -1;
            break;
        }
        
        lenSend = send(ctrl_sock,pcmd + sent_size, total_size - sent_size,0);  
        if(lenSend == -1)  
        {  
            printf("lenSend -1\n");
            ret = -1;
            break;  
        }
        else if(lenSend == 0)
        {
            printf("lenSend 0\n");
            close(ctrl_sock);
            ftp_info.ctrl_sock = -1;
            ret = -1;
            break;
        }
        sent_size += lenSend;  
        if(sent_size == total_size)  
        {  
            ret = 0;
            break;  
        }
    }  

    if(ret == -1)
    {
        printf("cmd send fail\n");
        return -1;
    }
    
    if(bWait == 0)
    {
        return 0;
    }

    if(ctrl_resp == NULL)
    {
        return -1;
    }
    
    get_abstime(10000, &abstime);  
    pthread_mutex_lock(&s_commandmutex);
    ret = pthread_cond_timedwait(&s_commandcond, &s_commandmutex, &abstime);

    if (ret == ETIMEDOUT) 
    {
        pthread_mutex_unlock(&s_commandmutex);
        printf("wait for response timeout\n");
        return -1;    
    }   
    
    memcpy(ctrl_resp, &ftp_info.ctrl_resp, sizeof(ftp_ctrl_resp_type));
    pthread_mutex_unlock(&s_commandmutex);

    return 0; 
}


static int ftp_pasv_connect()
{
    int     data_sock;
    int     ret;
    ssize_t len;
    int     addr[6];
    char    buf[512];

    ftp_ctrl_resp_type ctrl_resp;
    
    ret = ftp_sendcmd("PASV\r\n", TRUE, &ctrl_resp);
    if (ret != 0) 
    {
        return -1;
    }

    if(ctrl_resp.resp_code[0] != 227)
    {
        return -1;
    }
    
    sscanf(ctrl_resp.resp_str, "%*[^(](%d,%d,%d,%d,%d,%d)",&addr[0],&addr[1],&addr[2],&addr[3],&addr[4],&addr[5]);

    //printf("%d.%d.%d.%d:%d\n", addr[0],addr[1],addr[2],addr[3],addr[4]*256+addr[5]);
    
    bzero(buf, sizeof(buf));
    sprintf(buf, "%d.%d.%d.%d",addr[0],addr[1],addr[2],addr[3]);
    data_sock = ftp_data_connect(buf,addr[4]*256+addr[5]);
    
    return data_sock;
}

//表示类型
int ftp_type(char mode)
{
    int     ret;
    char    buf[128];
    ftp_ctrl_resp_type ctrl_resp;
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));
    sprintf( buf, "TYPE %c\r\n", mode );
    ret = ftp_sendcmd(buf, TRUE, &ctrl_resp);
    if(ret != 0)
    {
        return -1;
    }
    if(ctrl_resp.resp_code[0] != 200)
    {
        return -1;
    }
    
    return 0;
}

int ftp_param_init(char *host, int port, char *user, char *pwd)
{
    //pthread_cond_init(&s_commandmutex,NULL);
    //pthread_mutex_init(&s_commandcond,NULL); 


    if((host == NULL) || (strlen(host) >= sizeof(ftp_info.host)))
    {
        return -1;
    }

    if((user == NULL) || (strlen(user) >= sizeof(ftp_info.user)))
    {
        return -1;
    }
    
    if((pwd == NULL) || (strlen(pwd) >= sizeof(ftp_info.pwd)))
    {
        return -1;
    } 
    pthread_mutex_lock(&s_commandmutex);   
    ftp_info.port = port;
    strcpy(ftp_info.host, host);
    strcpy(ftp_info.user, user);
    strcpy(ftp_info.pwd, pwd);
    ftp_info.init = 1;    
    printf("host: %s, port: %d, usr: %s pwd: %s\n", host, port, ftp_info.user, ftp_info.pwd);
    pthread_mutex_unlock(&s_commandmutex);
    
    return 0;
}

int ftp_login()
{
    char    buf[128];
    int     ret; 
    int     ctrl_sock = ftp_info.ctrl_sock;
    ftp_ctrl_resp_type  ctrl_resp;
    if(ftp_info.init == 0)
    {
        return -2;
    }
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));
    sprintf( buf, "USER %s\r\n", ftp_info.user);
    ret = ftp_sendcmd(buf, TRUE, &ctrl_resp);
    if(ret != 0)
    {
        return -1;
    }

    if(ctrl_resp.resp_code[0] == 230)
    {
       // printf("already login\n");
        return 0;
    }

    if(ctrl_resp.resp_code[0] != 331)
    {
        return -1;
    }
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "PASS %s\r\n", ftp_info.pwd);
    ret = ftp_sendcmd(buf, TRUE, &ctrl_resp);        

    if((ret == 0) && (ctrl_resp.resp_code[0] == 230))
    {
        return 0;
    }
    else
    {
        return -1;
    }
    
}


int ftp_logout()
{
    int     ctrl_sock = ftp_info.ctrl_sock;
    if(ctrl_sock >= 0)
    {
        ftp_sendcmd("QUIT\r\n", FALSE, NULL);
        close(ctrl_sock);
        ftp_info.ctrl_sock = -1;
    }
    return 0;
}

int ftp_pwd(char *path)
{
    char    buf[1024] = {0};
    int     ret; 
    int     ctrl_sock = ftp_info.ctrl_sock;
    ftp_ctrl_resp_type  ctrl_resp;
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));

    ret = ftp_sendcmd("PWD\r\n", TRUE, &ctrl_resp);
    if(ret != 0 || ctrl_resp.resp_code[0] != 257)
    {
        return -1;
    }
    memcpy(path, ctrl_resp.resp_str, strlen(ctrl_resp.resp_str));
    return 0;
}


int ftp_size(char *path, long long *size)
{
    char    cmd[256] = {0};
    int     ret; 
    int     ctrl_sock = ftp_info.ctrl_sock;
    ftp_ctrl_resp_type  ctrl_resp;
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));

    sprintf(cmd, "SIZE %s\r\n", path);
    
    ret = ftp_sendcmd(cmd, TRUE, &ctrl_resp);
    if(ret != 0 || ctrl_resp.resp_code[0] != 213)
    {
        return -1;
    }

    sscanf(ctrl_resp.resp_str, "%lld", size);
    return 0;    
}

int ftp_cwd(char *path)
{
    int     ret;
    char    cmd[128] = {0};
    ftp_ctrl_resp_type  ctrl_resp;
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));
    sprintf(cmd, "CWD %s\r\n", path);
    ret = ftp_sendcmd(cmd, TRUE, &ctrl_resp);
    if ( ret != 0 || ctrl_resp.resp_code[0] != 250)
    {
        return -1;
    }
    return 0;
}


int ftp_mkdir(char *path )
{
    int     ret;
    char    cmd[128] = {0};
    ftp_ctrl_resp_type  ctrl_resp;
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));
    sprintf(cmd, "MKD %s\r\n", path);
    ret = ftp_sendcmd(cmd, TRUE, &ctrl_resp);
    if ( ret != 0 || ctrl_resp.resp_code[0] != 257)
    {
        return -1;
    }
    return 0;

}


int ftp_mv(char *s_path, char *d_path)
{
    int     ret;
    char    cmd[128]= {0};
    ftp_ctrl_resp_type  ctrl_resp;
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));
    sprintf(cmd, "RNFR %s\r\n", s_path);
    ret = ftp_sendcmd(cmd, TRUE, &ctrl_resp);
    if ( ret != 0 || ctrl_resp.resp_code[0] != 350)
    {
        return -1;
    }

    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "RNTO %s\r\n", d_path);
    ret = ftp_sendcmd(cmd, TRUE, &ctrl_resp);
    if ( ret != 0 || ctrl_resp.resp_code[0] != 250)
    {
        return -1;
    }

    return 0;
}
 

int ftp_deltefile(char *path)
{
    int     ret;
    char    cmd[128] = {0};
    ftp_ctrl_resp_type  ctrl_resp;
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));
    sprintf(cmd, "DELE %s\r\n", path);
    ret = ftp_sendcmd(cmd, TRUE, &ctrl_resp);
    if ( ret != 0 || ctrl_resp.resp_code[0] != 250)
    {
        return -1;
    }
    return 0;    
}
 
int ftp_deletefolder(char *path)
{
    int     ret;
    char    cmd[128] = {0};
    ftp_ctrl_resp_type  ctrl_resp;
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));
    sprintf(cmd, "RMD %s\r\n", path);
    ret = ftp_sendcmd(cmd, TRUE, &ctrl_resp);
    if ( ret != 0 || ctrl_resp.resp_code[0] != 250)
    {
        return -1;
    }
    return 0; 

}


int ftp_list(char *path, void *data, int max_data_len, int *data_len)
{
    char    buf[512] = {0};
    char    cmd[256] = {0};
    int     ret; 
    int     ctrl_sock = ftp_info.ctrl_sock;
    int     data_sock;
    int     data_overflow = 0;
    ssize_t     len,buf_len,total_len;
    ftp_ctrl_resp_type  ctrl_resp;
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));

    if(ftp_info.init == 0)
    {
        return -2;
    }

    data_sock = ftp_pasv_connect();
    if(data_sock <= 0)
    {
        return -1;
    }

    set_trans_status(0);
    bzero(cmd, sizeof(cmd));
    sprintf( cmd, "LIST %s\r\n", path);
    ret = ftp_sendcmd(cmd, TRUE, &ctrl_resp);
    if(ret != 0)
    {
        close(data_sock);
        return -1;
    }
    if(ctrl_resp.resp_code[0] != 150)
    {
        close(data_sock);
        return -1;
    }

    len=total_len = 0;
    buf_len = 512;

    while ( (len = recv(data_sock, buf, 512, 0 )) > 0 )
    {
        if (total_len+len > max_data_len)
        {
            if(total_len < max_data_len)
            {
                memcpy((void *)((char *)(data) + total_len), buf, max_data_len - total_len);
            }
            data_overflow = 1;
            total_len = max_data_len;
            break;
        }
        else 
        {
            memcpy((void *)((char *)(data) + total_len), buf, len);
        }

        memset(buf, 0, sizeof(buf));
        total_len += len;
    }
    *data_len = total_len;
    
    close( data_sock );
    get_trans_status();
    if(data_overflow)
    {
        return -3;
    }
    
    return 0;
}


int ftp_download(char *path, ftp_dl_recv_cb_fcn handle)
{
    char    buf[512] = {0};
    char    cmd[256] = {0};
    int     ret; 
    int     ctrl_sock = ftp_info.ctrl_sock;
    int     data_sock;
    int     data_overflow = 0;
    long long file_size = 0;
    long long total_len = 0;
    ssize_t   len;
    ftp_ctrl_resp_type  ctrl_resp;
    int     dl_pack_index = 1;
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));

    if(ftp_info.init == 0)
    {
        return -2;
    }

    ret = ftp_size(path, &file_size);
    if(ret != 0)
    {
        printf("get size fail\n");
        return -1;
    }

    
    ret = ftp_type('I');
    if(ret != 0)
    {
        return -1;
    }

    data_sock = ftp_pasv_connect();
    if(data_sock <= 0)
    {
        return -1;
    }

    set_trans_status(0);
    bzero(cmd, sizeof(cmd));
    sprintf(cmd, "RETR %s\r\n", path);
    ret = ftp_sendcmd(cmd, TRUE, &ctrl_resp);
    if(ret != 0)
    {
        close(data_sock);
        return -1;
    }
    if(ctrl_resp.resp_code[0] != 150)
    {
        close(data_sock);
        return -1;
    }
    
    bzero(buf, sizeof(buf));
    while ( (len = recv( data_sock, buf, 512, 0 )) > 0 ) 
    {
        handle(buf, len, dl_pack_index, file_size);
        dl_pack_index++;
        total_len += len;
    }

    dl_pack_index = 0;
    handle(buf, 0, 0, file_size);
      
    close( data_sock );

    get_trans_status();

    if(total_len == file_size)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int get_file_size(char* filename)
{
    FILE *fp=fopen(filename,"r");
    if(!fp) return -1;
    fseek(fp,0L,SEEK_END);
    int size=ftell(fp);
    fclose(fp);
    printf("File size = %d\n", size);
    return size;
}


int ftp_upload(char *local_file, char *ftp_path)
{
    char    buf[512] = {0};
    char    cmd[256] = {0};
    int     ret; 
    int     ctrl_sock = ftp_info.ctrl_sock;
    int     data_sock;
    int     data_overflow = 0;
    long long file_size = 0;
    long long total_len = 0;
    ssize_t   len, send_len;
    ftp_ctrl_resp_type  ctrl_resp;
    int     dl_pack_index = 1;
    int     fd;
    memset(&ctrl_resp, 0, sizeof(ftp_ctrl_resp_type));

    if(ftp_info.init == 0)
    {
        return -2;
    }

    file_size = get_file_size(local_file);
    if(file_size == -1)
    {
        return -1;
    }

    ret = ftp_type('I');
    if(ret != 0)
    {
        return -1;
    }

    data_sock = ftp_pasv_connect();
    if(data_sock <= 0)
    {
        return -1;
    }

    set_trans_status(0);
    bzero(cmd, sizeof(cmd));
    sprintf(cmd, "STOR %s\r\n", ftp_path);
    ret = ftp_sendcmd(cmd, TRUE, &ctrl_resp);
    if(ret != 0)
    {
        close(data_sock);
        return -1;
    }
    if(ctrl_resp.resp_code[0] != 150)
    {
        close(data_sock);
        return -1;
    }

    fd = open(local_file,  O_RDONLY); 
    if ( fd == -1 ) 
    {
        close(data_sock);
        return -1;
    }
    
    bzero(buf, sizeof(buf));
    printf("\n");
    while ( (len = read( fd, buf, 512)) > 0)
    {
        send_len = send(data_sock, buf, len, 0);
        if (send_len != len)
        {
            close(data_sock);
            close(fd);
            return -1;
        }
        
        total_len += len;
        printf("\r\033[k"); 
        printf("send = %lld/%lld  %%%d", total_len, file_size, total_len * 100 /file_size);      
        if(total_len == file_size)
        {
            break;
        }
    }
    
    close( data_sock );
    printf("\n");
    get_trans_status();

    if(total_len == file_size)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

