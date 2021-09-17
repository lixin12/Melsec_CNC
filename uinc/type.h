#ifndef _TYPE_H
#define _TYPE_H
#include "queue_array.h"
#include "usr_cfg.h"
#include "GPSControl.h"
#include "simcom_common.h"
#include "simcom_lbs.h"
#include <signal.h>
#include <time.h>

#define MQTT_MSG_MAX_LEN	(1024*2)
#define ZLG_DEBUG 2

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;

#define _INIT            0x1
#define _NotConnect      0x2 
#define _Collect_CNC     0x3

typedef struct
{
	char data[MQTT_MSG_MAX_LEN-40];
	char collTime[20]; 	
}MQData;

typedef struct
{
	u8 flag;
}MQCmd;

extern MQData     front_MQData(void);
extern void        pop_MQData(void);
extern u8          is_full_MQData(void);
extern void        push_MQData(MQData ps);
extern u8          is_empty_MQData(void);
extern u32          size_MQData(void);

extern MQCmd   	 front_MQCmd(void);
extern void        pop_MQCmd(void);
extern u8          is_full_MQCmd(void);
extern void        push_TCCmd(MQCmd ps);
extern u8          is_empty_MQCmd(void);
extern u32          size_MQCmd(void);

typedef struct
{
	u32 IP1;
	u32 IP2;
	u32 IP3;
	u32 IP4;
	u32 Port;
}HostIPPort;

typedef struct
{
	u16 svid;                //SVID
	u8 analyzeType;          //SVID¶ÔÓ¦µÄÊýÖµ½âÎö³ÉºÎÖÖÊýÖµÀàÐÍ
	long addr;                //½âÎö½á¹ûÔÝ´æµØÖ·
}SVIDINFO;

typedef struct 
{
	int (*IOT_GNSSInit)(void);
	GpsInfo* (*IOT_GetGpsInfo)(void);
	void (*IOT_AGPSInit)(void);
	simcom_lbs_receive_info_t* (*IOT_GetAGPSInfo)(void);
	simcom_lbs_receive_info_t* (*IOT_GetAGPSInfoVal)(void);
}_IOT_GPS;
extern _IOT_GPS IOT_GPS;

//ADC.C
typedef struct
{
    ///读取ADC的值，1或2通道
    int (*read_adc) (int ch);
}_IOT_ADC;
extern _IOT_ADC IOT_ADC;

//SIM7600CE.C
typedef struct
{
    ///读取ADC的值，1或2通道
    void (*SIM7600CE_Init) (void);
	char* (*Get_IMEI) (void);
}_IOT_SIM7600CE;
extern _IOT_SIM7600CE IOT_SIM7600CE;

//PBoxInfo.C
typedef struct
{
    ///读取ADC的值，1或2通道
    int (*GetPBoxInfo) (void);
	char* (*PBoxInfo_templateUniqueId) (void);
	char* (*PBoxInfo_deviceId) (void);
	char* (*PBoxInfo_clientId) (void);
	char* (*PBoxInfo_pbox_own_ip) (void);
	char* (*PBoxInfo_equipmentIP) (void);
	char* (*PBoxInfo_pbox_own_gateWay) (void);
}_IOT_PBoxInfo;
extern _IOT_PBoxInfo IOT_PBoxInfo;

//NTP.c
typedef struct
{
    ///读取ADC的值，1或2通道
    int (*NTP_UpdateSysTime) (void);
	int (*get_date_time) (char* psDateTime);
}_IOT_NTP;
extern _IOT_NTP IOT_NTP;

//mqtt_s.c
typedef struct
{
    ///读取ADC的值，1或2通道
    void (*mqtt_SendLogic) (void);
	int (*mqtt_IsOnline) (void);
}_IOT_mqtt_s;
extern _IOT_mqtt_s IOT_mqtt_s;

//mqtt_s.c
typedef struct 
{
    ///获取请求http的返回值
    int (*http_get)(char *url, char *rename);
	int (*Http_CheckCheck)(void);
}_IOT_http_s;
extern _IOT_http_s IOT_http_s;
//IOTData.c
typedef struct 
{
	char* (*Create_MQTT_Transfer_Data)( MQData *data);
}_IOT_Data;
extern _IOT_Data IOT_Data;

typedef struct 
{
	int (*GenerateSecsBinaryData) (MQData* data);
}_IOT_SecsBinaryCreate;
extern _IOT_SecsBinaryCreate IOT_SecsBinaryCreate;

typedef struct 
{
	int (*set_ip)(unsigned char ip[16]);
	int (*set_ip_netmask)(unsigned char ip[16]);
	int (*set_gateway)(unsigned char ip[16]);
	int (*set_MAC)(unsigned char *pMac) ;
}_IOT_IPSet;
extern _IOT_IPSet IOT_IPSet;

typedef struct
{
	int (*IOT_GPIO_Init)(void);
	int (*gpio_set_value)(unsigned int gpio, unsigned int value);
	int (*gpio_get_value)(unsigned int gpio);
	void(*Feed_Watchdog)(void);
	int LED_LTE_GPIO;
    int LED_LED1_GPIO;
	int LED_LED2_GPIO;
	int WATCHDOG_GPIO;
	int DO1_GPIO     ;
	int DI1_GPIO     ;
}_IOT_LED;
extern _IOT_LED IOT_LED;

//IOT_Timer.c
typedef struct
{
	void (*create_timer)(long long freq_nanosecs,timer_t* timerid);
	void (*destroy_timer)(timer_t* timerid);
	void (*handler)(int sig, siginfo_t *si, void *uc);
}_IOT_TIMER;
extern _IOT_TIMER IOT_TIMER;

//IOT_LEDControl.c
typedef struct
{
	void (*IOT_LEDControl)(void);
}_IOT_LEDCONTROL;
extern _IOT_LEDCONTROL IOT_LEDCONTROL;

//IOT_Uart.c
typedef struct
{
	char* (*uart_revDataTimeOut)(int baud,int timeout);
	int (*uart_SendData)(int baud,char* data,int length);
	void (*uart_dataLog)(char* tag,char* data,int length);
	int (*test485)(u32 baud);
}_IOT_UART;
extern _IOT_UART IOT_Uart;

//
typedef struct
{
	int (*ConvertHostNameToIP)(void);
	char* (*Get_DomainNameIP)(void);
}_IOT_DomainNameToIP;
extern _IOT_DomainNameToIP IOT_DomainNameToIP;

typedef struct 
{
	void (*mqtt_PubLogic)(void);
	int (*mqtt_PubIsOnline)(void);
}_IOT_MQTT_PUB;
extern _IOT_MQTT_PUB IOT_mqtt_pub;
#endif
