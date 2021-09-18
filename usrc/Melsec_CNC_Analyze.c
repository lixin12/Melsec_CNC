#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <sys/poll.h> 
#include "type.h"
#include "log.h"
#include "Melsec_CNC.h"
static char result[MQTT_MSG_MAX_LEN-40] = {0};
static char err_msg[100] = {0};
typedef struct{
    int m_nCncStatus;
    int m_ProgramStatus;
    int m_nAlarmNum;
	char m_sSpeed[100];
	char m_sFeedrate[100];
	char m_sFCmd[100];
	char m_sSpindleOverride[100];
	char m_sdFeedOverride[100];
	char m_sCurProgramBlockNo[100];
	char m_snToolNum[100];
	char m_sdLoad[100];
	char m_sdTemp[100];
	char m_sdLoad_x[100];
	char m_sdLoad_y[100];
	char m_sdLoad_z[100];
	char m_sdRelPos_x[100];
	char m_sdRelPos_y[100];
	char m_sdRelPos_z[100];
	char m_sdAbsPos_x[100];
	char m_sdAbsPos_y[100];
	char m_sdAbsPos_z[100];
	char m_sdMacPos_x[100];
	char m_sdMacPos_y[100];
	char m_sdMacPos_z[100];
	char m_sdRemPos_x[100];
	char m_sdRemPos_y[100];
	char m_sdRemPos_z[100];
	char m_snCycleTime[100];
	char m_snFinishPartsNum[100];
	char m_snTotalMachiningTime[100];
	char m_snTotalPowerOnTime[100];
	char m_snCncStatus[100];
	char m_sProgramStatus[100];
	char m_sCncMode[100];
    char m_sAxisNamex[100];
    char m_sAxisNamey[100];
    char m_sAxisNamez[100];
    char m_sCurProgramNo[100];
    char m_sAlarmType[100];
    char m_sAlarmCode[100];
    char m_sAlarmMsg[100];
	char m_sAlarmNumber[100];
}melsec_cnc_data;

melsec_cnc_data melsec_cnc_data_t;

char ABC_Digit[62]={"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};

char* Lx_itoaEx(int value, char* str, int radix) {
    static char dig[] ="0123456789abcdefghijklmnopqrstuvwxyz";
    int n = 0, neg = 0;
    unsigned int v;
    char* p, *q;
    char c;
    if (radix == 10 && value < 0) {
        value = -value;
        neg = 1;
    }
    v = value;
    do {
        str[n++] = dig[v%radix];
        v /= radix;
    } while (v);
    if (neg)
        str[n++] = '-';
    str[n] = '\0';
    for (p = str, q = p + (n-1); p < q; ++p, --q)
        c = *p, *p = *q, *q = c;
    return str;
}

unsigned char Inquire_letter(unsigned char letter)
{
	if(letter>=48&&letter<=57)
		return 1;
	else if(letter>=65&&letter<=90)
		return 2;
	else if(letter>=97&&letter<=120)
		return 3;
	else 
		return 0;
}
char ASCII(unsigned char letter)
{
	char ABC;
	switch (Inquire_letter(letter))
	{
		case 1:
			ABC = ABC_Digit[letter-0x30];
			break;
		case 2:
			ABC = ABC_Digit[letter-0x37];
			break;
		case 3:
			ABC = ABC_Digit[letter-0x3D];
		default:
			return 0;
	}
	return ABC;
}

void resolve_lotname(unsigned char * HEXLotName,char * Lotname)
{
		char * p = Lotname;
        unsigned char ii = 0;
		for(ii = 0;ii<8;ii++)
		{
			if(ASCII(HEXLotName[ii])!=0)
			{
				*p=ASCII(HEXLotName[ii]);
				p++;
			}		
		}
}

char *lotnameRevData(char *hexlotname,unsigned char * data,unsigned char len)
{
	unsigned char hexname[8];
	memcpy(hexname,data,len);
	resolve_lotname(hexname,hexlotname);
	return hexlotname;
}

//将Hex字符转换成无符号整型
unsigned int hex2int(char c)
{
    if( c >= '0' && c <= '9')
    {
        return (unsigned int) (c -48);
    }
    else if( c >= 'A' && c <= 'F')
    {
        return (unsigned int) (c -55);
    }
    else if( c >= 'a' && c <= 'f')
    {
        return (unsigned int) (c - 87);
    }
    else
    {
        return 0;
    }
}
unsigned char* getRevDataCnt(unsigned char* data,unsigned int cnt)
{
	unsigned short ucount;
	ucount = (unsigned short)((hex2int(data[0])<<12) | (hex2int(data[1])<<8) | (hex2int(data[2])<<4) | (hex2int(data[3])<<0));
	data++;
	data++;
	data++;
	data++;
	printf("%s\r\n",data);
	if(ucount+18 == cnt)
	{
		return data;
	}
	else
	{
		printf("ucount:%d  cnt:%d \r\n",ucount,cnt);
		return 0;
	}
}

unsigned char* isOverRevData(unsigned char* data)
{
	unsigned char ret;
	if(*data++ == '0' && *data++ == '0' && *data++ == '0' && *data++ == '0')
		ret = 1;
	if (ret == 1) 
	return data;
	else
	return 0;
}


unsigned char * AnalyzeHalfwordtatusData(unsigned char* data,unsigned short *data16)
{
	
	*data16=(unsigned short)((hex2int(data[0])<<12) | (hex2int(data[1])<<8) | (hex2int(data[2])<<4) | (hex2int(data[3])<<0));
	data=data+4;
	return data;
}
unsigned char * AnalyzeRevData(unsigned char* data,unsigned int *data32)
{
	unsigned short tp1 = 0;
	unsigned short tp2 = 0;
	data = AnalyzeHalfwordtatusData(data,&tp1);
	data = AnalyzeHalfwordtatusData(data,&tp2);
	*data32=((unsigned int)tp1<<16 | (unsigned int)tp2);
	return data;
}



void pack_analyze_result(void)
{
	strcat(result,melsec_cnc_data_t.m_sAlarmNumber);
	strcat(result,melsec_cnc_data_t.m_sAlarmCode);
	strcat(result,melsec_cnc_data_t.m_sAlarmMsg);
	strcat(result,melsec_cnc_data_t.m_sAlarmType);
	strcat(result,melsec_cnc_data_t.m_sCncMode);
	strcat(result,melsec_cnc_data_t.m_snCncStatus);
	strcat(result,melsec_cnc_data_t.m_sCurProgramBlockNo);
	strcat(result,melsec_cnc_data_t.m_sCurProgramNo);
	strcat(result,melsec_cnc_data_t.m_snCycleTime);
	strcat(result,melsec_cnc_data_t.m_sdFeedOverride);
	strcat(result,melsec_cnc_data_t.m_sFeedrate);
	strcat(result,melsec_cnc_data_t.m_snFinishPartsNum);
	strcat(result,melsec_cnc_data_t.m_sProgramStatus);
	strcat(result,melsec_cnc_data_t.m_sdAbsPos_x);
	strcat(result,melsec_cnc_data_t.m_sdLoad_x);
	strcat(result,melsec_cnc_data_t.m_sdMacPos_x);
	strcat(result,melsec_cnc_data_t.m_sAxisNamex);
	strcat(result,melsec_cnc_data_t.m_sdRelPos_x);
	strcat(result,melsec_cnc_data_t.m_sdRemPos_x);
	strcat(result,melsec_cnc_data_t.m_sdAbsPos_y);
	strcat(result,melsec_cnc_data_t.m_sdLoad_y);
	strcat(result,melsec_cnc_data_t.m_sdMacPos_y);
	strcat(result,melsec_cnc_data_t.m_sAxisNamey);
	strcat(result,melsec_cnc_data_t.m_sdRelPos_y);
	strcat(result,melsec_cnc_data_t.m_sdRemPos_y);
	strcat(result,melsec_cnc_data_t.m_sdAbsPos_z);
	strcat(result,melsec_cnc_data_t.m_sdLoad_z);
	strcat(result,melsec_cnc_data_t.m_sdMacPos_z);
	strcat(result,melsec_cnc_data_t.m_sAxisNamez);
	strcat(result,melsec_cnc_data_t.m_sdRelPos_z);
	strcat(result,melsec_cnc_data_t.m_sdRemPos_z);
}

char *deal_with_alarm(const char *m_strtype, const char *m_strcode)
{
	int ret = 0;
	if((strcmp(m_strtype,"M01 "))==0)
	{
		{
			if(strcmp(m_strcode,"0105")==0)  
				sprintf(err_msg,"%s","主轴停止");
		}
	}
	if((strcmp(m_strtype,"Z40 "))==0)
	{
		sprintf(err_msg,"%s","格式不一致");
	}
	if((strcmp(m_strtype,"Y51 "))==0)
	{
		{
			if(strcmp(m_strcode,"0002")==0)  
				sprintf(err_msg,"%s","参数G1tL加减速时间常数异常-2007");
		}
	}
	if((strcmp(m_strtype,"Y06 "))==0)
		sprintf(err_msg,"%s","mcp_no 设定错误");
	return err_msg;
}

int melsec_cnc_analyze_data(unsigned char* data)
{
	union{
		unsigned char b[4];
		long v;
	}ID,ID1,ld;
	union{
   		unsigned char b[8];
   		double v;
   	}d;
	union{
    	unsigned char b[2];
    	int v;
 	}iv;
	char m_strValue[20];
	double d_value=0;
	long   l_value=0;
	int    i_value=0;
	int    i=0;
	char *alarm_msg = NULL;
	if((data[0]==0x47)&&(data[1]==0x49)&&(data[2]==0x4f)&&(data[3]==0x50)&&(data[4]==0x01)&&(data[5]==0x00)&&(data[6]==0x01)&&(data[7]==0x01))
	{
		ID.b[0] = data[16];
		ID.b[1] = data[17];
		ID.b[2] = data[18];
		ID.b[3] = data[19];
		iv.v=0;
		l_value=0;
		d_value=0;
		if((data[28]==0x49)&&(data[29]==0x44)&&(data[30]==0x4c))   //IDL
			sprintf(m_strValue,"%s","IDL");
		else if((data[28]==0x01)&&(data[32]==0x01))    //BYTE		 
			sprintf(m_strValue,"%d",data[36]);		   
		else if((data[28]==0x02)&&(data[32]==0x02))    //INT
		{
			iv.v=0;
			iv.b[0]=data[36];
			iv.b[1]=data[37];
			i_value=iv.v;
			sprintf(m_strValue,"%ld",i_value);
		}
		else if((data[28]==0x03)&&(data[32]==0x04))  //long
		{
			ID1.v = 0;
			ID1.b[0]=data[36];
			ID1.b[1]=data[37];
			ID1.b[2]=data[38];
			ID1.b[3]=data[39];
			l_value=ID1.v;
			sprintf(m_strValue,"%ld",l_value);
		}
		else if((data[28]==0x05)&&(data[32]==0x08))  //dobule-8
		{
			d.v = 0;
			d.b[0]=data[36];
			d.b[1]=data[37];
			d.b[2]=data[38];
			d.b[3]=data[39];
			d.b[4]=data[40];
			d.b[5]=data[41];
			d.b[6]=data[42];
			d.b[7]=data[43];
			d_value=d.v;
			sprintf(m_strValue,"%lf",d_value);
		} 
		else if((data[28]==0x06)&&(data[32]==0x10))  //double-10
		{
			d.v = 0;
			d.b[0]=data[44];
			d.b[1]=data[45];
			d.b[2]=data[46];
			d.b[3]=data[47];
			d.b[4]=data[48];
			d.b[5]=data[49];
			d.b[6]=data[50];
			d.b[7]=data[51];
			d_value=d.v;
			sprintf(m_strValue,"%lf",d_value);
		}
		else if((data[28]==0x10))  //CString
		{
			for(i=0; i<data[36]; i++)
			{
				m_strValue[i] = data[40+i];
			}
			m_strValue[i] = 0;
		}
		else if((data[28]==0xbc))  //CString-188  
		{
			for(i=0; i<data[36]; i++)
			{
				m_strValue[i] = data[40+i];
			}
			m_strValue[i] = 0;
		}
		switch(ID.v)
		{
		case ID_RDY_HLD:            
			{
				melsec_cnc_data_t.m_nCncStatus =2; 
				melsec_cnc_data_t.m_ProgramStatus =2; 
				zlg_debug("CncStatus = %s\r\n", m_strValue);
				if(strcmp(m_strValue,"AUT") ==0)
				{
			       	melsec_cnc_data_t.m_nCncStatus =1; 
					melsec_cnc_data_t.m_ProgramStatus =1; 
					sprintf(melsec_cnc_data_t.m_snCncStatus,"%d;",melsec_cnc_data_t.m_nCncStatus);
					sprintf(melsec_cnc_data_t.m_sProgramStatus,"%d;",melsec_cnc_data_t.m_ProgramStatus);
				}
				else if (strcmp(m_strValue,"HLD") ==0)
				{
					melsec_cnc_data_t.m_nCncStatus =2; 
					melsec_cnc_data_t.m_ProgramStatus =3; 
					sprintf(melsec_cnc_data_t.m_snCncStatus,"%d;",melsec_cnc_data_t.m_nCncStatus);
					sprintf(melsec_cnc_data_t.m_sProgramStatus,"%d;",melsec_cnc_data_t.m_ProgramStatus);
				}	
				else
				{
					sprintf(melsec_cnc_data_t.m_snCncStatus,"%d;",melsec_cnc_data_t.m_nCncStatus);
					sprintf(melsec_cnc_data_t.m_sProgramStatus,"%d;",melsec_cnc_data_t.m_ProgramStatus);
				}
				zlg_debug("m_snCncStatus = %s\r\n",melsec_cnc_data_t.m_snCncStatus);
				zlg_debug("m_sProgramStatus = %s\r\n",melsec_cnc_data_t.m_sProgramStatus);		
				break;
			}
		case ID_PROC_MODE:           
			{
				sprintf(melsec_cnc_data_t.m_sCncMode,"%s;",m_strValue);
				zlg_debug("m_sCncMode = %s\r\n",melsec_cnc_data_t.m_sCncMode);
				break;
			}
		case ID_POWER: 
			{
				sprintf(melsec_cnc_data_t.m_snTotalPowerOnTime,"%ld;",l_value);
				zlg_debug("m_snTotalPowerOnTime = %s\r\n",melsec_cnc_data_t.m_snTotalPowerOnTime);
				break;
			}
		case ID_AUTO:  
			{
				sprintf(melsec_cnc_data_t.m_snTotalMachiningTime,"%ld;",l_value);
				zlg_debug("m_snTotalMachiningTime = %s\r\n",melsec_cnc_data_t.m_snTotalMachiningTime);
				break;
			}
		case ID_PROCESS:
			{
				sprintf(melsec_cnc_data_t.m_snCycleTime,"%ld;",l_value);
				zlg_debug("m_snCycleTime = %s\r\n",melsec_cnc_data_t.m_snCycleTime);
				break;
			}  	
		case ID_SERVO_NAME1:   
			{
				sprintf(melsec_cnc_data_t.m_sAxisNamex,"%s;",m_strValue);
				zlg_debug("m_sAxisNamex = %s\r\n",melsec_cnc_data_t.m_sAxisNamex);
				break;
			}  	
		case ID_SERVO_NAME2 :  
			{
				sprintf(melsec_cnc_data_t.m_sAxisNamey,"%s;",m_strValue);
				zlg_debug("m_sAxisNamey = %s\r\n",melsec_cnc_data_t.m_sAxisNamey);
			  	break;
			}  
		case ID_SERVO_NAME3 :  
			{
				sprintf(melsec_cnc_data_t.m_sAxisNamez,"%s;",m_strValue);
				zlg_debug("m_sAxisNamez = %s\r\n",melsec_cnc_data_t.m_sAxisNamez);
				break;
			}
		case ID_POS_CURRENT_X:  
			{
				sprintf(melsec_cnc_data_t.m_sdRelPos_x,"%lf;",d_value);
				zlg_debug("m_sdRelPos_x = %s\r\n",melsec_cnc_data_t.m_sdRelPos_x);
				break;
			}
		case ID_POS_WORK_X:      
			{
				sprintf(melsec_cnc_data_t.m_sdAbsPos_x,"%lf;",d_value);
				zlg_debug("m_sdAbsPos_x = %s\r\n",melsec_cnc_data_t.m_sdAbsPos_x);
				break;
			}
		case ID_POS_MACHINE_X:  
			{
				sprintf(melsec_cnc_data_t.m_sdMacPos_x,"%lf;",d_value); 
				zlg_debug("m_sdMacPos_x = %s\r\n",melsec_cnc_data_t.m_sdMacPos_x);
				break;
			}
		case ID_POS_DIS_X:     
			{
				sprintf(melsec_cnc_data_t.m_sdRemPos_x,"%lf;",d_value);
				zlg_debug("m_sdRemPos_x = %s\r\n",melsec_cnc_data_t.m_sdRemPos_x);
				break;
			}
		case ID_SERVO_LOADX:    
			{
				sprintf(melsec_cnc_data_t.m_sdLoad_x,"%ld;",l_value);
				zlg_debug("m_sdLoad_x = %s\r\n",melsec_cnc_data_t.m_sdLoad_x);
				break;
			}  
		case ID_POS_CURRENT_Y:   
			{
				sprintf(melsec_cnc_data_t.m_sdRelPos_y,"%lf;",d_value);
				zlg_debug("m_sdRelPos_y = %s\r\n",melsec_cnc_data_t.m_sdRelPos_y);
				break;
			}
		case ID_POS_WORK_Y:         
			{
				sprintf(melsec_cnc_data_t.m_sdAbsPos_y,"%lf;",d_value);
				zlg_debug("m_sdAbsPos_y = %s\r\n",melsec_cnc_data_t.m_sdAbsPos_y);
				break;
			}
		case ID_POS_MACHINE_Y:  
			{
				sprintf(melsec_cnc_data_t.m_sdMacPos_y,"%lf;",d_value);
				zlg_debug("m_sdMacPos_y = %s\r\n",melsec_cnc_data_t.m_sdMacPos_y);
				break;
			}
		case ID_POS_DIS_Y:       
			{
				sprintf(melsec_cnc_data_t.m_sdRemPos_y,"%lf;",d_value);
				zlg_debug("m_sdRemPos_y = %s\r\n",melsec_cnc_data_t.m_sdRemPos_y);
				break;
			}
		case ID_SERVO_LOADY:      
			{
				sprintf(melsec_cnc_data_t.m_sdLoad_y,"%ld;",l_value);
				zlg_debug("m_sdLoad_y = %s\r\n",melsec_cnc_data_t.m_sdLoad_y);
				break;
			}
		case ID_POS_CURRENT_Z:   
			{
				sprintf(melsec_cnc_data_t.m_sdRelPos_z,"%lf;",d_value);
				zlg_debug("m_sdRelPos_z = %s\r\n",melsec_cnc_data_t.m_sdRelPos_z);
				break;
			}
		case ID_POS_WORK_Z:          
			{
				sprintf(melsec_cnc_data_t.m_sdAbsPos_z,"%lf;",d_value);
				zlg_debug("m_sdAbsPos_z = %s\r\n",melsec_cnc_data_t.m_sdAbsPos_z);
				break;
			}
		case ID_POS_MACHINE_Z:  
			{
				sprintf(melsec_cnc_data_t.m_sdMacPos_z,"%lf;",d_value);
				zlg_debug("m_sdMacPos_z = %s\r\n",melsec_cnc_data_t.m_sdMacPos_z);
				break;
			}
		case ID_POS_DIS_Z:        
			{
				sprintf(melsec_cnc_data_t.m_sdRemPos_z,"%lf;",d_value);
				zlg_debug("m_sdRemPos_z = %s\r\n",melsec_cnc_data_t.m_sdRemPos_z);
				break;
			}
		case ID_SERVO_LOADZ:        
			{
				sprintf(melsec_cnc_data_t.m_sdLoad_z,"%ld;",l_value);
				zlg_debug("m_sdLoad_z = %s\r\n",melsec_cnc_data_t.m_sdLoad_z);
				break;
			} 
		case ID_SPENDLE_TEMP :       
			{
				sprintf(melsec_cnc_data_t.m_sdTemp,"%ld;",l_value);
				zlg_debug("m_sdTemp = %s\r\n",melsec_cnc_data_t.m_sdTemp);
				break;
			}
		case ID_SPENDLE_LOAD :       
			{
				sprintf(melsec_cnc_data_t.m_sdLoad,"%ld;",l_value);
				zlg_debug("m_sdLoad = %s\r\n",melsec_cnc_data_t.m_sdLoad);
				break;
			}
		case ID_SPENDLE_SPEED:        
			{
				sprintf(melsec_cnc_data_t.m_sSpeed,"%lf;",d_value);
				zlg_debug("m_sSpeed = %s\r\n",melsec_cnc_data_t.m_sSpeed);
				break;
			}
		case ID8002N:         
			{
				sprintf(melsec_cnc_data_t.m_snFinishPartsNum,"%s;",m_strValue);
				zlg_debug("m_snFinishPartsNum = %s\r\n",melsec_cnc_data_t.m_snFinishPartsNum);
				break;
			}
		case ID_TOOL_NUM:     
			{
				sprintf(melsec_cnc_data_t.m_snToolNum,"%ld;",l_value);
				zlg_debug("m_snToolNum = %s\r\n",melsec_cnc_data_t.m_snToolNum);
				break;
			}  
		case ID_FILE_NAME:  
			{
				sprintf(melsec_cnc_data_t.m_sCurProgramNo,"%s;",m_strValue);
				zlg_debug("m_sCurProgramNo = %s\r\n",melsec_cnc_data_t.m_sCurProgramNo);
				break;
			}
		case ID_BLOCK_NUM:  
			{
				sprintf(melsec_cnc_data_t.m_sCurProgramBlockNo,"%ld;",l_value);
				zlg_debug("m_sCurProgramBlockNo = %s\r\n",melsec_cnc_data_t.m_sCurProgramBlockNo);
				break;
			}  
		case ID_INPUT_FEEDRATE: 
			{
				sprintf(melsec_cnc_data_t.m_sdFeedOverride,"%s;",m_strValue);
				zlg_debug("m_sdFeedOverride = %s\r\n",melsec_cnc_data_t.m_sdFeedOverride);
				break;
			} 
		case ID_INPUT_SPENDLERATE:
			{
				sprintf(melsec_cnc_data_t.m_sSpindleOverride,"%s;",m_strValue);
				zlg_debug("m_sSpindleOverride = %s\r\n",melsec_cnc_data_t.m_sSpindleOverride);
				break;
			}         
		case ID_FEED_SET: 
			{
				sprintf(melsec_cnc_data_t.m_sFCmd,"%s;",m_strValue);
				zlg_debug("m_sFCmd = %s\r\n",melsec_cnc_data_t.m_sFCmd);
				break;
			}    
		case ID_FEED_ACT:  
			{
				sprintf(melsec_cnc_data_t.m_sFeedrate,"%s;",m_strValue);
				zlg_debug("m_sFeedrate = %s\r\n",melsec_cnc_data_t.m_sFeedrate);
				break;
			}         
		case ID_ALARM_MESSAGE:
			{
				int k=0;
				int mm=0;				
				if(data[8]==0x34)  
				{
					melsec_cnc_data_t.m_nAlarmNum = 0;
				}
				else 
				{
					if(data[8]>44)
					{
						ld.v=0;
						ld.b[0]=data[28];
						ld.b[1]=data[29];
						ld.b[2]=data[30];
						ld.b[3]=data[31]; 					
						melsec_cnc_data_t.m_nAlarmNum = (int)(ld.v/59);
						sprintf(melsec_cnc_data_t.m_sAlarmNumber,"%d;",melsec_cnc_data_t.m_nAlarmNum);	 
						for(mm=0;mm< melsec_cnc_data_t.m_nAlarmNum;mm++)
						{	
							for(k=0;k<4;k++)
								melsec_cnc_data_t.m_sAlarmType[mm*4+k] = data[35+k+mm*59];
							melsec_cnc_data_t.m_sAlarmType[mm*4+k] = 0x00;
							for(k=0;k<4;k++)	
								melsec_cnc_data_t.m_sAlarmCode[mm*4+k] = data[35+30+k+mm*59];
							melsec_cnc_data_t.m_sAlarmCode[mm*4+k] = 0x00;			   
							alarm_msg = deal_with_alarm((const char *)melsec_cnc_data_t.m_sAlarmType,(const char *)melsec_cnc_data_t.m_sAlarmCode);
							sprintf(melsec_cnc_data_t.m_sAlarmMsg,"%s;",alarm_msg);
						}
						melsec_cnc_data_t.m_sAlarmType[mm*4-1] = 0x3B;
						melsec_cnc_data_t.m_sAlarmCode[mm*4-1] = 0x3B;
					}	
					zlg_debug("AlarmNum = %s\r\n",melsec_cnc_data_t.m_sAlarmNumber);
					zlg_debug("m_sAlarmType = %s\r\n",melsec_cnc_data_t.m_sAlarmType);
					zlg_debug("m_sAlarmCode = %s\r\n",melsec_cnc_data_t.m_sAlarmCode);
					zlg_debug("m_sAlarmMsg = %s\r\n",melsec_cnc_data_t.m_sAlarmMsg);
				}
				break; 
			}
			default:
			{
				return -1;
			}		
		}                                         	
	}
	return 0;
}

char* SI_CNC_GetResult(unsigned char i)
{
	return result;
}
void SI_CNC_ClearResult(void)
{
	memset(result,0,sizeof(result));
}
int SI_CNC_GetResultCnt(void)
{
	return 0;
}

