#ifndef _UIM_CONTROL_H_
#define _UIM_CONTROL_H_
#include "qmi_client.h"
#include "user_identity_module_v01.h"
#include "simcom_common.h"

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

typedef unsigned char boolean;
typedef unsigned char uint8;

typedef struct{
	uim_card_state_enum_v01  card_status;
	uim_app_type_enum_v01    app_type;
	uim_app_state_enum_v01   app_state;
	uim_pin_info_type_v01    pin;
}SimCard_Status_type;


	int uim_init();
	void uim_deinit();
	/*****************************************************************************
	* Function Name : getSimCardStatus
	* Description   : 获取SIM卡的状态
	* Input         : 无
	* Output        : simStatus
					  card_status:     (0: not exist 1: Exist: 2: UNKNOW)
					  app_type:        (1: SIM 2: USIM 3: RUIM 4: CSIM 5: ISIM)
					  app_state        (1: detected 2: pin block  3: puk block 4: person check  5:Permanently blocked 6: illgeal)
					  pin_state        (1: unknown  2: Enabled and not verified 3: Enabled and verified  4: Disabled 5: Blocked  6:Permanently blocked)
		              pin_retries      剩余解pin次数
					  puk_retries      剩余解puk次数
	* Return        : boolean
	* Auther        : qjh
	* Date          : 2018.04.10
	*****************************************************************************/
	int getSimCardStatus(SimCard_Status_type *simStatus);
	/*****************************************************************************
	* Function Name : getIccid
	* Description   : 查询sim卡iccid
	* Input         : 无
	* Output        : pIccid
	* Return        : 0： succes     -1： Fail
	* Auther        : qjh
	* Date          : 2018.04.10
	*****************************************************************************/	
	int get_iccid(char *piccid);
	
	/*****************************************************************************
	* Function Name : get_imsi
	* Description   : 查询sim卡imsi
	* Input         : 无
	* Output        : pIccid
	* Return        : 0： succes     -1： Fail
	* Auther        : qjh
	* Date          : 2018.04.10
	*****************************************************************************/	
	int get_imsi(char *imsi);

#endif


