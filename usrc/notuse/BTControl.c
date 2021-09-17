#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
//#include <msg.h>
#include "common.h"
#include "BTControl.h"
#include <errno.h>

/*--------------------------------------------------------------------*/
uint8_t CharaProCollection[8] ={ATT_PERM_CONFIGURE_BROADCAST,
                                                 ATT_PERM_READ,
                                                 ATT_PERM_WRITE_CMD,
                                                 ATT_PERM_WRITE_REQ,
                                                 ATT_PERM_NOTIFY,
                                                 ATT_PERM_INDICATE,
                                                 ATT_PERM_AUTHENTICATED,
                                                 ATT_PERM_EXTENDED};


static int bt_send_request(bt_msg *send_msg, bt_msg *recv_msg)
{
    int server_fifo_fd;
    ssize_t write_len;
    
    server_fifo_fd = open(BT_SERVER_FIFO, O_WRONLY | O_NONBLOCK);

    if(server_fifo_fd == -1){
        //printf("open server fifo fail!\n");
        return -1;
    }

    write_len = write(server_fifo_fd, send_msg, sizeof(bt_msg));

    //printf("write_len = %d\r\n", write_len);
    if(write_len <= 0){
        printf("send msg write fail\n");
        close(server_fifo_fd);
        return -1;
    }
    
    close(server_fifo_fd);
    return 0;
}

static int bt_send_sync_request(bt_msg *send_msg, bt_msg *recv_msg)
{
    int result;
    fd_set fds;
    int recv_fd;
    int status;
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0; 
    int count = 0;
    
    if(access(BT_DAEMON_SYNC_FIFO, F_OK) == -1)  
    {  
        if (mkfifo(BT_DAEMON_SYNC_FIFO, 0666) < 0){
            printf("mkfifo client fifo fail\n");
            return -1;
        }
    }
   
    result = bt_send_request(send_msg, NULL);
    if(result < 0)
    {
        //printf("bt send request fail!\r\n");
        return -1;
    }
    


    recv_fd = open(BT_DAEMON_SYNC_FIFO, O_RDWR | O_NONBLOCK);

    if(recv_fd == -1){
        printf("open BT_DAEMON_SYNC_FIFO fail\n");
        return -1;
    }

    while(count++ < 5)
    {
        FD_ZERO(&fds);
        FD_SET(recv_fd, &fds);  
          
        status = select(recv_fd + 1, &fds, NULL, NULL, &tv);
        if(status == -1)
        {
            printf("select error (%d)-%s", errno, strerror(errno));
            if (errno == EINTR)
            {
                continue;      /* Interrupt system call, need continue */
            }
            else
            {
                result = -1;
            }        
        }
        else if(status == 0)
        {
            printf("\t\n sync request timeout\r\n");
            result = -1;
        }
        else
        {
            if (FD_ISSET(recv_fd, &fds))
            {
                int read_count;
                memset(recv_msg, 0, sizeof(bt_msg));
                read_count = read(recv_fd, recv_msg, sizeof(bt_msg));
                if(read_count < 0)
                {
                    printf("read_count < 0\r\n");       
                    result = -1;
                }
                else if(read_count == 0)
                {
                    printf("maybe the client has been closed, ignore");
                    result = -1;
                }
            }
        }
        break;
    }
    close(recv_fd);
    unlink(BT_DAEMON_SYNC_FIFO);  
    return result;
}

static int ReadBtAddrFile(const char *file_name, SIMCOM_BT_ADDR_T *addr)
{
    FILE *fp = NULL;
    uint32_t nap, uap, lap;

    if (file_name == NULL || addr == NULL)
    {
        return -1;
    }

    fp = fopen(file_name, "r");

    if (!fp)
    {
        return -1;
    }
    if (fscanf(fp, "BT_ADDR = %04x:%02x:%06x ", &nap, &uap, &lap) != 3)
    {
        fclose(fp);
        return -1;
    }

    addr->nap = (uint16_t)(nap & 0xFFFF);
    addr->uap = (uint8_t) (uap & 0xFF);
    addr->lap = (uint32_t)(lap & 0x00FFFFFF);

    fclose(fp);

    return 0;
}

static int ReadBtNameFile(const char *file_name, char *name)
{
    FILE *fp = NULL;
    unsigned int i;
    if (file_name == NULL || name == NULL)
    {
        return -1;
    }

    fp = fopen(file_name, "r");

    if (!fp)
    {
        return -1;
    }

    fgets(name,256,fp);

    for(i = 0; i < strlen(name) + 1; i++)
    {
        if(name[i] == 0x0A)
        {
            name[i] = 0;
        }
    }
    
    fclose(fp);
    return 0;
}

static int ReadBtNameUTF8(const char *file_name, unsigned char *utf8name)
{
    FILE *fp = NULL;
    bt_info r_info;
    size_t size;

    memset(&r_info, 0, sizeof(bt_info));

    if (file_name == NULL || utf8name == NULL)
    {
        return -1;
    }

    fp = fopen(file_name, "rb");

    if (!fp)
    {
        return -1;
    }

    size = fread(&r_info,1,sizeof(bt_info),fp);
    if(size != sizeof(bt_info))
    {
        //error
    }

    memcpy(utf8name, r_info.bt_name, sizeof(r_info.bt_name));

    fclose(fp);
    return 0;
}
static int ReadBtPassword(const char *file_name, unsigned char *btpwd)
{
    FILE *fp = NULL;
    bt_info r_info;
    size_t size;

    memset(&r_info, 0, sizeof(bt_info));

    if (file_name == NULL || btpwd == NULL)
    {
        return -1;
    }

    fp = fopen(file_name, "rb");

    if (!fp)
    {
        return -1;
    }

    size = fread(&r_info,1,sizeof(bt_info),fp);
    if(size != sizeof(bt_info))
    {
        //error
    }

    memcpy(btpwd, r_info.bt_pw, sizeof(r_info.bt_pw));

    fclose(fp);
    return 0;
}
static int WriteBtPassWord(const char *file_name, unsigned char *password)
{
    FILE *fp = NULL;
    bt_info r_info;
    bt_info w_info;
    int res;
    char name[64]={'\0'};

    memset(&r_info, 0, sizeof(bt_info));
    memset(&w_info, 0, sizeof(bt_info));
    
    if (file_name == NULL || password == NULL)
    {
        return -1;
    }
    res=ReadBtNameUTF8(file_name, (unsigned char *)name);
    if(res != 0)
    {
        return -1;
    }
    
    fp = fopen(file_name, "w");

    if (!fp)
    {
        return -1;
    }


    memcpy(w_info.bt_name, name, sizeof(name));

    memcpy(w_info.bt_pw, (const char*)password, strlen((const char*)password) <= 16 ? strlen((const char*)password):16);


	 
    fseek(fp,0,SEEK_SET);

    fwrite(&w_info, 1, sizeof(bt_info), fp);
    fclose(fp);
    return 0;
}

static int WriteBtNameFile(const char *file_name, unsigned char *name)
{
    FILE *fp = NULL;
    bt_info r_info;
    bt_info w_info;
    int res;
    char pwd[32]={'\0'};
    memset(&r_info, 0, sizeof(bt_info));
    memset(&w_info, 0, sizeof(bt_info));

    if (file_name == NULL || name == NULL)
    {
            return -1;
    }
    res = ReadBtPassword(file_name, (unsigned char *)pwd);
    if(res != 0)
    {
       return -1;
    }

    fp = fopen(file_name, "wb+");

    if (!fp)
    {
        return -1;
    }

    memcpy(w_info.bt_name, (const char*)name, strlen((const char*)name) <= 63 ? strlen((const char*)name):63);

    memcpy(w_info.bt_pw, pwd, sizeof(pwd));

    fseek(fp,0,SEEK_SET);

    fwrite(&w_info, 1, sizeof(bt_info), fp);

    //fprintf(fp, "%s", name);

    fclose(fp);
    return 0;
}

static int get_bt_name_4_power_on( char *btname)
{
    SIMCOM_BT_ADDR_T addr;
    int bGetName = 0;

    bGetName = ReadBtNameFile("/data/bluetooth/btname.inf",btname);

    if(bGetName != -1 && strlen(btname) > 0)
    {
        return 0;
    }
    
    ReadBtAddrFile("/data/bluetooth/bt.inf",&addr);
    sprintf(btname, "SIM7600_BT_%02X%02X%02X", (addr.lap >> 16) & 0xFF, (addr.lap >> 8) & 0xFF,addr.lap & 0xFF);
    return 0;
}


/*  设置inquiry   参数 */
static void bt_set_inquiry_scan(int interval ,int window)
{
    bt_msg send_msg;
    int result;
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_SET_INQUIRY_SCAN_COMMAND;
    if(interval<18||interval>4096||interval%2!=0||window<18||window>4096)
    {
        return;
    }
    send_msg.param1=interval;
    send_msg.param2=window;
    result = bt_send_request(&send_msg, NULL);
    if(result < 0)
    {
        printf("get profile fail!\r\n");
    }
}

void* daemon_thread_fun(void *param)
{
    fd_set fds;
    int status;
    bt_msg recv_msg;
    int s_daemon_fifo_fd = -1;
    bt_ind_cb_fcn process_handle = (bt_ind_cb_fcn)param;
    pthread_detach(pthread_self());
    if(access(BT_DAEMON_FIFO, F_OK) == -1)  
    {  
        if (mkfifo(BT_DAEMON_FIFO, 0666) < 0){
            printf("mkfifo client fifo fail\n");
            return NULL;
        }
    }

    s_daemon_fifo_fd = open(BT_DAEMON_FIFO, O_RDWR | O_NONBLOCK);

    if(s_daemon_fifo_fd == -1){
        printf("open BT_DAEMON_FIFO fail\n");
        return NULL;
    }

    FD_ZERO(&fds);
    FD_SET(s_daemon_fifo_fd, &fds);  
    
    while(TRUE)
    {
        FD_ZERO(&fds);
        FD_SET(s_daemon_fifo_fd, &fds);      
        status = select(s_daemon_fifo_fd + 1, &fds, NULL, NULL, NULL);
        if(status == -1)
        {
            printf("select error\r\n");
        }
        else
        {
            if (FD_ISSET(s_daemon_fifo_fd, &fds))
            {
                int read_count;
                memset(&recv_msg, 0, sizeof(bt_msg));
                read_count = read(s_daemon_fifo_fd, &recv_msg, sizeof(bt_msg));
                if(read_count < 0)
                {
                    printf("read_count < 0\r\n");              
                }
                else if(read_count == 0)
                {
                    printf("maybe the client has been closed, ignore");
                }
                else
                {
                	if(process_handle != NULL)
                	{
                    	process_handle(&recv_msg);
                    }
                }

            }

        }
    }
    close(s_daemon_fifo_fd);
    s_daemon_fifo_fd = -1;
    unlink(BT_DAEMON_FIFO);   
    return NULL;
}

static int create_dameon_recv_thread(void *data)
{
    pthread_t daemon_thread;

    if(0 != pthread_create(&daemon_thread, NULL, daemon_thread_fun, (void *)data))
    {
        printf("create daemon_thread fail");
        return -1;
    }  
    return 0;
}




int search_devices(int subcmd, int mode, int timeout)
{
    bt_msg send_msg;
    int result;
    memset(&send_msg, 0, sizeof(bt_msg));

    send_msg.command = BT_SEARCH_COMMAND;
    send_msg.param1 = subcmd;    // 1: start search   0: stop search
    send_msg.param2 = mode;      // 0: normal search  1: hide paired
    send_msg.param3 = timeout;   // search timeout 0~60
    
    result = bt_send_request(&send_msg, NULL);
    if(result < 0)
    {
        return -1;
    }
    return 0;
}

int set_iocap_device(int mode)
{
    bt_msg send_msg, recv_msg;
    int result;
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));
    
    send_msg.command = BT_IOCAP_COMMAND;
    send_msg.param1 = mode;

    result = bt_send_sync_request(&send_msg, &recv_msg);
    if(result < 0)
    {
       return -1;
    }
	return 0;
}
int bond_device(int index)
{
    bt_msg send_msg, recv_msg;
    int result;
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));;

    send_msg.command = BT_BOND_COMMAND;
    send_msg.param1 = index;

    result = bt_send_request(&send_msg, NULL);
    if(result < 0)
    {
        return -1;
    }
    return 0;
}

int debond_device(int index)
{
    bt_msg send_msg, recv_msg;
    int result;
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));;

    send_msg.command = BT_DEBOND_COMMAND;
    send_msg.param1 = index;

    result = bt_send_sync_request(&send_msg, &recv_msg);
    if(result < 0)
    {
        return -1;
    }

    if(recv_msg.param1 != 1)
    {
        return -2;
    }
    return 0;
}

int accept_bond(int isAccept, accept_mode mode)
{
    bt_msg send_msg;
    int result;
    int bAccept;
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_ACCEPT_COMMAND;


    if(isAccept)
    {
        if(mode == ACCEPT_MODE_NONE)
        {
            return -1;
        }
        if(mode == ACCEPT_MODE_NOTIFICATION || mode == ACCEPT_MODE_JUSTWORK)
        {
            return -1;
        }
        bAccept = 1;
    }
    send_msg.param1 = mode;
    send_msg.param2 = isAccept;	
    result = bt_send_request(&send_msg, NULL);
    if(result < 0)
    {
        return -1;
    }
    return 0;
}

int pin_read(char *pincode)
{
    char pwd[32]={0};
    int result=0;
    result=ReadBtPassword("/data/bluetooth/btname.inf", (unsigned char *)pwd);
	if(result < 0)
	{
		return -1;
	}
    else
    {
  		strcpy(pincode, pwd);
    }
   	return 0;
}

int pin_set(char * pwd)
{
    int len = strlen(pwd);
    if(len<4||len>16)
    {
        return -1;
    }

    if (-1 == WriteBtPassWord("/data/bluetooth/btname.inf", (unsigned char *)pwd))
    {
        return -1;
    }
    return 0;
}
int host_device_addr(char *device_name, SIMCOM_BT_ADDR_T *paddr)
{
    SIMCOM_BT_ADDR_T addr;
    char name[128] ={0};
    int bGetName = 0;

    if(ReadBtAddrFile("/data/bluetooth/bt.inf",&addr) != 0)
    {
        return -1;
    }

    bGetName = ReadBtNameUTF8("/data/bluetooth/btname.inf", (unsigned char *)name);

    if(bGetName < 0 || strlen(name) == 0)
    {
        sprintf(device_name, "SIM7600_BT_%2X%2X%2X", (addr.lap >> 16) & 0xFF, (addr.lap >> 8) & 0xFF,addr.lap & 0xFF);
    }
	else
	{
		sprintf(device_name,"%s",name);
	}
	
	memcpy(paddr, &addr, sizeof(SIMCOM_BT_ADDR_T));
	return 0;
		/*
    printf("+BTHOST: %s, %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                        device_name, 
                        (addr.nap >> 8) & 0xFF,
                        addr.nap & 0xFF,
                        addr.uap,
                        (addr.lap >> 16) & 0xFF,
                        (addr.lap >> 8) & 0xFF,
                        addr.lap & 0xFF);
    */ 
}         
int host_device(char *device_name)
{
    if(device_name == NULL || strlen(device_name) == 0)
    {
    	return -1;
    }
       
    #if 0
    FILE *fp;
    int num = 0;
    const char process[] = "pidof csr_bt_app_default_hci|wc -L";        
    fp = popen(process, "r");
    if(fp)
    {
        num = fgetc(fp);
        pclose(fp);
    }

    if(num != '0')
    {
        bt_msg send_msg;  
        memset(&send_msg, 0, sizeof(bt_msg));
        send_msg.command = BT_HOST_COMMAND;
        send_msg.data_len = strlen(device_name);
        sprintf((char *)send_msg.data, "%s", device_name);
        result = bt_send_request(&send_msg, NULL);
    }
    #endif
    
    return WriteBtNameFile("/data/bluetooth/btname.inf", (unsigned char *)device_name);
}

int spp_send_data(uint8_t *data, uint16_t data_len)
{
	bt_msg send_msg;
	bt_msg recv_msg;
	int result;
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_SPP_SEND_COMMAND;	

	memcpy(send_msg.data, data, data_len);
	send_msg.data_len = data_len;
    result = bt_send_sync_request(&send_msg, &recv_msg);
    if(result < 0)
    {
    	return -1;
    }

    if(recv_msg.result != BT_RESULT_SUCCESS)
    {
    	return -1;
    }
    return 0;
}

int spp_server(int bActive)
{
	bt_msg send_msg;
	bt_msg recv_msg;
	int result;
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));

    send_msg.command = BT_SPP_SERVER_COMMAND;
    if(bActive){
        send_msg.param1 = 1;    //active
    }else{
        send_msg.param1 = 0;     //deactive
    }
    
    result = bt_send_sync_request(&send_msg, &recv_msg);
	if(result < 0){
		return -1;
	}

	if(bActive)
	{
		if(recv_msg.param1 == 1)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else   
	{
		if(recv_msg.param1 == 0)
		{
			return 0;
		}
		else
		{
			return -1;
		}		
	}
	
	return 0;
}

int spp_disconncet_device(void)
{
	bt_msg send_msg;
	bt_msg recv_msg;
	int result;
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));

	
    send_msg.command = BT_DISCONNECT_COMMAND;
    send_msg.param3 = 1;
    result = bt_send_sync_request(&send_msg, &recv_msg);
    if(result < 0)
    {
        return -1;
    }
    
	return 0;
}


int get_bonded_device(void)
{
    bt_msg send_msg;
    int result;
    
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GET_BONDED_COMMAND;

    result = bt_send_request(&send_msg, NULL);
    if(result < 0)
    {
        return -1;
    }
    return 0;
}

int bt_get_profile(int index, int *isSupportSPP)
{
	bt_msg send_msg;
	bt_msg recv_msg;
	int result;
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));

    send_msg.command = BT_GET_PROFILE_COMMAND;
    send_msg.param1 = index;

    result = bt_send_sync_request(&send_msg, &recv_msg);
    if(result < 0)
    {
        return -1;
    }

    if(recv_msg.param1 == 0){
    	*isSupportSPP = 0;
    }else{
    	*isSupportSPP = 1;
    }
    return 0;
}
int bt_get_server_status(int *isActived)
{
    bt_msg send_msg;
    bt_msg recv_msg;
    int result;
    
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_SERVER_STATUS_COMMAND;

    result = bt_send_sync_request(&send_msg, &recv_msg);
    if(result < 0)
    {
        return -1;
    }

	if(recv_msg.param1){
		*isActived = 1;
	}else{
		*isActived = 0;
	}
    
	return 0;

}
int bt_get_connect_status(int *isConnected)
{
    bt_msg send_msg;
    bt_msg recv_msg;
    int result;
    
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_CONNECT_STATUS_COMMAND;

    result = bt_send_sync_request(&send_msg, &recv_msg);
    if(result < 0)
    {
        return -1;
    }

	if(recv_msg.param1){
		*isConnected = 1;
	}else{
		*isConnected = 0;
	}
    return 0;
}

int spp_conncet_device(int index)
{
    bt_msg send_msg;
    bt_msg recv_msg;
    int result;
    
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));

    send_msg.command = BT_CONNECT_COMMAND;
    send_msg.param1 = index;
    send_msg.param2 = 4;   // SPP profile_id always 4
	send_msg.param3 = 1;

    result = bt_send_sync_request(&send_msg, &recv_msg);
    if(result < 0)
    {
        return -1;
    }

	if(recv_msg.param1 != 1)
	{
		return -2;
	}
	
    return 0;
}


int bt_gatt_register(int bRegister)
{
    bt_msg send_msg,recv_msg;
    int result;
    
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_REGISTER_COMMAND;
    if(bRegister < 0 || bRegister > 1)
    {
        return -1;
    }

    if(bRegister)
    {
        send_msg.param1 = 1;
    }

    result = bt_send_sync_request(&send_msg, &recv_msg);
    if(result < 0)
    {
    	return -1;
    }

    if(recv_msg.param1==-1)
    {
        return -2;
    }
    
    if(recv_msg.param1 == 1)  //register
    {
		if(recv_msg.result == 1)
		{
			return 0;
		}
		else
		{
			return -3;
		}
    }
    else
    {
        if(recv_msg.result == 1)   //deregister
        {
            return 0;
        }
        else
        {
            return -3;
        }                
    }
    return 0;
}

int bt_gatt_active(int bActive)
{
    bt_msg send_msg,recv_msg;
    int result;
    memset(&recv_msg, 0, sizeof(bt_msg));
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_ACTIVE_COMMAND;
    if(bActive)
    {
        send_msg.param1 = 1;
    }

    result = bt_send_sync_request(&send_msg,&recv_msg);
	if(result < 0)
	{
		return -1;
	}

	if(recv_msg.param1 == -1)
	{
		return -2;
	}

    if(recv_msg.result == 0)
    {
        return -3;
    }

    return 0;
}

#if 0
void bt_gatt_advertise(int bAdvertise)
{
    bt_msg send_msg;
    int result;
    
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_ADVERTISE_COMMAND;
    if(bAdvertise)
    {
        send_msg.param1 = 1;
    }

    result = bt_send_request(&send_msg, NULL);
    if(result < 0)
    {
        printf("gatt active fail!\r\n");
    }
}
#endif

int bt_gatt_createdatabase(int bCreateDb)
{
    bt_msg send_msg;
    int result;
    bt_msg recv_msg;
    memset(&recv_msg, 0, sizeof(bt_msg));
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_CREATEDB_COMMAND;
    if(bCreateDb)
    {
        send_msg.param1 = 1;
    }
    else
    {
        return -1;
    }
    result = bt_send_sync_request(&send_msg,&recv_msg);
    if(result < 0)
    {
        return -1;
    }

    if(recv_msg.param1 == -1)
    {
        return -2;
    }

    if(recv_msg.result == 0)
    {
        return -3;
    }

    return 0;
}

int bt_gatt_create_service_uuid16(int uuid16)
{
    bt_msg send_msg;
    bt_msg recv_msg;
    int result;
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_CREATE_SERV_COMMAND;
    //uuid16 = 0x1814;   /* Running Speed and Cadence */
    if(uuid16 != 0)
    {
        send_msg.uuid16 = uuid16;
        send_msg.check=16;
        }
    else
    {
        return -1;
    }

    result = bt_send_sync_request(&send_msg,&recv_msg);
    if(result < 0)
    {
        return -1;
    }

    if(recv_msg.param1==-1)
    {
        return -2;
    }

    return 0;
}

int bt_gatt_create_service_uuid128(unsigned char *uuid128)
{
    bt_msg send_msg;
    bt_msg recv_msg;
    int result,i;
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_CREATE_SERV_COMMAND;
    if(uuid128 == NULL)
    {
        return -1;
    }

    memcpy(send_msg.uuid128,uuid128,16);
    send_msg.check=128;

    result = bt_send_sync_request(&send_msg,&recv_msg);
    if(result < 0)
    {
        return -1;
    }

    if(recv_msg.param1==-1)
    {
        return -2;
    }

    return 0;
}


int bt_gatt_create_characteristic_uuid16(int uuid16, int pro, int permission, uint32_t *attrHandle)
{
    bt_msg send_msg;
    bt_msg recv_msg;
    int result;
    uint8_t  properties;
    uint16_t attrValueFlags;
    uint16_t attrValueLength;
    
    memset(&recv_msg, 0, sizeof(bt_msg));
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_CREATE_CHAR_COMMAND;

    properties=pro;
    attrValueFlags=permission;
    attrValueLength = sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint8_t);
    if(uuid16 != 0)
    {
        send_msg.uuid16 = uuid16;
        send_msg.check=16;
    }
    else
    {
        return -1;
    }
   
    send_msg.param1 = properties;
    send_msg.param2 = attrValueFlags;
    send_msg.param3 = attrValueLength;
    
    result = bt_send_sync_request(&send_msg,&recv_msg);
    if(result < 0)
    {
        return -1;
    }

    if(recv_msg.param1==-1)
    {
        return -2;
    }
    
    if(recv_msg.result != 1)
    {
        return -3;
    }

    *attrHandle = recv_msg.attrHandle;
    return 0;

}

int bt_gatt_create_characteristic_uuid128(unsigned char *uuid128,int pro,int permission, uint32_t *attrHandle)
{
    bt_msg send_msg;
    bt_msg recv_msg;
    int result;
    uint8_t  properties;
    uint16_t attrValueFlags;
    uint16_t attrValueLength;

    memset(&recv_msg, 0, sizeof(bt_msg));
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_CREATE_CHAR_COMMAND;

    properties=pro;
    attrValueFlags=permission;
    attrValueLength = sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint8_t);
    if(uuid128 == NULL)
    {
        return -1;
    }

    memcpy(send_msg.uuid128, uuid128, 16);
    send_msg.check=128;

    send_msg.param1 = properties;
    send_msg.param2 = attrValueFlags;
    send_msg.param3 = attrValueLength;
    
    result = bt_send_sync_request(&send_msg,&recv_msg);
    if(result < 0)
    {
        return -1;
    }

    if(recv_msg.param1==-1)
    {
        return -2;
    }

    if(recv_msg.result != 1)
    {
        return -3;
    }

    *attrHandle = recv_msg.attrHandle;
    return 0;

}

int bt_gatt_create_descriptor()
{
    bt_msg send_msg;
    bt_msg recv_msg;
    int result;
   
    memset(&recv_msg, 0, sizeof(bt_msg));
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_CREATE_DESC_COMMAND;

    //result = bt_send_request(&send_msg, NULL);
    result = bt_send_sync_request(&send_msg, &recv_msg);
    if(result < 0)
    {
        return -1;
    }

    if(recv_msg.param1==-1)
    {
        return -2;
    }

    return 0;
}
int bt_gatt_add_service_2_db()
{
    bt_msg send_msg;
    bt_msg recv_msg;
    int result;
    
    memset(&recv_msg, 0, sizeof(bt_msg));
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_ADD_DB_COMMAND;

    result = bt_send_sync_request(&send_msg,&recv_msg);
    if(result < 0)
    {
        return -1;
    }

    if(recv_msg.param1==-1)
    {
        return -2;
    }
    return 0;
}


int bt_gatt_read_cfm(uint16_t       attrHandle, int rspCode, uint8_t *data, uint32_t data_len)
{
    bt_msg send_msg;
    int result;
    bt_msg recv_msg;
    memset(&recv_msg, 0, sizeof(bt_msg));
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_READ_CFM_COMMAND;
    send_msg.attrHandle = attrHandle;
    send_msg.data_len = data_len;
    memcpy(send_msg.data, data, data_len);
    send_msg.result = rspCode;

    result = bt_send_sync_request(&send_msg,&recv_msg);
    if(result < 0)
    {
        return -1;
    }

    if(recv_msg.param1==-1)
    {
        return -2;
    }

    return 0;
    
}

int bt_gatt_write_cfm(uint16_t       attrHandle, int rspCode)
{
    bt_msg send_msg;
    bt_msg recv_msg;
    int result;
    
    memset(&recv_msg, 0, sizeof(bt_msg));
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_WRITE_CFM_COMMAND;
    send_msg.attrHandle = attrHandle;
    send_msg.result = rspCode;
    
    result = bt_send_sync_request(&send_msg,&recv_msg);
    if(result < 0)
    {
        return -1;
    }

    if(recv_msg.param1==-1)
    {
        return -2;
    }
    return 0;
 
}

int bt_gatt_notification(uint16_t attrhandle,char *data)
{
    bt_msg send_msg;
    bt_msg recv_msg;
    int result;
    
    memset(&recv_msg, 0, sizeof(bt_msg));
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_NOTIFY_COMMAND;

    send_msg.attrHandle = attrhandle;
    send_msg.data_len = strlen(data)-1;
    strncpy((char*)send_msg.data, (const char *)data, strlen((const char *)data)-1);
    result = bt_send_sync_request(&send_msg,&recv_msg);
    if(result < 0){
        return -1;
    }

    if(recv_msg.param1==-1){
        return -2;
    }
    return 0;
}
int bt_gatt_indication(uint16_t attrhandle,char *data)
{
    bt_msg send_msg;
    bt_msg recv_msg;
    int result;
    memset(&recv_msg, 0, sizeof(bt_msg));
    memset(&send_msg, 0, sizeof(bt_msg));
    send_msg.command = BT_GATT_INDICATION_COMMAND;

    send_msg.attrHandle = attrhandle;
    send_msg.data_len = strlen(data)-1;
    strncpy((char*)send_msg.data, (const char*)data, strlen((const char*)data)-1);
    result = bt_send_sync_request(&send_msg,&recv_msg);
    if(result < 0){
        return -1;
    }

    if(recv_msg.param1==-1){
        return -2;
    }
    return 0;

}

int bt_get_scan_enable(int *inquiry_scan_status, int *page_scan_status)
{
    bt_msg send_msg;
    bt_msg recv_msg;    
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));
    int result;

    send_msg.command = BT_GET_SCAN_COMMAND;
    send_msg.param1 = 1;
    result = bt_send_sync_request(&send_msg, &recv_msg);
    if(result < 0)
    {
        return -1;
    }     

    if(recv_msg.param1 == 1)
    {
        *inquiry_scan_status = 1;
    }
    else
    {
        *inquiry_scan_status = 0;
    }

    if(recv_msg.param2 == 1)
    {
        *page_scan_status = 1;
    }
    else
    {
        *page_scan_status = 0;
    }
    
    return 0;
}

int bt_set_scan_enable(int inquiry_scan_status, int page_scan_status)
{
    bt_msg send_msg;
    bt_msg recv_msg;    
    int result;
    char    scan_string[10] = {0};
    memset(&send_msg, 0, sizeof(bt_msg));
    memset(&recv_msg, 0, sizeof(bt_msg));

    send_msg.command = BT_SET_SCAN_COMMAND;
    send_msg.param1 = inquiry_scan_status;
    send_msg.param2 = page_scan_status;
    result = bt_send_sync_request(&send_msg, &recv_msg);
    if(result < 0)
    {
        return -1;
    }

    if(recv_msg.result != 1)
    {
        return -2;
    }

    return 0;
}


int simcom_bt_power_on(int bPower)
{

    FILE *fp = NULL;
    char btname[128] = {0};
    const char process[] = "pidof csr_bt_app_default_hci|wc -L";
    //#define  LOG_FLAG;
    char start_app[512] = {0};
    const char power_on[]  = "echo 1 > /sys/class/rfkill/rfkill0/state";
    const char power_off[] = "echo 0 > /sys/class/rfkill/rfkill0/state";

#ifdef LOG_FLAG
    const char start_boot[] = "bt_bootstrap --bt-chip qca --bt-port /dev/ttyHS0 --bt-bs-baud 3000000 --use-pin 1 --flow-control 1 > /data/bt_console.log 2>&1";  
    const char start_app_fmt[] = "csr_bt_app_default_hci --bc-transport h4ibs --bc-port /dev/ttyHS0 --bc-baud 3000000 --bc-bs-baud 3000000 --platform MDM9607 --bc-bs-device-name %s --bt-dev carkit --ibs 1 --lpm 1 --log-cleartext-stdout --log-pcap-file /data/pcap.cap --log-btsnoop-file /data/hci.cfa --tui-nocls >/dev/null 2>&1 &"; 
#else
    const char start_boot[] = "bt_bootstrap --bt-chip qca --bt-port /dev/ttyHS0 --bt-bs-baud 3000000 --flow-control 1 > /dev/null 2>&1";        
    const char start_app_fmt[] = "csr_bt_app_default_hci --bc-transport h4ibs --bc-port /dev/ttyHS0 --bc-baud 3000000 --bc-bs-baud 3000000 --platform MDM9607 --bc-bs-device-name %s --bt-dev carkit --ibs 1 --lpm 1 >/dev/null 2>&1 &";
    const char start_app_fmt_old[] = "csr_bt_app_default_hci --bc-transport h4ibs --bc-port /dev/ttyHS0 --bc-baud 3000000 --bc-bs-baud 3000000 --platform MDM9607 --bc-bs-device-name %s --bt-dev carkit --lpm 0 >/dev/null 2>&1 &";
#endif
  
  
    if(bPower == 1)
    {
        system("rm -f /data/pcap.cap");
        system("rm -f /data/hci.cfa");
        get_bt_name_4_power_on(btname);

        printf("btname: %s\r\n", btname);
        printf("bt power on success!\r\n");

        sprintf(start_app, start_app_fmt, btname);
        int num = 0;
        fp = popen(process, "r");
        if(fp)
        {
            num = fgetc(fp);
            pclose(fp);
        }

        system(power_off);
        system(power_on);  
        poll(0,0,500);
        system(start_boot);
        if(num == '0')
        {
            system((const char *)start_app);
        }
    }
    else if(bPower == 2)
    {
        system("rm /data/pcap.cap");
        system("rm /data/hci.cfa");
        get_bt_name_4_power_on(btname);

        printf("btname: %s\r\n", btname);
        printf("bt power on success!\r\n");

        sprintf(start_app, start_app_fmt_old, btname);
        int num = 0;
        fp = popen(process, "r");
        if(fp)
        {
            num = fgetc(fp);
            pclose(fp);
        }

        system(power_off);
        system(power_on);  
        poll(0,0,500);
        system(start_boot);
        if(num == '0')
        {
            system((const char *)start_app);
        }
    }
    else
    {
        int result;
        bt_msg send_msg;
        bt_msg recv_msg;
        memset(&send_msg, 0, sizeof(bt_msg));
        memset(&recv_msg, 0, sizeof(bt_msg));

        send_msg.command = BT_POWER_DOWN_COMMAND;
        result = bt_send_sync_request(&send_msg,&recv_msg);
        if(result < 0)
        {
            return -1;
        }
        poll(0,0,300);
        system(power_off);
        printf("bt power off success!\r\n");
        //system("pidof csr_bt_app_default_hci|xargs kill");
        poll(0,0,200);
    }
    return 0;
}



int bt_init(bt_ind_cb_fcn handle)
{
    return create_dameon_recv_thread((void*)handle);
}


