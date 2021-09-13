#include "log.h"
#include "Siemens_CNC.h"
#include "usr_cfg.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "type.h"

int Get_lan_mode_Value( char *filename )
{
    int step = 10;
	char buf[10] = {0};
    int file = open( filename, O_RDONLY );
    if ( file == -1 )
        return -1;
	int cur_len = read( file, buf, step );
	if(cur_len<=0)
	{
		close( file );
		return -1;
	}
	else
	{
		close( file );
		printf("buf[0]=%c\r\n",buf[0]);
		if(buf[0]>='0' && buf[0]<='9')
			return buf[0]-'0';
	}
    
    return -2;
}

extern u8 app_GetDistMac(void);
extern int DataSendFrenq ;

u32 TIMTICK = 1;
int exit_flag_cnt = 0;
void exit_handler(int sig)
{
	if(SIGTERM == sig)
	{
		if(exit_flag_cnt == 0)
		{
			exit_flag_cnt = TIMTICK;
		}
	}
}
void updateTimtick(int* data)
{
	for(;;)
	{
		TIMTICK++;
		usleep(1000);
	}
}
int ADC_HSMS_CMD_TICK = 1;
int Safety_ADC_HSMS_CMD_TICK = 0;
void getADCVal(int* data)
{
	for(;;)
	{
		ADC_HSMS_CMD_TICK++;
		data[0] = IOT_ADC.read_adc(1);
		data[1] = IOT_ADC.read_adc(2);
		zlg_debug("ADC1=%d   ADC2=%d\r\n",data[0],data[1]);
		if(data[1]<500)
		{
			app_ConvertToQueue();
			sleep(1);
		}
		else
		{

		}
		app_TestMQ();
		sleep(6);
	}
}
int Queue_TICK = 1;
int Safety_Queue_TICK = 0;
void Data_To_Queue(void)
{
	sleep(1);
	for(;;)
	{
		Queue_TICK++;
		sleep(DataSendFrenq);
		app_ConvertToQueue();
	}
}
extern int MQTT_TICK;
int Safty_MQTT_TICK = 0;
void MqttToCloud(void)
{
	for(;;)
	{
		MQTT_TICK++;
		IOT_mqtt_pub.mqtt_PubLogic();
		usleep(600000);
	}
}
int NTP_TICK = 1;
int Safety_NTP_TICK = 0;
void DistMACUpdate(void)
{
	for(;;)
	{
		NTP_TICK++;
		//app_GetDistMac();
		IOT_GPS.IOT_GetAGPSInfo();
		sleep(60);
	}
}
extern int SECS_TICK;
int Safty_SECS_TICK = 0;
void Logic(void)
{
	for(;;)
	{
		//zlg_debug("logic running \r\n");
		app_Si_CNC_Logic();
		usleep(15000);
	}
}
int Safety_Flag[20] = {0};
int Safety_Process(int* lasttimeTick,int* tick,int* Safety_Flag,int cnt)
{
	if(*lasttimeTick==0)
	{
		*lasttimeTick = *tick;
	}
	else if(*lasttimeTick == *tick)
	{
		*Safety_Flag += 1;
		if(*Safety_Flag>=cnt)
		{
			return 1;
		}
	}
	else if(*lasttimeTick != *tick)
	{
		*lasttimeTick = *tick;
		*Safety_Flag = 0;
	}
	return 0;
}

void Safety_Process_Thread(void)
{
	for(;;)
	{
		if((Safety_Process(&Safty_SECS_TICK,&SECS_TICK,&Safety_Flag[0],12)==0)    && \
		   (Safety_Process(&Safety_Queue_TICK,&Queue_TICK,&Safety_Flag[3],12)==0) && \
		   (Safety_Process(&Safty_MQTT_TICK,&MQTT_TICK,&Safety_Flag[4],12)==0) && \
		   (Safety_Process(&Safety_ADC_HSMS_CMD_TICK,&ADC_HSMS_CMD_TICK,&Safety_Flag[9],12)==0))    /*//6s      */
		{
			IOT_LED.Feed_Watchdog();
			sleep(10);
			/*zlg_debug("Safty_SECS_TICK:%d ; SECS_TICK:%d ; Safety_Flag[0]:%d!\r\n",Safty_SECS_TICK,SECS_TICK,Safety_Flag[0]);
			zlg_debug("Safety_NTP_TICK:%d ; NTP_TICK:%d ; Safety_Flag[1]:%d!\r\n",Safety_NTP_TICK,NTP_TICK,Safety_Flag[1]);
			zlg_debug("Safety_MQTT_TICK:%d ; MQTT_TICK:%d ; Safety_Flag[2]:%d!\r\n",Safety_MQTT_TICK,MQTT_TICK,Safety_Flag[2]);
			zlg_debug("Safety_Queue_TICK:%d ; Queue_TICK:%d ; Safety_Flag[3]:%d!\r\n",Safety_Queue_TICK,Queue_TICK,Safety_Flag[3]);
			zlg_debug("Safety_ADC_HSMS_CMD_TICK:%d ; ADC_HSMS_CMD_TICK:%d ; Safety_Flag[4]:%d!\r\n",Safety_ADC_HSMS_CMD_TICK,ADC_HSMS_CMD_TICK,Safety_Flag[4]);*/
		}
		else
		{
			sleep(10);
		}
	}
}
int adcValue[2] = {0};
char NetMask[] = "255.255.255.0";

void ping_server(void)
{
	char cmd[128] = {'\0'}; 
	snprintf(cmd,127, "ping %s -c 4","114.114.114.114");
	zlg_debug("%s\r\n",cmd);
	system(cmd); 
	sleep(5);
}

void IOT_APP_Init(void)
{
	int cnt = 0;
	int ret = 0;
	int sleepTime = 0;
    IOT_LED.IOT_GPIO_Init();
	IOT_LED.Feed_Watchdog();
	signal(SIGTERM,exit_handler);

	cnt = 0;
	IOT_DomainNameToIP.ConvertHostNameToIP();
	IOT_LED.Feed_Watchdog();
	while(IOT_NTP.NTP_UpdateSysTime()!=0)
	{
		cnt++;
		if(cnt>20)
		{
			zlg_debug("NTP_UpdateSysTime error\r\n");
			exit(-1);
		}
		//ping_server();
		sleep(5);
	}

	
	IOT_LED.Feed_Watchdog();
	cnt = 0;
	while(IOT_http_s.Http_CheckCheck()!=0)
	{
		cnt++;
		if(cnt>20)
		{
			zlg_debug("Http_CheckCheck error\r\n");
			exit(-1);
		}
		sleep(5);
	}
	sleep(5);
	IOT_LEDCONTROL.IOT_LEDControl();
	IOT_SIM7600CE.SIM7600CE_Init();
	cnt = 0;
	sleepTime = 20;
	loop:ret = IOT_PBoxInfo.GetPBoxInfo();
	if(ret!=0)
	{
		cnt++;
		if(cnt>10)
		{
			zlg_debug("GetPBoxInfo error\r\n");
			exit(-1);
		}
		while(sleepTime>0)
		{
			sleepTime = sleep(sleepTime);
		}
		goto loop;
	}
	if(Get_lan_mode_Value("/data/lan_mode")!=0)
	{
		//IOT_IPSet.set_MAC(IOT_SIM7600CE.Get_IMEI()+2);
		sleep(10);
		IOT_IPSet.set_ip(IOT_PBoxInfo.PBoxInfo_pbox_own_ip());
		IOT_IPSet.set_ip_netmask(NetMask);
		//IOT_IPSet.set_gateway(IOT_PBoxInfo.PBoxInfo_pbox_own_gateWay());
	}
}

void IOT_APP_Task_Create(void)
{
    int ret = 0;
	
	pthread_t GetDISTMAC;
	ret = pthread_create(&GetDISTMAC,NULL,(void*)DistMACUpdate,NULL);
	if(ret!=0)
	{
		zlg_debug("Thread GetDISTMAC create error quit!\r\n");
	}
	pthread_t MqttToCloudThread;
	ret = pthread_create(&MqttToCloudThread,NULL,(void*)MqttToCloud,NULL);
	if(ret!=0)
	{
		zlg_debug("Thread MqttToCloud create error quit!\r\n");
	}
	pthread_t TestADC;
	ret = pthread_create(&TestADC,NULL,(void*)getADCVal,(void*)&adcValue);
	if(ret!=0)
	{
		zlg_debug("Thread TestADC create error quit!\r\n");
	}
	
	pthread_t UpdateTimtick;
	ret = pthread_create(&UpdateTimtick,NULL,(void*)updateTimtick,NULL);
	if(ret!=0)
	{
		zlg_debug("Thread UpdateTimtick create error quit!\r\n");
	}
	
	pthread_t Logic_pthread;

	ret = pthread_create(&Logic_pthread,NULL,(void*)Logic,NULL);
	if(ret!=0)
	{
		zlg_debug("Thread TCLogic create error quit!\r\n");
	}

	pthread_t DATA_QUEUE;
	ret = pthread_create(&DATA_QUEUE,NULL,(void*)Data_To_Queue,NULL);
	if(ret!=0)
	{
		zlg_debug("Thread DATA_QUEUE create error quit!\r\n");
	}

	pthread_t Safty_PThread;
	ret = pthread_create(&Safty_PThread,NULL,(void*)Safety_Process_Thread,NULL);
	if(ret!=0)
	{
		zlg_debug("Thread Safty_PThread create error quit!\r\n");
		exit(-1);
	}
}

void IOT_APP_Exit_Check(void)
{
    if(exit_flag_cnt!=0 && (is_empty_MQData()==1))
    {
        zlg_debug("MQData empty and exit process!\r\n");
        IOT_LED.Feed_Watchdog();
        exit(0);
    }
    else if(exit_flag_cnt!=0 && (TIMTICK-exit_flag_cnt)>=600000)
    {
        zlg_debug("Time out and exit process!\r\n");
        IOT_LED.Feed_Watchdog();
        exit(1);
    }
}

u16 IOT_APP_GetADValue(void)
{
    return adcValue[0];
}
