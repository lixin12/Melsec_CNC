#ifndef _WIFI_CONTROL_H_
#define _WIFI_CONTROL_H_
#include "LedControl.h"
#include "qcmap_client_def.h"


typedef enum{
	AP_MODE = 0,
	APAP_MODE,
	APSTA_MODE
}wifi_mode_type;

typedef enum{
	AP_INDEX_0 = 0,
	AP_INDEX_1,
	AP_INDEX_STA
}ap_index_type;


typedef unsigned char boolean;
typedef unsigned char uint8;

    
    /*****************************************************************************
    * Function Name : get_wifi_mode
    * Description   : 获取当前WIFI模式设置
    * Input         : None
    * Output        : 0：单AP模式     1：双AP模式     2: AP+STA模式
    * Return        : wifi_mode_type
    * Auther        : qjh
    * Date          : 2018.04.10
    *****************************************************************************/ 
    wifi_mode_type get_wifi_mode();
    
	/*****************************************************************************
	* Function Name : set_wifi_mode
	* Description   : 设置WIFI模式
	* Input         : 0：单AP模式     1：双AP模式     2: AP+STA模式
	* Output        : TRUE or FALSE
	* Return        : boolean
	* Auther        : qjh
	* Date          : 2018.04.10
	*****************************************************************************/
	boolean set_wifi_mode(wifi_mode_type mode);
	
    /*****************************************************************************
    * Function Name : wifi_power
    * Description   : WIFI开关
    * Input         : act 1： 打开      0： 关闭
    * Output        : None
    * Return        : int
    * Auther        : qjh
    * Date          : 2018.04.10
    *****************************************************************************/	
	boolean wifi_power(int act);

    /*****************************************************************************
    * Function Name : get_wifi_status
    * Description   : 获取WIFI 状态
    * Input         : act 1： 打开      0： 关闭
    * Output        : None
    * Return        : int
    * Auther        : qjh
    * Date          : 2018.04.10
    *****************************************************************************/	
	boolean get_wifi_status(uint8 *flag);

	/*****************************************************************************
	* Function Name : set_auth
	* Description   : 设置AP auth类型, 加密模式, 密码

		1.当auth type输入为0或1时，encrypt mode的输入值为0或1；
		2.当auth type输入为2时，encrypt mode的输入值只能为1；
		3.当auth type的输入值大于3时，encrypt mode的输入值必须大于等于2；
		4.当encrypt mode输入为0时，不需要输入password；
		5.当encrypt mode输入为1时，password必须输入。
		     Password输入格式必须满足：长度为5或者13的ASCII编码字符串，
		     或者长度为10或者26的十六进制编码字符串；
		6.当encrypt mode的输入值大于等于2时，password必须输入。
		     Password输入的格式必须满足：长度为8到63的ASCII编码字符串，
		     或者长度为64的十六进制编码字符串。

		Default value :
			int authType = 5;
			int encryptMode = 4;
			ap_index_type: 0-> ap
						   1-> ap & ap
						   2-> ap & sta
	* Input			: int authType, int encryptMode, char *password
	* Output        : None
	* Return        : 0:success, -1:failed
	* Auther        : qjh
	* Date          : 2018.04.11
	*****************************************************************************/	
	boolean set_auth(char *str_pwd, int auth_type, int  encrypt_mode, ap_index_type ap_index);


	/*****************************************************************************
	* Function Name : wifi_get_ap_auth
	* Description   : 获取AP auth类型, 加密模式, 密码
	* Input			: int *authType, int *encryptMode, char *pwd_str,
	                  ap_index_type: 0-> ap
						             1-> ap & ap
						             2-> ap & sta
	* Output        : None
	* Return        : 0:success, -1:failed
	* Auther        : qjh
	* Date          : 2018.04.11
	*****************************************************************************/	
	boolean get_auth(int *auth_type_ptr, int *encrypt_mode_ptr, char *pwd_str, ap_index_type ap_index);

	/*****************************************************************************
	* Function Name : set_ssid 
	* Description   : 设置WIFI 热点名称
	* Input			: char *ssid,
	                  ap_index_type: 0-> ap
						             1-> ap & ap
						             2-> ap & sta
	* Output        : None
	* Return        : 0:success, -1:failed
	* Auther        : qjh
	* Date          : 2018.04.11
	*****************************************************************************/		
	boolean set_ssid(char *SSID, ap_index_type ap_index);
	/*****************************************************************************
	* Function Name : get_ssid 
	* Description   : 获取WIFI 热点名称
	* Input			: char *ssid,
	                  ap_index_type: 0-> ap
						             1-> ap & ap
						             2-> ap & sta
	* Output        : None
	* Return        : 0:success, -1:failed
	* Auther        : qjh
	* Date          : 2018.04.11
	*****************************************************************************/		
	boolean get_ssid(char *str_SSID, ap_index_type ap_index);
	
	/*****************************************************************************
	* Function Name : set_bcast 
	* Description   : 设置WIFI 广播开关
	* Input			: int broadcast，
	                  ap_index_type: 0-> ap
						             1-> ap & ap
						             2-> ap & sta
	* Output        : None
	* Return        : 0:success, -1:failed
	* Auther        : qjh
	* Date          : 2018.04.11
	*****************************************************************************/		
	boolean set_bcast(int broadcast, ap_index_type ap_index);
	/*****************************************************************************
	* Function Name : get_bcast 
	* Description   : 获取WIFI 广播设定
	* Input			: int broadcast,
	                  ap_index_type: 0-> ap
						             1-> ap & ap
						             2-> ap & sta
	* Output        : None
	* Return        : 0:success, -1:failed
	* Auther        : qjh
	* Date          : 2018.04.11
	*****************************************************************************/	
	boolean get_bcast(int *broadcast,ap_index_type ap_index);
	
	
	/*****************************************************************************
	* Function Name : get_dhcp 
	* Description   : 获取DHCP设定
	* Input			: ap_index_type: 0-> ap
						             1-> ap & ap
						             2-> ap & sta
	* Output        : host_ip_str： 主机地址 start_ip_str：开始地址 end_ip_str：结束地址
	* Return        : 0:success, -1:failed
	* Auther        : qjh
	* Date          : 2018.04.11
	*****************************************************************************/		
	boolean get_dhcp(char *host_ip_str, char *start_ip_str, char *end_ip_str, char *time_str);

	/*****************************************************************************
	* Function Name : get_client_count 
	* Description   : 获取连接的客户端数
	* Input			: ap_index_type: 0-> ap
						             1-> ap & ap
						             2-> ap & sta
	* Output        : 连接总数
	* Return        : 0:success, -1:failed
	* Auther        : qjh
	* Date          : 2018.04.11
	*****************************************************************************/		
	int get_client_count(ap_index_type ap_index);
	
	/*****************************************************************************
	* Function Name : get_mac_addr 
	* Description   : 获取WIFI物理地址
	* Input			: ap_index_type: 0-> ap
						             1-> ap & ap
						             2-> ap & sta
	* Output        : *mac_addr
	* Return        : int： 总数
	* Auther        : qjh
	* Date          : 2018.04.11
	*****************************************************************************/		
	boolean get_mac_addr(char *mac_addr, ap_index_type ap_index);


    /*****************************************************************************
    * Function Name : get_sta_ip
    * Description   : 设置STA连接外部热点后获取到的IP
    * Input         : None
    * Output        : ip_str
    * Return        : boolean
    * Auther        : qjh
    * Date          : 2018.04.11
    *****************************************************************************/	    
	boolean get_sta_ip(char *ip_str, int len);
	

    /*****************************************************************************
    * Function Name : set_sta_cfg
    * Description   : 设置STA连接外部AP的 SSID 和密码
    * Input         : char *SSID, char *psk_value（密码）
    * Output        : None
    * Return        : boolean
    * Auther        : qjh
    * Date          : 2018.04.11
    *****************************************************************************/	
	boolean set_sta_cfg(char *ssid_str, char *psk_value);
	
	
    /*****************************************************************************
    * Function Name : get_sta_cfg
    * Description   : 获取STA设置的SSID 和 密码
    * Input         : char *SSID, char *psk_value（密码）
    * Output        : None
    * Return        : boolean
    * Auther        : qjh
    * Date          : 2018.04.11
    *****************************************************************************/	
	boolean get_sta_cfg(char *ssid_str, char *psk_value);	

    /*****************************************************************************
    * Function Name : sta_scan
    * Description   : wifi sta 扫描可用热点
    * Input         : None
    * Output        : list_str
    * Return        : boolean
    * Auther        : qjh
    * Date          : 2018.04.11
    *****************************************************************************/	
	boolean sta_scan(char *list_str);

    /*****************************************************************************
    * Function Name : sta_scan_with_sig
    * Description   : wifi sta 扫描可用热点
    * Input         : None
    * Output        : list_str
    * Return        : boolean
    * Auther        : qjh
    * Date          : 2018.04.11
    *****************************************************************************/
    boolean sta_scan_with_sig(char *list_str);

    /*****************************************************************************
    * Function Name : set_user_name_pwd
    * Description   : 设置cdma模式拨号的用户名和密码
    * Input         : sz_usrname sz_usrpwd
    * Output        : None
    * Return        : boolean
    * Auther        : qjh
    * Date          : 2018.04.11
    *****************************************************************************/		
	boolean set_user_name_pwd(char *sz_usrname, char *sz_usrpwd);

    /*****************************************************************************
    * Function Name : get_user_name_pwd
    * Description   : 获取cdma模式拨号的用户名和密码
    * Input         : None
    * Output        : sz_usrname sz_usrpwd
    * Return        : boolean
    * Auther        : qjh
    * Date          : 2018.04.11
    *****************************************************************************/		
	boolean get_user_name_pwd(char *sz_usrname, int len_name, char *sz_usrpwd, int len_pwd);
	
    /*****************************************************************************
    * Function Name : get_net_status
    * Description   : 获取网络状态
    * Input         : None
    * Output        : net_enable_str to_be_save
    * Return        : boolean
    * Auther        : qjh
    * Date          : 2018.04.11
    *****************************************************************************/		
	boolean get_net_status(char *net_enable_str, int *to_be_save);

	
    /*****************************************************************************
    * Function Name : restore_wifi
    * Description   : 恢复wifi设定
    * Input         : None
    * Output        : None
    * Return        : void
    * Auther        : qjh
    * Date          : 2018.04.11
    *****************************************************************************/		
	void restore_wifi();

boolean set_ssid_and_auth(char *SSID, char *str_pwd, int auth_type, int  encrypt_mode, ap_index_type ap_index);

boolean wifi_is_sta_mode();

/*****************************************************************************
    * Function Name : sta_init
    * Description   : open/close STA mode for W58L
    * Input         : int: sta_enable
    * Output        : None
    * Return        : boolean
    * Auther        : qjh
    * Date          : 2018.04.11
    *****************************************************************************/	
	boolean sta_init(int sta_enable);
	
	
    /*****************************************************************************
    * Function Name : get_sta_cfg
    * Description   : get the STA state for W58L
    * Input         : None
    * Output        : flag
    * Return        : boolean
    * Auther        : qjh
    * Date          : 2018.04.11
    *****************************************************************************/	
	boolean get_sta_status(uint8 *flag);

/*****************************************************************************
    * Function Name : wifi_net_cnct
    * Description   : connect or disconnect the net;
                                  if save the net status for later reboot
    * Input         : act  to_be_dave
    * Output        : None
    * Return        : boolean
    * Auther        : qjh
    * Date          : 2018.10.26
    *****************************************************************************/	
    boolean wifi_net_cnct(int act, int to_be_save);

/*****************************************************************************
		* Function Name : wifi_get_net_status
		* Description	: get the net connected or disconnected status;
		* Input 		: None
		* Output		: act
		* Return		: boolean
		* Auther		: dyy
		* Date			: 2018.12.27
		*****************************************************************************/	
    boolean wifi_get_net_status(int *act);


/*****************************************************************************

    * Function Name : wifi_get_err_code
    * Description   : get the latest error code
    * Input         : None
    * Output        : None
    * Return        : unit8
    * Auther        : qjh
    * Date          : 2018.10.26
    *****************************************************************************/	
    uint8 wifi_get_err_code();
#endif

