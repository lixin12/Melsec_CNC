#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include "simcom_common.h"
#include "qmi_client.h"
#include "wireless_messaging_service_v01.h"

/////////////////////////////////////////////////////////////
// MACRO CONSTANTS DEFINITIONS
///////////////////////////////////////////////////////////// 
#define TRUE   1
#define FALSE  0

#define SMS_MAX_ADDRESS_LENGTH  48
#define SMS_MAX_CONTENT_LENGTH  255

#define LOG printf

/////////////////////////////////////////////////////////////
// ENUM TYPE DEFINITIONS
/////////////////////////////////////////////////////////////
typedef enum
{
	SMS_MSG_FORMAT_TEXT_NULL,
	SMS_MSG_FORMAT_TEXT_ASCII = 1,
	SMS_MSG_FORMAT_TEXT_UTF8 = 2,
	SMS_MSG_FORMAT_TEXT_UNICODE = 3,
	SMS_MSG_FORMAT_TEXT_MAX
}sms_format;

/////////////////////////////////////////////////////////////
// STRUCT TYPE DEFINITIONS
/////////////////////////////////////////////////////////////
#if 0
/*this struct is old */
typedef struct {
  uint8 format;
  char message_content[256];
  char source_address[253];
}sms_info_type;
#endif

typedef unsigned char uint8;
typedef unsigned char boolean;

typedef struct
{
	boolean     is_concate;
	uint8         msg_ref;
	uint8         seq_num;
	uint8         total_sm;
	sms_format         format;
	int            message_len;
	char         message_content[SMS_MAX_CONTENT_LENGTH];
	char         source_address[SMS_MAX_ADDRESS_LENGTH];
}sms_info_type;

typedef void (*sms_ind_cb_fcn)(simcom_event_e simcom_event, sms_info_type *sms_info);

/////////////////////////////////////////////////////////////
// GLOBAL DATA DEFINITION
/////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS
/////////////////////////////////////////////////////////////

/*****************************************************************************
 * FUNCTION
 *  sms_init
 *
 * DESCRIPTION
 *  init sms 
 *  
 * PARAMETERS
 *  sms_ind_cb_fcn: register call_back to process received sms
 *
 * RETURNS
 *  -1, FAIL
 *  0, SUCCESS
 *
 * NOTE
 *  
 *****************************************************************************/
int sms_init(sms_ind_cb_fcn sms_ind_cb);

/*****************************************************************************
 * FUNCTION
 *  sms_deinit
 *
 * DESCRIPTION
 *  release sms
 *  
 * PARAMETERS
 *  VOID
 *
 * RETURNS
 *  VOID
 *
 * NOTE
 *  if quit sms ,call this
 *****************************************************************************/
void sms_deinit();

/*****************************************************************************
 * FUNCTION
 *  sms_set_format
 *
 * DESCRIPTION
 *  set received chinese sms output format. defalut English sms output format ASCII.
 *  chinese sms output format UTF8, if you want to output UCS2 format. pls call the fuction.
 *  
 * PARAMETERS
 *  [IN] format:   1: not use 2:UTF8(default)  3:UCS2
 *
 * RETURNS
 *  VOID
 *
 * NOTE
 * only for chinese sms 
 *****************************************************************************/
void sms_set_format(sms_format format);

/*****************************************************************************
 * FUNCTION
 *  send_message
 *
 * DESCRIPTION
 *  send sms api
 
 * PARAMETERS
 *  smsMode:                  1:ASCII  2:UTF8 3:UCS2
 *  phoneNumber:            destination address
 *  content:                     send message content
 *  content_len:               send message length
 *
 * RETURNS
 *  -1, FAIL
 *  0, SUCCESS
 *
 * NOTE
 *  
 *****************************************************************************/
int send_message(int smsMode, char *phoneNumber, unsigned char *content, int content_len);

#endif
