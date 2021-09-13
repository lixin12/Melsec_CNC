#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>
#include <string.h>
#include "ssl.h"
#include "type.h"
#define MQTT_QOS         0  /*0-1-2*/
#define MQTT_INIT        1
#define MQTT_Connect     2
#define MQTT_Connected   3

static char topic[100] = {0};
static bool session = true;

static unsigned char current_status_mq = MQTT_INIT;
static struct mosquitto *mosq = NULL;
static int mqtt_Init(void)
{
    //libmosquitto 库初始化
    mosquitto_lib_init();
    //创建mosquitto客户端
    mosq = mosquitto_new(NULL,session,NULL);
    if(!mosq){
        zlg_debug("create client failed..\r\n");
        mosquitto_lib_cleanup();
        return 1;
    }
	else
	{
		zlg_debug("MQTT INIT OK!\r\n");
		return 0;
	}
}

static int mqtt_Connect(void)
{
	if(mosquitto_username_pw_set(mosq,USERNAME,PASSWORD))
	{
		mosquitto_destroy(mosq);
		mosquitto_lib_cleanup();
        zlg_debug("Unable to set username for mq.\r\n");
        return 5;
	}
	else
	{
		zlg_debug("MQ Set username OK!\r\n");
	}
	 //连接服务器
    if(mosquitto_connect(mosq, IOT_DomainNameToIP.Get_DomainNameIP(), PORT, KEEP_ALIVE)){
		mosquitto_destroy(mosq);
		mosquitto_lib_cleanup();
        zlg_debug("Unable to connect.\r\n");
        return 2;
    }
	else
	{
		zlg_debug("MQTT Connect OK!\r\n");
	}
    //开启一个线程，在线程里不停的调用 mosquitto_loop() 来处理网络信息
    int loop = mosquitto_loop_start(mosq);
    if(loop != MOSQ_ERR_SUCCESS)
    {
		mosquitto_destroy(mosq);
		mosquitto_lib_cleanup();
        zlg_debug("mosquitto loop error\n");
        return 3;
    }
	else
	{
		zlg_debug("MQTT Thread Start OK!\r\n");
		return 0;
	}
}

static int mqtt_Connected(MQData* mqdata)
{
	char* dataBuf = 0;
    if(mqdata==NULL)
    {
        zlg_debug("fifo_data null\r\n");
        return -1;
    }
	dataBuf = IOT_Data.Create_MQTT_Transfer_Data(mqdata);
   
	sprintf(topic,"%s/%s",IOT_PBoxInfo.PBoxInfo_clientId(),IOT_PBoxInfo.PBoxInfo_templateUniqueId());
	if(MOSQ_ERR_SUCCESS == mosquitto_publish(mosq,NULL,topic,strlen(dataBuf)+1,dataBuf,MQTT_QOS,0))
	{
		zlg_debug("dataBuf:%s\r\n",dataBuf);
		return 0;
	}
	else
	{
		mosquitto_loop_stop(mosq,1);
		mosquitto_destroy(mosq);
		mosquitto_lib_cleanup();
		zlg_debug("MQTT Send Error\r\n");
		return 4;
	}
}
int MQTT_TICK = 1;
void mqtt_PubLogic(void)
{
	MQData temp = {0};
	switch(current_status_mq)
	{
		case MQTT_INIT:
			if(mqtt_Init()!=0)
			{
				current_status_mq = MQTT_INIT;
			}
			else
			{
				current_status_mq = MQTT_Connect;
			}
		break;
		case MQTT_Connect:
			if(mqtt_Connect()!=0)
			{
				current_status_mq = MQTT_INIT;
			}
			else
			{
				current_status_mq = MQTT_Connected;
                zlg_debug("MQTT Connected OK\r\n");
			}
		break;
		case MQTT_Connected:
			while(!is_empty_MQData())
			{
				temp = front_MQData();
				MQTT_TICK++;
				if(mqtt_Connected(&temp)==0)
				{
					zlg_debug("MQTT Send OK\r\n");
					pop_MQData();
				}
				else
				{
					current_status_mq = MQTT_INIT;
					return;
				}
			}
		   
		break;
		default:
		break;
	}
}
int mqtt_PubIsOnline(void)
{
	return current_status_mq==MQTT_Connected;
}

_IOT_MQTT_PUB IOT_mqtt_pub = 
{
	mqtt_PubLogic,
	mqtt_PubIsOnline
};
