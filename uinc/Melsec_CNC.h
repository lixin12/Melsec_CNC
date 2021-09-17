#ifndef _MELSEC_CNC_H
#define _MELSEC_CNC_H

#define _Data_Num     31

#define  M_ALM_ALL_ALARM   			0
#define  M_ALM_OPE_MSG     			4
#define  M_ALM_PLC_ALARM   			3
#define  M_ALM_STOP_CODE   			2
#define  M_ALM_NC_ALARM    			1
#define  AXIS_X            			1
#define  AXIS_Y            			2
#define  AXIS_Z            			4
#define  AXIS_4            			8
#define  EZNC_M            			0    // M command (sub function M command value)
#define  EZNC_S            			1     //S command (spindle rotation speed S command value)
#define  EZNC_T            			2     //T command (tool change T command value)
#define  EZNC_B            			3    //B command (second sub function command value
//主程序，子程序
#define	EZNC_MAINPRG       			0  // Main progra
#define EZNC_SUBPRG        			1   // Subprogram

//关于主轴ID
#define  ID_SPENDLE_GAIN       		100       //增益
#define  ID_SPENDLE_TEMP       		101       //温度
#define  ID_SPENDLE_LOAD       		102       //负载
#define  ID_SPENDLE_I          		103       //电流
#define  ID_SPENDLE_SPEED      		104       //速度

#define  ID_SERVO_LOADX   			111      
#define  ID_SERVO_IX      			112      
#define  ID_SERVO_SPEEDX  			113      
#define  ID_SERVO_NAME1  			114      
#define  ID_POS_CURRENT_X  			115
#define  ID_POS_WORK_X     			116
#define  ID_POS_MACHINE_X     		117
#define  ID_POS_DIS_X     			118

#define  ID_SERVO_GAINY   			120      
#define  ID_SERVO_LOADY   			121      
#define  ID_SERVO_IY      			122      
#define  ID_SERVO_SPEEDY  			123      
#define  ID_SERVO_NAME2   			124     
#define  ID_POS_CURRENT_Y 			125
#define  ID_POS_WORK_Y    			126
#define  ID_POS_MACHINE_Y 			127
#define  ID_POS_DIS_Y     			128

#define  ID_SERVO_GAINZ   			130   
#define  ID_SERVO_LOADZ   			131   
#define  ID_SERVO_IZ      			132   
#define  ID_SERVO_SPEEDZ  			133   
#define  ID_SERVO_NAME3    			134   
#define  ID_POS_CURRENT_Z  			135
#define  ID_POS_WORK_Z     			136
#define  ID_POS_MACHINE_Z     		137
#define  ID_POS_DIS_Z     			138

#define  ID_SERVO_GAINB   			140     
#define  ID_SERVO_LOADB   			141     
#define  ID_SERVO_IB      			142     
#define  ID_SERVO_SPEEDB  			143     
#define  ID_SERVO_NAME4  			144     
#define  ID_POS_CURRENT_4  			145
#define  ID_POS_WORK_4     			146
#define  ID_POS_MACHINE_4    		147
#define  ID_POS_DIS_4     			148

#define  ID_SERVO_NUM    			151
#define  ID_RDY_HLD      			152
#define  ID_PROC_MODE    			153
#define  ID_VER1         			154
#define  ID_VER2         			155  
#define  ID_VER3         			156 
#define  ID_SYSTEM_LM    			157
#define  ID_ALARM_MESSAGE 			158
#define  ID_FEED_SET       			165
#define  ID_FEED_ACT       			166

//PROGRRAM
#define ID_EZNC_PRG_CURNUM        	170
#define ID_EZNC_PRG_RESTNUM       	171
#define ID_EZNC_PRG_CHARNUM        	172
#define ID_EZNC_PRG_RESTCHARNUM     173

#define ID_OPEN_DIRECTORY           174
#define ID_READ_DIRECTORY           175
#define ID_CLOSE_DIRECTORY          176
#define ID_STAT_FILE                177
#define ID_OPEN_FILE                178
#define ID_READ_FILE                179
#define ID_READ_FILE2               180
#define ID_CLOSE_FILE               181
#define ID_DELETE_FILE              182

#define  ID_FILE_NAME       		185
#define  ID_BLOCK_NUM       		186
#define  ID_PROCESS_DOC     		187
#define  ID_N_NUM           		188

#define  ID_INPUT_FEEDRATE      	190
#define  ID_INPUT_SPENDLERATE   	191

#define  ID_OUTPUT_Y         		200 
#define  ID_TEST_PAR         		250 

#define  ID_GFUN_1           		261       //G00 G01
#define  ID_GFUN_3           		262       //G90 
#define  ID_GFUN_5           		263       //G93 G94 G95
#define  ID_GFUN_7           		264       //G40 G41 G42
#define  ID_GFUN_12          		265       //G54 G55 G56

#define  ID_TOOL_LIFE_1      		301
#define  ID_TOOL_LIFE_2      		302
#define  ID_TOOL_COMP_11     		331
#define  ID_TOOL_COMP_12     		332
#define  ID_TOOL_COMP_13     		333
#define  ID_TOOL_COMP_15     		334
#define  ID_TOOL_COMP_16     		335
#define  ID_TOOL_COMP_17     		336
#define  ID_TOOL_D_D         		340
#define  ID_TOOL_D_W         		341
#define  ID_TOOL_H           		342

#define  ID_G54_X            		350 
#define  ID_G54_Y            		351 
#define  ID_G54_Z            		352 
#define  ID_G55_X            		353 
#define  ID_G55_Y            		354 
#define  ID_G55_Z            		355   
#define  ID_G56_X            		356 
#define  ID_G56_Y            		357 
#define  ID_G56_Z            		358   
#define  ID_G57_X            		359 
#define  ID_G57_Y            		360 
#define  ID_G57_Z            		361   
#define  ID_G58_X            		362 
#define  ID_G58_Y            		363 
#define  ID_G58_Z            		364   
#define  ID_G59_X            		365 
#define  ID_G59_Y            		366 
#define  ID_G59_Z            		367  
//COMMAND2
#define  ID_TOOL_NUM        		400
#define  ID_SPENDLE_CWCCW   		401
#define  ID_SEENDLE_SET     		402
//ID
#define ID_DATE           			700
#define ID_TIME           			701
#define ID_POWER          			702
#define ID_AUTO           			703
#define ID_START          			704
#define ID_PROCESS        			705
//鍙橀噺
#define ID_COMM_VAR_R      			710
#define ID_COMM_VAR_W      			711
#define ID_LOCAL_VAR_R     			712
#define ID8002N           			80020     //已经加工的工件数


extern int sock;
extern char rev[2048];

void app_melsec_cnc_logic(void);
int melsec_cnc_init(void);
void app_TestMQ(void);
void app_ConvertToQueue(void);
unsigned char bsp_GetOnLine(void);
char* bsp_GetDistMac(void);
unsigned char app_GetDistMac(void);

char* SI_CNC_GetResult(unsigned char i);

void SI_CNC_ClearResult(void);
int Get_SendDataCNT(void);
int Get_SendCollectCNT(void);
int melsec_cnc_analyze_data(unsigned char* data);
void pack_analyze_result(void);

int Get_HighLow_Exchange(int i);

int get_melsec_cmd_str();

unsigned char GetDeviceID(void);
int TC_GetResultCnt(void);


#endif
