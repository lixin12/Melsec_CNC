#ifndef _VOICE_CALL_H_
#define _VOICE_CALL_H_
#include "voice_service_v02.h"
#include "simcom_common.h"


typedef enum {
    VOICE_CALL_HANDUP = 1,
    VOICE_CALL_ANSWER,
    VOICE_CALL_HOLD,
    VOICE_END_ALL,
    VOICE_END_BG,
    VOICE_END_FG
}voice_call_option;

#define VOICE_QMI_TIMEOUT_VALUE 10000

typedef struct {
    uint8_t call_id;
    call_state_enum_v02 call_state;
    call_direction_enum_v02 direction;
    char phone_number[QMI_VOICE_NUMBER_MAX_V02];
}call_info2_type;

typedef struct {
    uint8_t call_num;
	call_info2_type call_info[QMI_VOICE_CALL_INFO_MAX_V02];
}call_info_type;



	int voice_init();
	void voice_deinit();
	
    /*****************************************************************************
    * Function Name : voice_dial
    * Description   : 拨打电话
    * Input         : phoneNum  电话号码
    * Output        : None
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.19
    *****************************************************************************/ 	
	int voice_dial(char *phoneNum);
	
    /*****************************************************************************
    * Function Name : voice_dial
    * Description   : 处理通话或者来电
    * Input         : voice_call_option：   
                    ：  VOICE_CALL_HANDUP   挂断
                    ：  VOICE_CALL_ANSWER   接听
                    ：  VOICE_CALL_HOLD     挂起当前电话，接入新来电
                    ：  VOICE_END_ALL       挂断所有
                    ：  VOICE_END_BG        挂断后台，保持当前通话
                    ：  VOICE_END_FG        挂断当前电话， 后台电话转为接入
                    ：call_id  标记每一路通话的ID
    * Output        : None
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.19
    *****************************************************************************/ 	
	int voice_mtcall_process(voice_call_option option, unsigned char call_id);

    /*****************************************************************************
    * Function Name : get_all_call_info
    * Description   : 获取所有通话信息
    * Input         : None
    * Output        : pcall_info_list
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.19
    *****************************************************************************/ 		
	int get_all_call_info(call_info_type *pcall_info_list);

	/*****************************************************************************
    * Function Name : voice_dial
    * Description   : 获取特定通话信息
    * Input         : callid
    * Output        : pcall_info
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.19
    *****************************************************************************/ 
	int get_call_info(uint8_t callid, call_info2_type *pcall_info);

#endif

