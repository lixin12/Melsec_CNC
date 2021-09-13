#include "type.h"
#include "Siemens_CNC.h"
//typedef enum{RCS=0x01,RIS=2,RHR=3,RIR=4,FSC=5,PSR=6,FMC=15,PMR=16}MODBUS_TCP_TYPE; func code
//_U16=1,_U32=2,_FLOAT32=3,_INT16=4,_INT32=5

//一下函数禁止修改
int Get_Si_CNC_Collect_Num()
{
	return _Collect_Num;
}