#ifndef _BT_CONTROL_H_
#define _BT_CONTROL_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <sys/select.h>
#include "common.h"

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define PARAM_MAX_NUM  10
#define PARAM_MAX_LEN  2048
#define BT_DAEMON_SYNC_FIFO   "/data/bt_daemon_sync_fifo"
#define BT_DAEMON_FIFO        "/data/bt_daemon_fifo"
#define BT_SERVER_FIFO        "/data/bt_server_fifo"

#define BT_SEARCH_COMMAND           1
#define BT_IOCAP_COMMAND            2
#define BT_BOND_COMMAND             3
#define BT_ACCEPT_COMMAND           4
#define BT_DEBOND_COMMAND           5
#define BT_GET_BONDED_COMMAND       6

#define BT_POWER_COMMAND            11
#define BT_HOST_COMMAND             12
#define BT_PIN_READ_COMMAND             13

#define BT_SPP_SERVER_COMMAND       21
#define BT_GET_PROFILE_COMMAND      22
#define BT_CONNECT_COMMAND          23
#define BT_DISCONNECT_COMMAND       24
#define BT_SPP_SEND_COMMAND         25
#define BT_SPP_RECV_COMMAND         26
#define BT_SERVER_STATUS_COMMAND    27
#define BT_CONNECT_STATUS_COMMAND   28

#define BT_GATT_REGISTER_COMMAND    31
#define BT_GATT_ACTIVE_COMMAND      32
#define BT_GATT_ADVERTISE_COMMAND   33
#define BT_GATT_CONNECT_COMMAND     34
#define BT_GATT_CREATEDB_COMMAND    35
#define BT_GATT_CREATE_SERV_COMMAND 36
#define BT_GATT_CREATE_CHAR_COMMAND 37
#define BT_GATT_CREATE_DESC_COMMAND 38
#define BT_GATT_ADD_DB_COMMAND      39
#define BT_GATT_READ_IND_COMMAND    40
#define BT_GATT_READ_CFM_COMMAND    41
#define BT_GATT_WRITE_IND_COMMAND   42
#define BT_GATT_WRITE_CFM_COMMAND   43
#define BT_GATT_NOTIFY_COMMAND      44
#define BT_GATT_INDICATION_COMMAND  45
#define BT_GATT_MTU_CHANGED_COMMAND 46
#define BT_POWER_DOWN_COMMAND 47

#define BT_GET_SCAN_COMMAND         51
#define BT_SET_SCAN_COMMAND         52
#define BT_SET_INQUIRY_SCAN_COMMAND         53

#define BT_BOND_FAILED              0
#define BT_BOND_SUCCESS             1

#define BT_RESULT_FAILED            0
#define BT_RESULT_SUCCESS           1

#define IO_CAP_DISPLAY_ONLY                 0x00
#define IO_CAP_DISPLAY_YES_NO               0x01
#define IO_CAP_KEYBOARD_ONLY                0x02
#define IO_CAP_NO_INPUT_NO_OUTPUT           0x03


/* ---------Characteristic Properties bit fiels definition --------*/
#define ATT_PERM_CONFIGURE_BROADCAST            0x01
#define ATT_PERM_READ                           0x02
#define ATT_PERM_WRITE_CMD                      0x04
#define ATT_PERM_WRITE_REQ                      0x08
#define ATT_PERM_NOTIFY                         0x10
#define ATT_PERM_INDICATE                       0x20
#define ATT_PERM_AUTHENTICATED                  0x40
#define ATT_PERM_EXTENDED                       0x80

/*******************************************************************************
 * Attribute flags
 *******************************************************************************/

/* No attribute flags*/
#define ATTR_FLAGS_NONE                             0x0000
/* Read access to the attribute sends CSR_BT_GATT_DB_ACCESS_READ_IND
 * to the application */
#define ATTR_FLAGS_IRQ_READ                       0x10
/* Write access to the attribute sends CSR_BT_GATT_DB_ACCESS_WRITE_IND
 * to the application */
#define ATTR_FLAGS_IRQ_WRITE                     0x20


typedef enum {
  ACCEPT_MODE_NONE = 0,
  ACCEPT_MODE_COMPARE = 1,
  ACCEPT_MODE_PASSKEY = 2, 
  ACCEPT_MODE_REBOND =  3, 
  ACCEPT_MODE_NOTIFICATION = 4, 
  ACCEPT_MODE_JUSTWORK = 5, 
  ACCEPT_MODE_PINCODE = 6
}accept_mode;

typedef struct
{
    uint32_t    lap;   /*!< Lower Address Part 00..23 */
    uint8_t     uap;   /*!< upper Address Part 24..31 */
    uint16_t    nap;   /*!< Non-significant    32..47 */
} SIMCOM_BT_ADDR_T;
typedef struct bt_info{
    unsigned char bt_name[64];
    unsigned char bt_pw[32];
}bt_info;

typedef struct bt_msg
{
    int             command;
    int             param1;
    int             param2;
    int             param3;
    unsigned short  uuid16;
    unsigned char   uuid128[16];
    SIMCOM_BT_ADDR_T  address;
    int             result;
    unsigned int    check;
    unsigned int    attrHandle;
    unsigned int    data_len;
    unsigned int    offset;
    unsigned char   data[PARAM_MAX_LEN];
}bt_msg;

#define BT_FAILED              0
#define BT_SUCCESS             1
#define BT_DAEMON_VERSION      "BT_DAEMON_VER_20171206"

#define MAX_LOOP_CNT  3

typedef void (*bt_ind_cb_fcn)(void *pData);


	int bt_init(bt_ind_cb_fcn handle);

    /*****************************************************************************
    * Function Name : simcom_bt_power_on
    * Description   : 开关蓝牙  
    * Input         : bPower:   1. 开   0. 关闭
    * Output        : None
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/	
    int simcom_bt_power_on(int bPower);
    
    /*****************************************************************************
    * Function Name : search_devices
    * Description   : 搜索蓝牙设备
    * Input         : subcmd  1: start search   0: stop search
    				：mode     0: normal search  1: hide paired
    				：timeout  search timeout 0~60
    * Output        : 管道消息 BT_SEARCH_COMMAND
    * Return        : wifi_mode_type
    * Auther        : qjh
    * Date          : 2018.04.16
    *****************************************************************************/ 	
	int search_devices(int subcmd, int mode, int timeout);
	
    /*****************************************************************************
    * Function Name : set_iocap_device
    * Description   : 设置输入输出模式
    * Input         : mode  IO_CAP_DISPLAY_ONLY       (0x00)   只有显示
							IO_CAP_DISPLAY_YES_NO     (0x01)   有显示，并且能输入Yes/No
							IO_CAP_KEYBOARD_ONLY      (0x02)   没有显示， 有输入（可以输入字符）
							IO_CAP_NO_INPUT_NO_OUTPUT (0x03)   没有显示， 没有输入
   
    * Output        : 0: success   -1: fail
    * Return        : result
    * Auther        : qjh
    * Date          : 2018.04.16
    *****************************************************************************/ 	
	int set_iocap_device(int mode);
	
    /*****************************************************************************
    * Function Name : bond_device
    * Description   : 配对
    * Input         : index， 搜索列表index
                    ：注： 此接口成功后并非配对成功，仅仅只是发送请求成功。
                    ：      需要等待底层主动上报配对的下一步动作，比如确认密码等
    * Output        : 0: success   -1: fail  
    * Return        : result
    * Auther        : qjh
    * Date          : 2018.04.16
    *****************************************************************************/ 		
	int bond_device(int index);
	
    /*****************************************************************************
    * Function Name : debond_device
    * Description   : 解除配对
    * Input         : index， 已配对列表index
    * Output        : 0: success   -1: fail  -2 其它错误
    * Return        : result
    * Auther        : qjh
    * Date          : 2018.04.16
    *****************************************************************************/ 		
	int debond_device(int index);	

    /*****************************************************************************
    * Function Name : accept_bond
    * Description   : 接受配对
    * Input         : isAccept  非0 接受  ：   0： 不接受
                    ：mode
                    ：  ACCEPT_MODE_NONE = 0         不接受的时候模式可以忽略， 可以填一个0
					：  ACCEPT_MODE_COMPARE = 1      对比配对密码
					:  ACCEPT_MODE_PASSKEY = 2      需要输入密码
					:  ACCEPT_MODE_REBOND =  3      重新配对
					:  ACCEPT_MODE_NOTIFICATION = 4 仅通知，不需要回复（就是说模块收到此消息的时候，可以不需要调用此接口回复确认）
					:  ACCEPT_MODE_JUSTWORK = 5     此模式为直接配对， 模块不会收到配对确认的通知
					:  ACCEPT_MODE_PINCODE = 6      输入PIN码
					： accept mode只能设置为 0,1,2,3,6
    * Output        : 0: success   -1: fail  -2 其它错误
    * Return        : result
    * Auther        : qjh
    * Date          : 2018.04.20
    *****************************************************************************/ 
	int accept_bond(int isAccept, accept_mode mode);

    /*****************************************************************************
    * Function Name : get_bonded_device
    * Description   : 配对
    * Input         : None
                    ：等待 BT_GET_BONDED_COMMAND 主动上报，每一个配对会有一个上报
    * Output        : 0: success   -1: fail  
    * Return        : result
    * Auther        : qjh
    * Date          : 2018.04.16
    *****************************************************************************/ 		
	int get_bonded_device(void);

    /*****************************************************************************
    * Function Name : pin_read
    * Description   : 读取pin码
    * Input         : None
    * Output        : pincode：  pin码
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.16
    *****************************************************************************/ 	
	int pin_read(char *pincode);
	
    /*****************************************************************************
    * Function Name : pin_set
    * Description   : 设置pin码
    * Input         : None
    * Output        : pincode：  pin码
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.16
    *****************************************************************************/ 	
	int pin_set(char *pincode);
	
    /*****************************************************************************
    * Function Name : host_device
    * Description   : 读取蓝牙设备名称和MAC地址
    * Input         : None
    * Output        : device_name 设备名称       paddr 蓝牙地址 
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.16
    *****************************************************************************/ 	
	int host_device_addr(char *device_name, SIMCOM_BT_ADDR_T *paddr);
	
    /*****************************************************************************
    * Function Name : host_device
    * Description   : 设置蓝牙设备名称
    * Input         : device_name 设备名称
    * Output        : None
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.16
    *****************************************************************************/ 
	int  host_device(char *device_name);
    /*****************************************************************************
    * Function Name : spp_send_data
    * Description   : 蓝牙SPP发送数据
    * Input         : data： 数据       data_len  数据长度 
    * Output        : None
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.16
    *****************************************************************************/ 	
	int spp_send_data(uint8_t *data, uint16_t data_len);

    /*****************************************************************************
    * Function Name : spp_server
    * Description   : 开启或关闭SPP server
   					：SPP server关闭成功，有可能是当前spp处于连接状态。 处于连接状态的时候
   					：SPP就被关闭了。
    * Input         : bActive 0：关闭 1：开启 
    * Output        : None
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/ 		
	int spp_server(int bActive);


    /*****************************************************************************
    * Function Name : bt_get_profile
    * Description   : 查询配对的设备是否支持SPP（对方是否开启SPP服务）
    * Input         : index 配对列表index
    * Output        : isSupportSPP  0.不支持SPP    1.支持SPP
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/ 		
	int bt_get_profile(int index, int *isSupportSPP);
	
    /*****************************************************************************
    * Function Name : bt_get_server_status
    * Description   : 查询当前模块SPP Server状态
    * Input         : None
    * Output        : isActived    0: 关闭    1：开启
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/ 	
	int bt_get_server_status(int *isActived);

    /*****************************************************************************
    * Function Name : bt_get_connect_status
    * Description   : 查询当前模块SPP 连接状态
    * Input         : None
    * Output        : isConnected    0: 断开    1：连接
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/
	int bt_get_connect_status(int *isConnected);
	
    /*****************************************************************************
    * Function Name : bt_connect_profile_id
    * Description   : 通过SPP 连接配对列表中的设备
    * Input         : Index. 配对列表index   
    * Output        : None
    * Return        : 0：success   -1： fail  -2: 连接失败
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/
	int spp_conncet_device(int index);

    /*****************************************************************************
    * Function Name : spp_disconncet_device
    * Description   : 断开SPP 连接
    * Input         : None
    * Output        : None
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.16
    *****************************************************************************/ 		
	int spp_disconncet_device(void);

    /*****************************************************************************
    * Function Name : bt_gatt_register
    * Description   : 注册GATT
    * Input         : bRegister   是否注册       1.注册   2. 取消注册   
    * Output        : None
    * Return        : 0：success   -1： fail
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/	
	int bt_gatt_register(int bRegister);


    /*****************************************************************************
    * Function Name : bt_gatt_active
    * Description   : active gatt server
    * Input         : bactive： 1. active （只有active， 没有deactive）   
    * Output        : 
    * Return        : 0：success   -1： fail   -2：流程状态不对
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/	
	int bt_gatt_active(int bActive);

    /*****************************************************************************
    * Function Name : bt_gatt_createdatabase
    * Description   : create gatt db
    * Input         : bCreateDb  1. 创建     0：删除（暂不支持）
    * Output        : 
    * Return        : 0：success   -1： fail   -2：流程状态不对
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/	
	int  bt_gatt_createdatabase(int bCreateDb);
	
    /*****************************************************************************
    * Function Name : bt_gatt_create_service
    * Description   : create gatt service (uuid16)
    * Input         : uuid16
    * Output        : 
    * Return        : 0：success   -1： fail   -2：流程状态不对
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/	
	int bt_gatt_create_service_uuid16(int uuid16);
	
    /*****************************************************************************
    * Function Name : bt_gatt_create_service
    * Description   : create gatt service (uuid128)
    * Input         : uuid128
    * Output        : 
    * Return        : 0：success   -1： fail   -2：流程状态不对
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/	
	int bt_gatt_create_service_uuid128(unsigned char *uuid128);
	
	
    /*****************************************************************************
    * Function Name : bt_gatt_create_characteristic
    * Description   : create gatt characteristic (uuid16)
    * Input         : uuid16 pro permission
    * Output        : attrHandle
    * Return        : 0：success   -1： fail   -2：流程状态不对 -3: 其他错误
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/	
	int bt_gatt_create_characteristic_uuid16(int uuid16, int pro, int permission, uint32_t *attrHandle);
	
    /*****************************************************************************
    * Function Name : bt_gatt_create_characteristic
    * Description   : create gatt characteristic (uuid128)
    * Input         : uuid128 pro permission
    * Output        : attrHandle
    * Return        : 0：success   -1： fail   -2：流程状态不对 -3: 其他错误
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/	
    
	int bt_gatt_create_characteristic_uuid128(unsigned char *uuid128,int pro,int permission, uint32_t *attrHandle);

    /*****************************************************************************
    * Function Name : bt_gatt_create_descriptor
    * Description   : create gatt descriptor 
    * Input         : None
    * Output        : None
    * Return        : 0：success   -1： fail   -2：流程状态不对
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/	
    
	int bt_gatt_create_descriptor();
	
    /*****************************************************************************
    * Function Name : bt_gatt_add_service_2_db
    * Description   : add service to database 
    * Input         : None
    * Output        : None
    * Return        : 0：success   -1： fail   -2：流程状态不对
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/
	int bt_gatt_add_service_2_db();
	
    /*****************************************************************************
    * Function Name : bt_gatt_notification
    * Description   : gatt notification
    * Input         : None
    * Output        : None
    * Return        : 0：success   -1： fail   -2：流程状态不对
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/	
	int bt_gatt_notification(uint16_t attrhandle,char *data);
	
    /*****************************************************************************
    * Function Name : bt_gatt_indication
    * Description   : gatt indication
    * Input         : None
    * Output        : None
    * Return        : 0：success   -1： fail   -2：流程状态不对
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/		
	int bt_gatt_indication(uint16_t attrhandle,char *data);

    /*****************************************************************************
    * Function Name : bt_gatt_read_cfm
    * Description   : 其他设备通过GATT访问本地数据 回复确认
    * Input         : attrHandle 
                    : rspCode 
                    : data data_len
    * Output        : None
    * Return        : 0：success   -1： fail   -2：其它错误
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/	
	int bt_gatt_read_cfm(uint16_t       attrHandle, int rspCode, uint8_t *data, uint32_t data_len);
	
	
    /*****************************************************************************
    * Function Name : bt_gatt_write_cfm
    * Description   : 其他设备通过GATT写本地数据， 回复确认
    * Input         : attrHandle 
                    : rspCode 
    * Output        : None
    * Return        : 0：success   -1： fail   -2：其它错误
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/		
	int bt_gatt_write_cfm(uint16_t       attrHandle, int rspCode);	
	
    /*****************************************************************************
    * Function Name : bt_get_scan_enable
    * Description   : 获取inquiry scan 和 page scan 状态
    			    ：注： inquiry_scan_status  ：  其他设备是否可以发现本蓝牙
    			    ：      page_scan_status：   其他设备是否可以连接本蓝牙
    * Input         : None
    * Output        : inquiry_scan_status:   1. on   0. off
					: page_scan_status:      1. on   0. off
    * Return        : 0：success   -1： fail   -2：其他错误
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/	  
	int bt_get_scan_enable(int *inquiry_scan_status, int *page_scan_status);
	
    /*****************************************************************************
    * Function Name : bt_get_scan_enable
    * Description   : gatt indication  
    * Input         : inquiry_scan_status:   1. on   0. off
					: page_scan_status:      1. on   0. off
    * Output        : None
    * Return        : 0：success   -1： fail   -2：其他错误
    * Auther        : qjh
    * Date          : 2018.04.17
    *****************************************************************************/		
	int bt_set_scan_enable(int inquiry_scan_status, int page_scan_status);
	

#endif

