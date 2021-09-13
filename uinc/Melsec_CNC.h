#ifndef _Siemens_CNC_H
#define _Siemens_CNC_H

#define _Collect_Num  13
#define _Data_Num     33
#define Type_Size     40

typedef struct{
    int  m_MachineMode;//0 T 1 M
    int  m_OpMode;
    int  m_iState;
    int  m_nCycleNum;
    int  m_nStatusCh1;
	int  m_nStatusCh2;
    int  m_nCycleTime;
    int  m_nFinishPartsNum;
    int  ALARM_seqence_number;
    double m_dFAct;
    double m_dFCmd;
    double m_dSOvr;
    double m_dSAct;
	double m_dSCmd;
    double m_dFOvr;
    float m_dMacPos_x;
    float m_dMacPos_y;
    float m_dMacPos_z;
    float m_dRemPos_x;
    float m_dRemPos_y;
    float m_dRemPos_z;
    float m_dAbsPos_x;
    float m_dAbsPos_y;
    float m_dAbsPos_z;
    char m_strVer1[Type_Size];
    char m_strVer2[Type_Size]; 
    char m_strVer3[Type_Size]; 
    char m_strVer4[Type_Size]; 
    char m_strVer5[Type_Size];
    char m_sAxisName1[5];
    char m_sAxisName2[5];
    char m_sAxisName3[5];
    char m_strOprate[5];
    char m_strState[20];
    char m_strFeedAct[20];
    char m_strFeedSet[20];
    char m_strSAct[20];
    char m_strSSet[20];
    char m_strSRate[20];
    char m_strMx1[20];
	char m_strMy1[20];
	char m_strMz1[20];
    char m_strDisX[20];
    char m_strDisY[20];
    char m_strDisZ[20];
    char m_strWorkX[20];
    char m_strWorkY[20];
    char m_strWorkZ[20];
    char m_strToolLengthX[20];
    char m_strToolLengthZ[20];
    char m_strTooledg[20];
    char m_strToolRadiu[20];
    char m_strHnum[20];
    char m_strTno[20];
    char m_strDno[20];
    char m_strFeedRate[20];
    char m_strProcessName[100];
    char m_sCurProgramNo[20];
    char m_strProAct[20];
    char m_strProLeft[20];
    char m_strPicAct[20];
    char m_strPicReq[20];
    char m_ncycleTime[20];
    char m_strAlarmNo[20];
    char m_strAlarmTime[20];
}Si_CNC_Data;

Si_CNC_Data Si_CNC_data;

void app_Si_CNC_Logic(void);
int Si_CNC_Init(void);
void app_TestMQ(void);
void app_ConvertToQueue(void);
unsigned char bsp_GetOnLine(void);
char* bsp_GetDistMac(void);
unsigned char app_GetDistMac(void);

char* SI_CNC_GetResult(unsigned char i);

void SI_CNC_ClearResult(void);
int Get_SendDataCNT(void);
int Get_SendCollectCNT(void);
char Si_CNC_AnalyzeData(unsigned char* data,unsigned char index);
void pack_analyze_result(void);

int Get_HighLow_Exchange(int i);

char* Get_Si_CNC_CMD_STR(int i);

unsigned char GetDeviceID(void);
int TC_GetResultCnt(void);


#endif
