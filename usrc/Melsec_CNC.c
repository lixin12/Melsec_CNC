#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h> 
#include <sys/ioctl.h> 
#include <net/if.h> 
#include <net/if_arp.h> 
#include <arpa/inet.h> 

#include "type.h"
#include "log.h"
#include "Siemens_CNC.h"

#define     T1           (20)    /*Connect timeout*/
extern u32 TIMTICK;

char uart_rev[200][50] = {0};
char* uart_result[200] = {0};
char sendDataReault[MQTT_MSG_MAX_LEN-40] = {0};
int uart_resultCnt = 0;
int uart_isSendData = 0;

int current_status = _INIT;

char serverIP[20] = {"192.168.0.102"};
u32 serverPort = 102; //   Port

char rev[2048];
int len;
int sock;
unsigned long sck_mode;

struct sockaddr_in addr;

static int si_CNC_send_end(void);

static void print_Si_CNC_Data(char* data,int flag,int len)
{
	u16 i = 0; 
    char buff[4096] = {0};
	for(i=0 ; i<len ; i++)
	{
		sprintf(buff+i*3,"%02X ",data[i]);
	}
	if(flag)
	{
		zlg_debug("Si_CNC_SendData:%s\r\n",buff);
	}
	else
	{
		zlg_debug("Si_CNC_RecvData:%s\r\n",buff);
	}
}

int Si_CNC_Init(void)
{
	int i = 0;
	if(sock>0)
	{
		close(sock);
	}
	for(i=0 ; i<200 ; i++)
	{
		uart_result[i] = uart_rev[i];
	}
    current_status = _INIT;

	bzero(&addr,sizeof(addr));
	addr.sin_port = htons(serverPort);
	addr.sin_family = PF_INET;
	strcpy(serverIP,IOT_PBoxInfo.PBoxInfo_equipmentIP());
	addr.sin_addr.s_addr = inet_addr(serverIP);
	sock = socket (AF_INET, SOCK_STREAM , 0);
	if(sock<0)
	{
		return sock;
	}

	return 0;
}


int Si_CNC_NotConnect(void)
{
	int res;
	res = connect (sock, (struct sockaddr_in *)&addr, sizeof (addr));
	return res;
}

int Si_CNC_Collect(void)
{
	int res = 0;
    char* sendData = NULL;
    char* datarev = NULL;
	int i = 0;
	int cnt = Get_Si_CNC_Collect_Num();
	zlg_debug("cmd cnt = %d\r\n",cnt);
	//for(i=0 ; i<cnt ; i++)
	for(i=0 ; i<cnt ; i++)
	{
		sendData = Get_Si_CNC_CMD_STR(i);//数据打包
		res = send (sock, (char*)(sendData), sendData[3], 0);
		//sleep(1);
		if(res<0)
		{
			zlg_debug("Send Error : %d\r\n",res);
			return 2;
		}
		print_Si_CNC_Data(sendData,1,sendData[3]);
		res = recv (sock, (char*)rev, sizeof(rev), 0);
		if(res>0)
		{
			datarev = rev;
			rev[res] = 0;
		}
		else
		{
			datarev = 0;
		}
		//datarev = IOT_Uart.uart_revDataTimeOut(baud,200);
		if(datarev!=0)
		{
			print_Si_CNC_Data(datarev,0,res);
			//datarev+=2;
			if(0==Si_CNC_AnalyzeData(datarev,i))
			{
			}
			else
			{
				if(close(sock)==0)
					zlg_debug("socket close OK\r\n",i);
				else
					zlg_debug("socket close failed\r\n",i);
				zlg_debug("analyze error! index = %d\r\n",i);
				return 1;
			}
		}
		else
		{
			zlg_debug("receive data error! index = %d\r\n",i);
			return 2;
		}
	}
	return 0;
}


static int si_CNC_send_end(void)
{
	int res = 0;
	unsigned char p_send[] = { 0x03, 0x00, 0x00, 0x07, 0x02, 0xf0, 0x00 };
	res = send(sock,(unsigned char*)p_send, p_send[3], 0);
	print_Si_CNC_Data(p_send,1,p_send[3]);
	if(res<0)
	{
		zlg_debug("Send Error : %d\r\n",res);
		return 1;
	}
	return 0;
}


int m_CMDth = 0;

int link_Si_CNC(void)
{
	int res = 0;
	m_CMDth = 0;
	Si_CNC_data.m_nCycleNum = 0;
	unsigned char p_send[] = { 0x03,0x00,0x00,0x16,0x11,0xe0,0x00,0x00,0x00,0x48,0x00,0xc1,0x02,0x04,0x00,0xc2,0x02,0x0d,0x04,0xc0,0x01,0x0a };
	p_send[9] = m_CMDth;
	res = send(sock,(unsigned char*)p_send, p_send[3], 0);
	print_Si_CNC_Data(p_send,1,p_send[3]);
	if(res<0)
	{
		zlg_debug("Send Error : %d\r\n",res);
		return 1;
	}
	res = recv (sock, (char*)rev, sizeof(rev), 0);
	if(res>0)
	{
		print_Si_CNC_Data(rev,0,res);
	}
	else
	{
		return 2;
	}
	unsigned char p_send1[] = { 0x03,0x00,0x00,0x19,0x02,0xf0,0x80,0x32,0x01,0x00,0x00,0x00,0x01,0x00,0x08,0x00,0x00,0xf0,0x00,0x00,0x64,0x00,0x64,
	0x03,0xc0 };
	res = send(sock,(unsigned char*)p_send1, p_send1[3], 0);
	print_Si_CNC_Data(p_send1,1,p_send1[3]);
	if(res<0)
	{
		zlg_debug("Send Error : %d\r\n",res);
		return 3;
	}
	res = recv (sock, (char*)rev, sizeof(rev), 0);
	if(res>0)
	{
		print_Si_CNC_Data(rev,0,res);
	}
	else
	{
		return 4;
	}
	unsigned char p_send2[] = { 0x03,0x00,0x00,0x1d,0x02,0xf0,0x80,0x32,0x01,0x00,0x00,0x00,0x01,0x00,0x0c,0x00,0x00,0x04,0x01,0x12,0x08,0x82,0x01,
	0x00,0x14,0x00,0x01,0x3b,0x01 };
	res = send(sock,(unsigned char*)p_send2, p_send2[3], 0);
	print_Si_CNC_Data(p_send2,1,p_send2[3]);
	if(res<0)
	{
		zlg_debug("Send Error : %d\r\n",res);
		return 5;

	}
	res = recv (sock, (char*)rev, sizeof(rev), 0);
	if(res>0)
	{
		print_Si_CNC_Data(rev,0,res);
	}
	else
	{
		return 6;
	}
	if(si_CNC_send_end()!=0)
	{
		return 0;
	}
	return 0;
}

unsigned int notConnectTimeOut = 0;
int SECS_TICK = 1;
void app_Si_CNC_Logic(void)
{
	u8 t = 0;
	u16 i = 0;
	int res = 0;
	char tmp[30] = {0};
	switch(current_status)
	{
		case _INIT:
			zlg_debug("INIT\r\n");
			if(Si_CNC_Init()!=0)
			{
				current_status = _INIT;
			}
			else
			{
				current_status = _NotConnect;
			}
		break;
		case _NotConnect:
			zlg_debug("_NotConnect\r\n");
			if(Si_CNC_NotConnect()!=0)
			{
				current_status = _NotConnect;
				notConnectTimeOut++;
                sleep(2);
				if(notConnectTimeOut > T1)
				{
					notConnectTimeOut = 0;
					current_status = _INIT;
				}
			}
			else
			{
				notConnectTimeOut = 0;
				current_status = _Si_CNC_Connect;
			}
		break;
		case _Si_CNC_Connect:
			zlg_debug("_Si_CNC_Connect\r\n");
			if(link_Si_CNC()!=0)
			{
				current_status = _INIT;
			}
			else
			{
				current_status = _Collect_CNC;
			}
			zlg_debug("ststus=%d\r\n",current_status);
		break;
		case _Collect_CNC:
            SECS_TICK++;
			if(!is_empty_MQCmd())
			{
				zlg_debug("is_empty_MQCmd success\r\n");
				if(front_MQCmd().flag==1)
				{
					pop_MQCmd();
					if(0==Si_CNC_Collect())
					{
						pack_analyze_result();//数据打包 
						//RSTCP_SendData(TC_GetResult(0));
						strncpy(sendDataReault,SI_CNC_GetResult(0),MQTT_MSG_MAX_LEN-40);
						zlg_debug("analyze success:%s\r\n",SI_CNC_GetResult(0));
						SI_CNC_ClearResult();
					}
					else
					{
						SI_CNC_ClearResult();
						current_status = _INIT;
					}
				}
			}
		break;
	}
}

unsigned char bsp_GetOnLine(void)
{
	return (current_status == _Collect_CNC);
}

void app_TestMQ(void)
{
	int i=0;
	if(current_status==_Collect_CNC)
	{
		MQCmd temp = {0};
		temp.flag = 1;
		push_MQCmd(temp);
	}
	else
	{
		//memset(sendDataReault,0,MQTT_MSG_MAX_LEN-40);
		memset(uart_rev,0,200*30);
		strncpy(sendDataReault,"-1;-1;-1;",MQTT_MSG_MAX_LEN-40);
		
	}
}


static int split(char *src,const char *separator,char **dest,int *num) {
     char *pNext;
     int count = 0;
     if (src == NULL || strlen(src) == 0) //?????????????0,???? 
        return 1;
     if (separator == NULL || strlen(separator) == 0) //??????????,???? 
        return 2;
     pNext = (char *)strtok(src,separator); //????(char *)????????(??????????????????)
     while(pNext != NULL) {
          strcpy(*dest++ , pNext);
          ++count;

         pNext = (char *)strtok(NULL,separator);  //????(char *)????????
    }  
    *num = count;
	return 0;
} 


typedef enum {BYTE=1,SHORT=2,INT=3,FLOAT=4,STRING=5} DataTypeAPP;
extern void DataSerialization(u8 dataType,int* pos,void* data,char* dataDiscripton);
void app_interface_binary_data(int* pos,MQData* data)
{
	int i = 0;
	int uart_resultCnt = 0;
	char buf[20] = {0};
	const char sep = ';';
	simcom_lbs_receive_info_t* templbs = IOT_GPS.IOT_GetAGPSInfoVal();
	int dataCnt = Get_SendDataCNT();
	split(data->data,&sep,uart_result,&uart_resultCnt);

    for(i=0 ; i< dataCnt; i++)
   	{
		sprintf(buf,"%d->",i);
		//printf("uart_result[%d] : %s\r\n",i,uart_result[i]);
		DataSerialization((u8)STRING,pos,(void*)(uart_result[i]),buf);
	}
	//获取经纬度值
	sprintf(buf,"%f",(float)(templbs->u32Lng)/1000000);
	DataSerialization((u8)STRING,pos,(void*)buf,"longitude");

	sprintf(buf,"%f",(float)(templbs->u32Lat)/1000000);
	DataSerialization((u8)STRING,pos,(void*)buf,"latitude");
	memset(uart_rev,0,200*30);
}

void app_ConvertToQueue(void)
{
	MQData temp = {0};
	strncpy(temp.data,sendDataReault,MQTT_MSG_MAX_LEN-40);
	IOT_NTP.get_date_time(temp.collTime);
	if(!is_full_MQData())
	{
		push_MQData(temp);
	}
	else
	{
		zlg_debug("is_full_MQData is true\r\n");
	}
}
