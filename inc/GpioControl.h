#ifndef _GPIO_CONTROL_H_
#define _GPIO_CONTROL_H_


/*****************************************************************************
* Function Name : gipo_wakeup_init
* Description   : gpio唤醒脚的配置
* Input         : None
* Output        : None
* Return        : int 
* Auther        : simcom
* Date          : 2018.04.16
*****************************************************************************/
void* gipo_wakeup_init(void* cmdpipefd);


/*****************************************************************************
* Function Name : modem_ri_notify_mcu
* Description   : 用于4g唤醒mcu
* Input         : None
* Output        : None
* Return        : int 
* Auther        : simcom
* Date          : 2018.04.16
*****************************************************************************/
int modem_ri_notify_mcu(void);

/*****************************************************************************
* Function Name : lowPowerMode
* Description   : go to low power mode,
                  isSleep ->0,  go to sleep
                  isSleep ->1,  wake up system
* Input         : None
* Output        : None
* Return        : 0: success 
* Auther        : simcom
* Date          : 2018.04.16
*****************************************************************************/
int lowPowerMode(int isSleep);


#endif

