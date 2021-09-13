#include "stdio.h"
#include "stdbool.h"
#include "type.h"
#include "log.h"

unsigned char BinaryData[MQTT_MSG_MAX_LEN] = {0};

typedef union _float {
  float f;
  char  bufF[4];
} FloatUnion;

typedef union _int {
  int i;
  char  bufI[4];
} IntUnion;

typedef union _short {
  short S;
  char  bufS[2];
} ShortUnion;
extern char* IOT_Analyze_Version;
typedef enum { IntType = 0, FloatType = 1, StringType = 2,ByteType = 3,ShortType = 4} DataType;

//check the position is valid. If not valid, return -1;
static bool CheckPosition(unsigned int position){
	if(position < MQTT_MSG_MAX_LEN)
	{
		return true;
	}
	else{
		return false;
	}
}

unsigned int Write_secs_FloatData(unsigned int pos, float fVal){
   
    bool ret;
    int i = 0;
	int position = pos;
	FloatUnion fu;
	fu.f = fVal;
  ret = CheckPosition(position + 5);
	if(ret == false){
		printf("Data exceed mqtt max length.");
		return position;
	}
	
	BinaryData[position++] = FloatType;
	for(i = 0; i < 4; i++){	
		if(position + i < MQTT_MSG_MAX_LEN){
			BinaryData[position++] = fu.bufF[i];			
		}
	}
	return position;
}

unsigned int Write_secs_IntData(unsigned int pos, int iVal){
	
	bool ret;
	int i = 0;
	int position = pos;
	IntUnion iu;
	iu.i = iVal;
  ret = CheckPosition(position + 5);
	if(ret == false){
		printf("Data exceed mqtt max length.");
		return position;
	}
	
	BinaryData[position++] = IntType;
	
	for(i = 0; i < 4; i++){	
		if(position + i < MQTT_MSG_MAX_LEN){
			BinaryData[position++] = iu.bufI[i];
		}
	}
	
	return position;
}

unsigned int Write_secs_ByteData(unsigned int pos, char iVal){
	
	bool ret;
	int i = 0;
	int position = pos;
 
  ret = CheckPosition(position + 2);
	if(ret == false){
		printf("Data exceed mqtt max length.");
		return position;
	}
	BinaryData[position++] = ByteType;
	if(position + i < MQTT_MSG_MAX_LEN)
	{
		BinaryData[position++] = iVal;
	}
	return position;
}

unsigned int Write_secs_ShortData(unsigned int pos, short iVal){
	
	bool ret;
	int i = 0;
	int position = pos;
	ShortUnion iu;
	iu.S = iVal;
 
  ret = CheckPosition(position + 3);
	if(ret == false){
		printf("Data exceed mqtt max length.");
		return position;
	}
	BinaryData[position++] = ShortType;
	for(i = 0; i < 2; i++){	
		if(position + i < MQTT_MSG_MAX_LEN){
			BinaryData[position++] = iu.bufS[i];
		}
	}
	return position;
}

unsigned int Write_secs_StringData(unsigned int pos, char *pStr){
	int i = 0;
	int position = pos;
	int length = strlen(pStr);
	bool ret = CheckPosition(position + length + 2);
	
	if(ret == false){
		printf("Data exceed mqtt max length.");
		
		return position;
	}
	
	BinaryData[position++] = StringType;
	
	BinaryData[position++] = length;
	
	for(i = 0; i < length; i++){	
		if(position + i < MQTT_MSG_MAX_LEN){
			BinaryData[position++] = *(pStr+i);
		}
	}
	
	return position;
}

void DataSerialization(u8 dataType,int* pos,void* data,char* dataDiscripton)
{
	switch(dataType)
	{
		case 1://Byte
			*pos = Write_secs_ByteData(*pos, *((char*)data));
			if(strcmp(dataDiscripton,"")==0)
				{zlg_debug("%d\r\n",*((char*)data));}
			else
				{zlg_debug("%s = %d\r\n",dataDiscripton,*((char*)data));}
			break;
		case 2://short
			*pos = Write_secs_ShortData(*pos, *((short*)data));
			if(strcmp(dataDiscripton,"")==0)
				{zlg_debug("%d\r\n",*((short*)data));}
			else
				{zlg_debug("%s = %d\r\n",dataDiscripton,*((short*)data));}
			break;
		case 3://int
			*pos = Write_secs_IntData(*pos, *((int*)data));
			if(strcmp(dataDiscripton,"")==0)
				{zlg_debug("%d\r\n",*((int*)data));}
			else
				{zlg_debug("%s = %d\r\n",dataDiscripton,*((int*)data));}
			break;
		case 4://float
			*pos = Write_secs_FloatData(*pos, *((float*)data));
			if(strcmp(dataDiscripton,"")==0)
				{zlg_debug("%.3f\r\n",*((float*)data));}
			else
				{zlg_debug("%s = %.3f\r\n",dataDiscripton,*((float*)data));}
			break;
		case 5://string
			*pos = Write_secs_StringData(*pos, (char*)data);
			if(strcmp(dataDiscripton,"")==0)
				{zlg_debug("%s\r\n",(char*)data);}
			else
				{zlg_debug("%s = %s\r\n",dataDiscripton,(char*)data);}
			break;
		default:
			break;
	}
}

extern void app_interface_binary_data(int* pos,MQData* data);
extern int adcValue[2];
int GenerateSecsBinaryData(MQData* data)
{		
 	int pos = 0;
	char buf[20] = {0};
	app_interface_binary_data(&pos,data);
	//pos = Write_secs_StringData(pos,bsp_GetDistMac());
	sprintf(buf,"%d",adcValue[1]);
	pos = Write_secs_StringData(pos,buf);
	pos = Write_secs_StringData(pos,Program_Version);
	//printf("%s\r\n",Program_Version);
	
	return pos;
}
_IOT_SecsBinaryCreate IOT_SecsBinaryCreate = 
{
	GenerateSecsBinaryData
};
