#include "type.h"
#include "log.h"
#include "Siemens_CNC.h"

char* MQTTHOST = "www.ziyun-cloud.com";
int   PORT     = 21669;
char* USERNAME = "admin";
char* PASSWORD = "password";
char* TOPIC    = "bc8c2f1d5f6545b8902a2e26da96e55b/5KYK37p0EJXd6Ej76Rtp";
int   KEEP_ALIVE = 60;

char* HTTPHOST =  "www.ziyun-cloud.com";
char* PBOXINFO_URL = "/service/iots/pBox/template-info";
char* PBOXUPDATE_URL = "/service/iots/pBox/update/iccId";

char* IOT_Analyze_Version = "4GLinuxTC_V1.0"; 
char* Program_Version = "4GLinuxTC_V1.0.20201230" ;
int DataSendFrenq = 8;//数据发送频率

CREATE_ARRAY_QUEUE(MQCmd,_MQCmd,5);
CREATE_ARRAY_QUEUE(MQData,_MQData,1200);

void main(void)
{
	int ret = 0;
	IOT_APP_Init();
	sleep(2);
	IOT_GPS.IOT_AGPSInit();
	IOT_APP_Task_Create();

	/*
	user code
	*/
	for(;;)
	{
		IOT_APP_Exit_Check();
		sleep(2);
	}
}
