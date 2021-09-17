#ifndef _DEV_CONTROL_H_
#define _DEV_CONTROL_H_

#include <stdbool.h>

/*****************************************************************************
* Function Name : exec_cdelta_cmd
* Description	: set UE enter recovery mode
* Input 		: ota packet full path, when paramter path is NULL, use the default path /cache/update_ota.zip
* Output		: None
* Return		: 0 succes	 -1 Fail
* Auther		: lhy
* Date		: 2018.08.23
*****************************************************************************/	
int exec_cdelta_cmd(const char *path);

/*****************************************************************************
* Function Name : exec_cusbadb_cmd
* Description	: open or close adb port 
* Input 		: 1:open  0: close
* Output		: None
* Return		: 0 succes	 -1 Fail
* Auther		: lhy
* Date		: 2018.08.21
*****************************************************************************/	
int exec_cusbadb_cmd(bool value);

#endif



