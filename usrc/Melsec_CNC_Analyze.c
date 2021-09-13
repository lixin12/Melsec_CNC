#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type.h"
#include "log.h"
#include "Siemens_CNC.h"
static char result[MQTT_MSG_MAX_LEN-40] = {0};

Si_CNC_Data Si_CNC_data;

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

char Si_CNC_Analyze_Ver(unsigned char* data)
{
	int k = 0;
	int len = 0;
	int point = 0;
	if(data[12] == 0x0f)
	{
		len = 0;
		point = 20;
		if ((data[point + 1] == 0xff) && (data[point + 2] == 0x09))
		{
			len = data[point + 4];
			for (k = 0; k < len; k++)
			{
				if (data[k + point + 5] == 0x00) break;
				Si_CNC_data.m_strVer1[k] = data[k + point + 5];
			}
			Si_CNC_data.m_strVer1[k]= 0x3B;
			Si_CNC_data.m_strVer1[k+1]= 0x00;
			zlg_debug("m_strVer1=%s\r\n",Si_CNC_data.m_strVer1);
		}
		point += 4;
		point += len;
		if ((data[point + 1] == 0xff) && (data[point + 2] == 0x09))
		{
			len = data[point + 4];
			for (k = 0; k < len; k++)
			{
				if (data[k + point + 5] == 0x00) break;
				Si_CNC_data.m_strVer2[k]= data[k + point + 5];
			}
			Si_CNC_data.m_strVer2[k]= 0x3B;
			Si_CNC_data.m_strVer2[k+1] = 0x00;
			zlg_debug("m_strVer2=%s\r\n",Si_CNC_data.m_strVer2);
		}
		point += 4;
		point += len;
		if ((data[point + 1] == 0xff) && (data[point + 2] == 0x09))
		{
			len = data[point + 4];
			for (k = 0; k < len; k++)
			{
				if (data[k + point + 5] == 0x00) break;
				Si_CNC_data.m_strVer3[k]= data[k + point + 5];
			}
			Si_CNC_data.m_strVer3[k]= 0x3B;
			Si_CNC_data.m_strVer3[k+1] = 0x00;
			zlg_debug("m_strVer3=%s\r\n",Si_CNC_data.m_strVer3);
		}
		point += 4;
		point += len;
		if ((data[point + 1] == 0xff) && (data[point + 2] == 0x09))
		{
			len = data[point + 4];
			for (k = 0; k < len; k++)
			{
				if (data[k + point + 5] == 0x00) break;
				Si_CNC_data.m_strVer4[k]= data[k + point + 5];
			}
			Si_CNC_data.m_strVer4[k]= 0x3B;
			Si_CNC_data.m_strVer4[k+1] = 0x00;
			zlg_debug("m_strVer4=%s\r\n",Si_CNC_data.m_strVer4);
		}
		point += 4;
		point += len;
		if ((data[point + 1] == 0xff) && (data[point + 2] == 0x09))
		{
			len = data[point + 4];
			for (k = 0; k < len; k++)
			{
				if ((data[k + point + 5] == 0x74) || (data[k + point + 5] == 0x54))
				{
					Si_CNC_data.m_MachineMode = 0;	  //T
				}
				else
				{
					Si_CNC_data.m_MachineMode = 1;	  //M
				}
				if (data[k + point + 5] == 0x00) break;
				Si_CNC_data.m_strVer5[k]= data[k + point + 5];
			}
			//GetDlgItem(IDC_EDIT_VER3)->SetWindowText(m_strVer5);
			Si_CNC_data.m_strVer5[k]= 0x3B;
			Si_CNC_data.m_strVer5[k+1]= 0x00;
			zlg_debug("m_strVer5=%s\r\n",Si_CNC_data.m_strVer5);
		}
		return 0;
	}
	else return 1;
}

char Si_CNC_Analyze_Axis_Name(unsigned char* data)
{
	int k = 0;
	if ((data[3] == 0x59) && (data[12] == 0x30))
	{
		if (data[25] != 0x00)       //NAME1
		{
			for (k = 0; k<5; k++)
			{
				if (data[k + 25] != 0x00)
				{
					Si_CNC_data.m_sAxisName1[k] = data[k + 25];
				}
				else break;
			}
			Si_CNC_data.m_sAxisName1[k] = 0x3B;
			Si_CNC_data.m_sAxisName1[k+1] = 0x00;
			zlg_debug("m_sAxisName1=%s\r\n",Si_CNC_data.m_sAxisName1);			
		}
		if (data[41] != 0x00)      //NAME2
		{
			for (k = 0; k<5; k++)
			{
				if (data[k + 41] != 0x00)
				{
					Si_CNC_data.m_sAxisName2[k] = data[k + 41];
				}
				else break;
			}
			Si_CNC_data.m_sAxisName2[k] = 0x3B;
			Si_CNC_data.m_sAxisName2[k+1] = 0x00;
			zlg_debug("m_sAxisName2=%s\r\n",Si_CNC_data.m_sAxisName2);	
		}
		if (data[57] != 0x00)      //NAME3
		{
			for (k = 0; k<5; k++)
			{
				if (data[k + 57] != 0x00)
				{
					Si_CNC_data.m_sAxisName3[k] = data[k + 57];
				}
				else break;
			}
			Si_CNC_data.m_sAxisName3[k] = 0x3B;
			Si_CNC_data.m_sAxisName3[k+1] = 0x00;
			zlg_debug("m_sAxisName3=%s\r\n",Si_CNC_data.m_sAxisName3);			
		}
		/*	else if((data[57]!=0x00)&&(data[58]!=0x00))   //SP
		{
		//	spindles_info[0].Name[0]=data[57];
		//spindles_info[0].Name[1]=data[58];
		}
		if((data[73]!=0x00)&&(data[74]==0x00))       //
		{
		}
		else if((data[73]!=0x00)&&(data[74]!=0x00))   //?锟斤拷?锟斤拷??3?
		{
		spindles_info[0].Name[0]=data[73];
		spindles_info[0].Name[1]=data[74];
		}
		*/
		return 0;
	}
	else return 1;
}

char Si_CNC_Analyze_Oprate(unsigned char* data)
{
	if ((data[3] == 0x21) && (data[12] == 0x0b))
	{
		if (data[24] == 0x02)
		{
			if (data[31] == 0x00)
			{
				Si_CNC_data.m_OpMode = data[25];    //0  JOG  1  mda  2 auto  3 ref
				if (Si_CNC_data.m_OpMode == 0)
					sprintf(Si_CNC_data.m_strOprate,"%s;","0");
				if (Si_CNC_data.m_OpMode == 1)
					sprintf(Si_CNC_data.m_strOprate,"%s;","1");
				if (Si_CNC_data.m_OpMode == 2)
					sprintf(Si_CNC_data.m_strOprate,"%s;","2");
			}
			else if (data[31] == 0x03)
			{
				Si_CNC_data.m_OpMode = 3;   //REF
				sprintf(Si_CNC_data.m_strOprate,"%s;","3");
			}
		}
		zlg_debug("Si_CNC_data.m_strOprate:%s\r\n",Si_CNC_data.m_strOprate);
		return 0;
	}
	else return 1;
}

char Si_CNC_Analyze_CycStart(unsigned char* data)
{
	if ((data[3] == 0x21) && (data[12] == 0x0c))
	{
		if (data[24] == 0x02)
		{
			if (data[25] == 0x00)
			{
				Si_CNC_data.m_iState = 0;
				sprintf(Si_CNC_data.m_strState,"%s;","0");//0  RES
			}  //RES
			if ((data[25] == 0x02) && ((data[31] == 0x02) || (data[31] == 0x01)|| (data[31] == 0x05))) { Si_CNC_data.m_iState = 1;  sprintf(Si_CNC_data.m_strState,"%s;","1");} //HOLD 2 2  press hold key    2  1  alarm hold
			if ((data[25] == 0x01) && (data[31] == 0x03)) { Si_CNC_data.m_iState = 2;   sprintf(Si_CNC_data.m_strState,"%s;","2");} //CYC_START
			if ((data[25] == 0x01) && (data[31] == 0x05)) { Si_CNC_data.m_iState = 3;   sprintf(Si_CNC_data.m_strState,"%s;","3");} //CYC_STOP
			if (Si_CNC_data.m_nCycleNum == 2)
			{
				//get template status
				if (Si_CNC_data.m_iState == 0)
				{
					Si_CNC_data.m_nStatusCh2 = 2;
				}
				else if (Si_CNC_data.m_iState == 1)
				{
					Si_CNC_data.m_nStatusCh2 = 2;
				}
				else if (Si_CNC_data.m_iState == 2)
				{
					Si_CNC_data.m_nStatusCh2 = 1;
				}
				else if (Si_CNC_data.m_iState == 3)
				{
					Si_CNC_data.m_nStatusCh2 = 2;
				}
			}
			else
			{
				//get template status
				if (Si_CNC_data.m_iState == 0)
				{
					Si_CNC_data.m_nStatusCh1 = 2;
				}
				else if (Si_CNC_data.m_iState == 1)
				{
					Si_CNC_data.m_nStatusCh1 = 2;
				}
				else if (Si_CNC_data.m_iState == 2)
				{
					Si_CNC_data.m_nStatusCh1 = 1;
				}
				else if (Si_CNC_data.m_iState == 3)
				{
					Si_CNC_data.m_nStatusCh1 = 2;
				}
			}
			zlg_debug("m_iState = %d, m_strState = %s\r\n", Si_CNC_data.m_iState, Si_CNC_data.m_strState);
		}
		return 0;
	}
	else return 1;
}

char Si_CNC_Analyze_AUTOFeed(unsigned char* data)
{
	union {
		unsigned char b[8];
		double v;
	}d;
	int k = 0;
	if ((data[3] == 0x39) && (data[12] == 0x09))     //FEED_ACT  FEE_SET  FEED_RATE
	{
		if (data[24] == 0x08)
		{
			d.v = 0;
			for (k = 0; k < 8; k++)
				d.b[k] = data[25 + k];
			if (Si_CNC_data.m_OpMode != 0)//锟斤拷?锟斤拷??锟斤拷
			{
				Si_CNC_data.m_dFAct = d.v;
				sprintf(Si_CNC_data.m_strFeedAct,"%3.3f;",d.v);
				//GetDlgItem(IDC_EDIT_FEEDSET)->SetWindowText(m_strFeedAct);
				zlg_debug("m_strFeedAct = %s\r\n", Si_CNC_data.m_strFeedAct);
			}
		}
		if (data[36] == 0x08)
		{
			d.v = 0;
			for (k = 0; k < 8; k++)
				d.b[k] = data[37 + k];
			if (Si_CNC_data.m_OpMode != 0)// 
			{
				sprintf(Si_CNC_data.m_strFeedSet,"%3.3f;",d.v);
				Si_CNC_data.m_dFCmd = d.v;
				//GetDlgItem(IDC_EDIT_FEEDACT)->SetWindowText(m_strFeedSet);
				zlg_debug("m_strFeedSet = %s\r\n", Si_CNC_data.m_strFeedSet);
			}
		}
		if (data[48] == 0x08)
		{
			d.v = 0;
			for (k = 0; k < 8; k++)
				d.b[k] = data[49 + k];
			if (Si_CNC_data.m_OpMode != 0)
			{
				sprintf(Si_CNC_data.m_strFeedRate,"%3.3f;",d.v);
				Si_CNC_data.m_dFOvr = d.v;
				//GetDlgItem(IDC_EDIT_FEEDRATE)->SetWindowText(m_strFeedRate);
				zlg_debug("m_strFeedRate = %s\r\n", Si_CNC_data.m_strFeedRate);
			}
		}
		return 0;
	}
	else return 1;
}

char Si_CNC_Analyze_S_Data(unsigned char* data)
{
	int k = 0;
	union {
		unsigned char b[8];
		double v;
	}d;
	if ((data[3] == 0x39) && (data[12] == 0x0a))             //S_ACT  S_SET  S_RATE
	{
		if (data[24] == 0x08)
		{
			d.v = 0;
			for (k = 0; k < 8; k++)
				d.b[k] = data[25 + k];
			sprintf(Si_CNC_data.m_strSAct,"%3.3f;",d.v);
			Si_CNC_data.m_dSAct = d.v;
			zlg_debug("m_strSAct = %s\r\n", Si_CNC_data.m_strSAct);
		}
		if (data[36] == 0x08)
		{
			d.v = 0;
			for (k = 0; k < 8; k++)
				d.b[k] = data[37 + k];
			Si_CNC_data.m_dSCmd = d.v;
			sprintf(Si_CNC_data.m_strSSet,"%3.3f;",d.v);
			zlg_debug("m_strSSet = %s\r\n",Si_CNC_data.m_strSSet);
		}
		if (data[48] == 0x08)
		{
			d.v = 0;
			for (k = 0; k < 8; k++)
				d.b[k] = data[49 + k];
			Si_CNC_data.m_dSOvr = d.v;
			sprintf(Si_CNC_data.m_strSRate,"%3.3f;",d.v);
			zlg_debug("m_strSRate = %s\r\n", Si_CNC_data.m_strSRate);
		}
		return 0;
	}
	else return 1;
}


char Si_CNC_Analyze_once_axis(unsigned char* data)
{
	int i,k =0;
		union {
		unsigned char b[8];
		double v;
	}d;
	if ((data[3] == 0x75) && (data[12] == 0x0e))
	{
		i = 21;
		for (k = 0; k < 8; k++)
		{
			if ((data[i] == 0xff) && (data[i + 1] == 0x09))
			{
				d.v = 0;
				d.b[0] = data[i + 4];
				d.b[1] = data[i + 5];
				d.b[2] = data[i + 6];
				d.b[3] = data[i + 7];
				d.b[4] = data[i + 8];
				d.b[5] = data[i + 9];
				d.b[6] = data[i + 10];
				d.b[7] = data[i + 11];
				if (k == 0)
				{
					sprintf(Si_CNC_data.m_strMx1,"%lf;",d.v);
					////GetDlgItem(IDC_EDIT_MX1)->SetWindowText(m_strMx1);
					Si_CNC_data.m_dMacPos_x = d.v;
					zlg_debug("m_strMx1 = %s\r\n", Si_CNC_data.m_strMx1);
				}
				else if (k == 1)
				{
					if (Si_CNC_data.m_MachineMode == 0)//T
					{
						sprintf(Si_CNC_data.m_strMz1,"%lf;",d.v);
						Si_CNC_data.m_dMacPos_z = d.v;
						zlg_debug("m_strMz1 = %s\r\n", Si_CNC_data.m_strMz1);
					}
					else  //
					{
						sprintf(Si_CNC_data.m_strMy1,"%lf;",d.v);
						Si_CNC_data.m_dMacPos_y = d.v;
						zlg_debug("m_strMy1 = %s\r\n", Si_CNC_data.m_strMy1);
					}
				}
				else if (k == 2)
				{
					if (Si_CNC_data.m_MachineMode == 1)  //M
					{
						sprintf(Si_CNC_data.m_strMz1,"%lf;",d.v);
						Si_CNC_data.m_dMacPos_z = d.v;
						zlg_debug("m_strMz1 = %s\r\n", Si_CNC_data.m_strMz1);
					}
				}
				else if (k == 3)
				{
				}
				else if (k == 4)
				{
					sprintf(Si_CNC_data.m_strDisX,"%lf;",d.v);
					Si_CNC_data.m_dRemPos_x = d.v;
					zlg_debug("m_strDisX = %s\r\n", Si_CNC_data.m_strDisX);
				}
				else if (k == 5)
				{
					if (Si_CNC_data.m_MachineMode == 0)//T
					{
						sprintf(Si_CNC_data.m_strDisZ,"%lf;",d.v);
						Si_CNC_data.m_dRemPos_z = d.v;
						zlg_debug("m_strDisZ = %s\r\n", Si_CNC_data.m_strDisZ);
					}
					else  //
					{
						sprintf(Si_CNC_data.m_strDisY,"%lf;",d.v);
						Si_CNC_data.m_dRemPos_y = d.v;
						zlg_debug("m_strDisY = %s\r\n", Si_CNC_data.m_strDisY);
					}
				}
				else if (k == 6)
				{
					if (Si_CNC_data.m_MachineMode == 1)  //M
					{
						sprintf(Si_CNC_data.m_strDisZ,"%lf;",d.v);
						Si_CNC_data.m_dRemPos_z = d.v;
						zlg_debug("m_strDisZ = %s\r\n", Si_CNC_data.m_strDisZ);
					}
				}
				else if (k == 7)
				{
				}
			}
			i += 12;
		}//for
		return 0;
	}
	else return 1;
}

char Si_CNC_Analyze_Work_Axis(unsigned char* data)
{
	int i,k=0;
	union {
		unsigned char b[8];
		double v;
	}d;
	if ((data[3] == 0x45) && (data[12] == 0x14))
	{
		i = 21;
		for (k = 0; k < 4; k++)
		{
			if ((data[i] == 0xff) && (data[i + 1] == 0x09))
			{
				d.v = 0;
				d.b[0] = data[i + 4];
				d.b[1] = data[i + 5];
				d.b[2] = data[i + 6];
				d.b[3] = data[i + 7];
				d.b[4] = data[i + 8];
				d.b[5] = data[i + 9];
				d.b[6] = data[i + 10];
				d.b[7] = data[i + 11];
				if (k == 0)
				{
					sprintf(Si_CNC_data.m_strWorkX,"%lf;",d.v);
					Si_CNC_data.m_dAbsPos_x = d.v;
					zlg_debug("m_strWorkX = %s\r\n", Si_CNC_data.m_strWorkX);
				}
				else if (k == 1)
				{
					if (Si_CNC_data.m_MachineMode == 0)//T
					{
						sprintf(Si_CNC_data.m_strWorkZ,"%lf;",d.v);
						Si_CNC_data.m_dAbsPos_z = d.v;
						zlg_debug("m_strWorkZ = %s\r\n", Si_CNC_data.m_strWorkZ);
					}
					else  //
					{
						sprintf(Si_CNC_data.m_strWorkY,"%lf;",d.v);
						Si_CNC_data.m_dAbsPos_y = d.v;
						zlg_debug("m_strWorkY = %s\r\n", Si_CNC_data.m_strWorkY);
					}
				}
				else if (k == 2)
				{
					if (Si_CNC_data.m_MachineMode == 1)  //M
					{
						sprintf(Si_CNC_data.m_strWorkZ,"%lf;",d.v);
						Si_CNC_data.m_dAbsPos_z = d.v;
						zlg_debug("m_strWorkZ = %s\r\n", Si_CNC_data.m_strWorkZ);
					}
				}
				else if (k == 3)
				{
				}
			}
			i += 12;
		}//for
		return 0;
	}
	else return 1;
}

char Si_CNC_Analyze_ToolComp(unsigned char* data)
{
	union {
		unsigned char b[8];
		double v;
	}d;
	union {
		unsigned char b[4];
		long va;
	}l_int;
	int i,k,j=0;
	if (((data[3] == 0x4d)||(data[3] == 0x49)) && (data[12] == 0x15))
	{
		// if(data[12]==0x015) TOOLCOMP_seqence_number=data[24];
		//  	int i=35;
		i = 21;
		for (k = 0; k < 4; k++)
		{
			if ((data[i] == 0xff) && (data[i + 1] == 0x09))
			{
				d.v = 0.000000;
				d.b[0] = data[i + 4];
				d.b[1] = data[i + 5];
				d.b[2] = data[i + 6];
				d.b[3] = data[i + 7];
				d.b[4] = data[i + 8];
				d.b[5] = data[i + 9];
				d.b[6] = data[i + 10];
				d.b[7] = data[i + 11];
			}
			if (k == 0)
			{
				sprintf(Si_CNC_data.m_strToolLengthX,"%lf;",d.v);
				//GetDlgItem(IDC_EDIT_TOOLLENGTHX)->SetWindowText(m_strToolLengthX);
				zlg_debug("m_strToolLengthX = %s\r\n", Si_CNC_data.m_strToolLengthX);
			}
			else if (k == 1)
			{
				sprintf(Si_CNC_data.m_strToolLengthZ,"%lf;",d.v);
				//GetDlgItem(IDC_EDIT_TOOLLENGTHZ)->SetWindowText(m_strToolLengthZ);
				zlg_debug("m_strToolLengthZ = %lf\r\n",d.v);
				zlg_debug("m_strToolLengthZ = %s\r\n", Si_CNC_data.m_strToolLengthZ);
			}
			else if (k == 2)
			{
				sprintf(Si_CNC_data.m_strTooledg,"%lf;",d.v);
				//GetDlgItem(IDC_EDIT_TOOLEDG)->SetWindowText(m_strTooledg);
				zlg_debug("m_strTooledg = %s\r\n", Si_CNC_data.m_strTooledg);
			}
			else if (k == 3)
			{
				sprintf(Si_CNC_data.m_strToolRadiu,"%lf;",d.v);
				//GetDlgItem(IDC_EDIT_TOOLRADIU)->SetWindowText(m_strToolRadiu);
				zlg_debug("m_strToolRadiu = %s\r\n", Si_CNC_data.m_strToolRadiu);
			}
			i += 12;
		}//for
		if ((data[i] == 0xff) && (data[i + 1] == 0x09) && (data[i + 3] == 0x04))
		{
			l_int.va = 0;
			l_int.b[0] = data[i + 4];
			l_int.b[1] = data[i + 5];
			l_int.b[2] = data[i + 6];
			l_int.b[3] = data[i + 7];
			sprintf(Si_CNC_data.m_strHnum,"%ld;",l_int.va);
			////GetDlgItem(IDC_EDIT_HNUMBER)->SetWindowText(m_strHnum);
			zlg_debug("m_strHnum = %s\r\n", Si_CNC_data.m_strHnum);
		}
		return 0;
	}
	else return 1;
}

char Si_CNC_Analyze_TDnumber(unsigned char* data)
{
	union {
		unsigned char b[2];
		int v;
	}iv;
	if ((data[3] == 0x21) && (data[12] == 0x08))      //T  D
	{
		if (data[21] == 0xff)
		{
			iv.v = 0;
			iv.b[0] = data[25];
			iv.b[1] = data[26];
			sprintf(Si_CNC_data.m_strTno,"%d;",iv.v);
			////GetDlgItem(IDC_EDIT_TOOLNUMBER)->SetWindowText(m_strTno);
			zlg_debug("m_strTno = %s\r\n", Si_CNC_data.m_strTno);
			iv.v = 0;
			iv.b[0] = data[31];
			iv.b[1] = data[32];
			sprintf(Si_CNC_data.m_strDno,"%d;",iv.v);
			////GetDlgItem(IDC_EDIT_DNUMBER)->SetWindowText(m_strDno);
			zlg_debug("m_strDno = %s\r\n", Si_CNC_data.m_strDno);
		}
		return 0;
	}
	else return 1;
}

char Si_CNC_Analyze_JOG_FEEDRATE(unsigned char* data)
{
	int k=0;
	union {
		unsigned char b[8];
		double v;
	}d;
	if ((data[3] == 0x39) && ((data[12] == 0x09)||(data[12] == 0x07)))     //FEED_ACT  FEE_SET  FEED_RATE
	{
		if (data[24] == 0x08)
		{
			d.v = 0;
			for (k = 0; k < 8; k++)
				d.b[k] = data[25 + k];
			if (Si_CNC_data.m_OpMode != 0)//锟斤拷?锟斤拷??锟斤拷
			{
				Si_CNC_data.m_dFAct = d.v;
				sprintf(Si_CNC_data.m_strFeedAct,"%3.3f;", d.v);
				//GetDlgItem(IDC_EDIT_FEEDSET)->SetWindowText(m_strFeedAct);
				zlg_debug("m_strFeedAct = %s\r\n", Si_CNC_data.m_strFeedAct);
			}
		}
		if (data[36] == 0x08)
		{
			d.v = 0;
			for (k = 0; k < 8; k++)
				d.b[k] = data[37 + k];
			if (Si_CNC_data.m_OpMode != 0)// 
			{
				sprintf(Si_CNC_data.m_strFeedSet,"%3.3f", d.v);
				Si_CNC_data.m_dFCmd = d.v;
				//GetDlgItem(IDC_EDIT_FEEDACT)->SetWindowText(m_strFeedSet);
				zlg_debug("m_strFeedSet = %s\r\n", Si_CNC_data.m_strFeedSet);
			}
		}
		if (data[48] == 0x08)
		{
			d.v = 0;
			for (k = 0; k < 8; k++)
				d.b[k] = data[49 + k];
			if (Si_CNC_data.m_OpMode != 0)
			{
				sprintf(Si_CNC_data.m_strFeedRate,"%3.3f", d.v);
				Si_CNC_data.m_dFOvr = d.v;
				//GetDlgItem(IDC_EDIT_FEEDRATE)->SetWindowText(m_strFeedRate);
				zlg_debug("m_strFeedRate = %s\r\n", Si_CNC_data.m_dFOvr);
			}
		}
		return 0;
	}
	else return 1;
}

char Si_CNC_Analyze_ProcessName(unsigned char* data)
{
	int f = 0;
	int g = 0;
	int i,j= 0;
	if ((data[3] == 0xb9) && (data[12] == 0x12))
	{
		if ((data[21] == 0xff) && (data[22] == 0x09))
		{
			f = 0; 
			g = 0;
			i = 25; 
			memset(Si_CNC_data.m_strProcessName,0,sizeof(Si_CNC_data.m_strProcessName));
			/* while ((data[i] != 0x00) && (i < data[3]))
			{
				m_strProcessName += data[i];
				i++;
			} */
			while ((data[i] != 0x00) && (i < data[3]))
			{
				if (f > 0)
				{
					if (g == 2)
					{
						Si_CNC_data.m_strProcessName[j++]= data[i];
						Si_CNC_data.m_strProcessName[j]= 0x3B;
						Si_CNC_data.m_strProcessName[j+1]= 0x00;
					}
					if ((data[i] == 0x5F) && (g< 2))
					{
						g++;
					}
				}
				if (data[i] == 0x2F)
				{
					f++;
					//sprintf(Si_CNC_data.m_strProcessName,"%c","");
					//memset(Si_CNC_data.m_strProcessName,0,100);
					zlg_debug("Si_CNC_data.m_strProcessName %d =%s\r\n",i,Si_CNC_data.m_strProcessName);
					g = 0;
					j=0;
				}
				i++;
			}
			//sprintf(Si_CNC_data.m_sCurProgramNo,"%s",Si_CNC_data.m_strProcessName);
			//GetDlgItem(IDC_EDIT_PROCESSNAME)->SetWindowText(m_strProcessName);
			zlg_debug("m_strProcessName = %s\r\n", Si_CNC_data.m_strProcessName);
		}
		return 0;
	}
	else return 1;
}

char Si_CNC_Piece_Time(unsigned char* data)
{
	int i,k=0;
	union {
		unsigned char b[8];
		double v;
	}d;
	if ((data[3] == 0x45) && (data[12] == 0x11))
	{	
		i = 21;
		for (k = 0; k < 4; k++)
		{
			if ((data[i] == 0xff) && (data[i + 1] == 0x09))
			{
				float tempf;
				d.v = 0;
				d.b[0] = data[i + 4];
				d.b[1] = data[i + 5];
				d.b[2] = data[i + 6];
				d.b[3] = data[i + 7];
				d.b[4] = data[i + 8];
				d.b[5] = data[i + 9];
				d.b[6] = data[i + 10];
				d.b[7] = data[i + 11];
				if (k == 0)
				{
					sprintf(Si_CNC_data.m_strProAct,"%3.3lf;",d.v);
					Si_CNC_data.m_nCycleTime = d.v;
					sprintf(Si_CNC_data.m_ncycleTime,"%d;",Si_CNC_data.m_nCycleTime);
					tempf = d.v;
					zlg_debug("m_nCycleTime = %s\r\n", Si_CNC_data.m_ncycleTime);
					zlg_debug("m_strProAct = %s\r\n", Si_CNC_data.m_strProAct);
				}
				if (k == 1)
				{
					sprintf(Si_CNC_data.m_strProLeft,"%3.3lf;",(d.v - tempf));
					//GetDlgItem(IDC_EDIT_PRO_LEFT)->SetWindowText(m_strProLeft);
					zlg_debug("m_strProLeft = %s\r\n", Si_CNC_data.m_strProLeft);
				}
				if (k == 2)
				{
					sprintf(Si_CNC_data.m_strPicAct,"%3.0lf;", d.v);
					Si_CNC_data.m_nFinishPartsNum = d.v;
					//GetDlgItem(IDC_EDIT_PIC_ACT)->SetWindowText(m_strPicAct);
					zlg_debug("m_strPicAct = %s\r\n", Si_CNC_data.m_strPicAct);
				}
				if (k == 3)
				{
					sprintf(Si_CNC_data.m_strPicReq,"%3.0lf;", d.v);
					//GetDlgItem(IDC_EDIT_PIC_REQ)->SetWindowText(m_strPicReq);
					zlg_debug("mm_strPicReq = %s\r\n", Si_CNC_data.m_strPicReq);
				}
			}
			i += 12;
		} //for
		return 0; 
	}
	else return 1;
}

char Si_CNC_AlarmNo(unsigned char* data)
{
	union {
		unsigned char b[2];
		int v;
	}iv;
	if ((data[3] == 0xc3) && ((data[12] == 0x10) || ((data[24] == Si_CNC_data.ALARM_seqence_number) && (Si_CNC_data.ALARM_seqence_number != 0))))
	{

		if (data[12] == 0x10) Si_CNC_data.ALARM_seqence_number = data[24];
		if ((data[35] == 0xff) && (data[36] == 0x09))  //锟斤拷锟斤拷锟斤拷
		{
			if (data[187] == 0x01)  // No reset alarm
			{
				iv.v = 0;
				iv.b[0] = data[39];
				iv.b[1] = data[40];
				sprintf(Si_CNC_data.m_strAlarmNo,"%d;", iv.v);

				zlg_debug("m_strAlarmNo = %s\r\n", Si_CNC_data.m_strAlarmNo);
				if (iv.v != 0)
				{
					sprintf(Si_CNC_data.m_strAlarmTime,"%2x:%2x;", data[54], data[55]);
					//GetDlgItem(IDC_EDIT_ALARMTIME)->SetWindowText(m_strAlarmTime);
					zlg_debug("m_strAlarmTime = %s\r\n", Si_CNC_data.m_strAlarmTime);

					//m_pCollectConfig->m_pTemplate->m_nAlarmNum = 1;
					//Alarm al;
					//al.m_sCode = m_strAlarmNo;
					//m_pCollectConfig->m_pTemplate->m_vItemsAlarm.push_back(al);
				}
			}
			else
			{
				sprintf(Si_CNC_data.m_strAlarmNo,"%d;", 0);
				zlg_debug("m_strAlarmNo = %s\r\n", Si_CNC_data.m_strAlarmNo);
				sprintf(Si_CNC_data.m_strAlarmTime,"%s;","    ");
				zlg_debug("m_strAlarmTime = %s\r\n", Si_CNC_data.m_strAlarmTime);
			}
		}
		return 0;
	}
	else return 1;
}
char Si_CNC_end_collect(unsigned char* data)
{
	return 0;
}

void pack_analyze_result(void)
{
	strcat(result,Si_CNC_data.m_strState); //状态
	strcat(result,Si_CNC_data.m_strOprate);//模式
	strcat(result,Si_CNC_data.m_strVer5);
	strcat(result,Si_CNC_data.m_sAxisName1);
	strcat(result,Si_CNC_data.m_sAxisName2);
	strcat(result,Si_CNC_data.m_sAxisName3);
	strcat(result,Si_CNC_data.m_strToolLengthX);
	strcat(result,Si_CNC_data.m_strToolLengthZ);
	strcat(result,Si_CNC_data.m_strTooledg);
	strcat(result,Si_CNC_data.m_strToolRadiu);
	strcat(result,Si_CNC_data.m_strFeedAct);//实际进给量
	strcat(result,Si_CNC_data.m_strFeedSet);//设置进给量
	strcat(result,Si_CNC_data.m_strFeedRate);//进给倍率
	strcat(result,Si_CNC_data.m_strSAct);//
	strcat(result,Si_CNC_data.m_strSSet);//
	strcat(result,Si_CNC_data.m_strSRate);//
	strcat(result,Si_CNC_data.m_strMx1);//MacPos_x
	strcat(result,Si_CNC_data.m_strMy1);//MacPos_y
	strcat(result,Si_CNC_data.m_strMz1);//MacPos_z
	strcat(result,Si_CNC_data.m_strDisX);//
	strcat(result,Si_CNC_data.m_strDisY);//
	strcat(result,Si_CNC_data.m_strDisZ);//
	strcat(result,Si_CNC_data.m_strWorkX);//绝对坐标x
	strcat(result,Si_CNC_data.m_strWorkY);//绝对坐标y
	strcat(result,Si_CNC_data.m_strWorkZ);//绝对坐标z
	strcat(result,Si_CNC_data.m_ncycleTime);//循环时间
	strcat(result,Si_CNC_data.m_strTno);
	strcat(result,Si_CNC_data.m_strDno);	
	strcat(result,Si_CNC_data.m_strProAct);
	strcat(result,Si_CNC_data.m_strProLeft);
	strcat(result,Si_CNC_data.m_strPicAct);//产量
	strcat(result,Si_CNC_data.m_strPicReq);		
	strcat(result,Si_CNC_data.m_strProcessName);	
	//strcat(result,Si_CNC_data.m_strAlarmNo);
	//zlg_debug("---%s\r\n",result);
}

char (*Si_CNC_Data_Analyze[_Collect_Num])(unsigned char* data) ={
	&Si_CNC_Analyze_CycStart,
	&Si_CNC_Analyze_Oprate,
	&Si_CNC_Analyze_Ver,
	&Si_CNC_Analyze_Axis_Name,
	&Si_CNC_Analyze_ToolComp,
//	&Si_CNC_Analyze_CycStart,
	&Si_CNC_Analyze_AUTOFeed,
	&Si_CNC_Analyze_S_Data,
	&Si_CNC_Analyze_once_axis,
	&Si_CNC_Analyze_Work_Axis,
	&Si_CNC_Analyze_TDnumber,
	//&Si_CNC_Analyze_JOG_FEEDRATE,
	&Si_CNC_Piece_Time,
	&Si_CNC_Analyze_ProcessName,
	&Si_CNC_AlarmNo,
	//&Si_CNC_end_collect,
};


char Si_CNC_AnalyzeData(unsigned char* data,unsigned char index)
{
	int i = 0;
	if(data[3] == 0x13) return 1;
	return Si_CNC_Data_Analyze[index](data);
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

