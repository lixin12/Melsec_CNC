#ifndef _SIMCOM_COMMON_H
#define _SIMCOM_COMMON_H
#include "qmi_client.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DEBUG
	#define DEBUG 1
#endif

#if DEBUG
    #define DEBUGLOG(format,...) printf("### SYSLOG ### %s,%s[%d] "format"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
	#define DEBUG_NO(format,...) printf(format,##__VA_ARGS__)
#else
	#define DEBUGLOG(format,...)
	#define DEBUG_NO(format,...)
#endif

#ifndef __int8_t_defined
# define __int8_t_defined
typedef signed char     int8_t;
typedef short int       int16_t;
typedef int             int32_t;
# if __WORDSIZE == 64
typedef long int        int64_t;
# else
__extension__
typedef long long int   int64_t;
# endif
#endif

/* Unsigned. */
typedef unsigned char       uint8_t;
typedef unsigned short int  uint16_t;
#ifndef __uint32_t_defined
typedef unsigned int        uint32_t;
# define __uint32_t_defined
#endif
#if __WORDSIZE == 64
typedef unsigned long int   uint64_t;
#else
__extension__
typedef unsigned long long int uint64_t;
#endif

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef uint8_t  boolean;

#define TRUE  1
#define FALSE 0

#define EXIT_SUCCESS 0
#define EXIT_FAIL -1


#define FEATURE_JSON_SDK
#define FEATURE_NETWORK_SDK

typedef enum
{
    SIMCOM_EVENT_SERVING_SYSTEM_IND = 0,
    SIMCOM_EVENT_SMS_PP_IND,
    SIMCOM_EVENT_VOICE_CALL_IND,
    SIMCOM_EVENT_VOICE_RECORD_IND,
    SIMCOM_EVENT_NETWORK_IND,
    SIMCOM_EVENT_DATACALL_CONNECTED_IND,
    SIMCOM_EVENT_DATACALL_DISCONNECTED_IND,
    SIMCOM_EVENT_NMEA_IND,
    SIMCOM_EVENT_LOC_IND,
    SIMCOM_EVENT_MAX,
    SIMCOM_EVENT_AUDIO
}simcom_event_e;


#define SIMCOM_TEST_UI

/*************************************************************
 *  input 
 *          --scan_string                     :input string
 *          --nlen_scan_string                :input string length  
 *          --nMaxLen                         :max len
 *          --nMinLen                         :min len  
 *  output
 *          --out_str                         :output string
  
 * ret
 * -1 len too short
 * 1 len too long
 * 2 error
 *************************************************************/
int simcom_handle_input_string(char* scan_string, int nlen_scan_string, char* out_str, int nMaxLen, int nMinLen);

/*************************************************************
 *  input 
 *          --scan_string                     :input string
 *          --nlen_scan_string            :input string length  
 * ret
 * 0xFFFF error
 * number  success
 *************************************************************/
int simcom_handle_input_number(char* scan_string, int nlen_scan_string);

void simcom_fgets(char *scan_string, uint32_t scan_len);


void process_simcom_ind_message(simcom_event_e event,void *cb_usr_data);
#ifdef SIMCOM_TEST_UI
//void print_option_menu(const char* options_list[], int array_size);
void simcom_test_main();

#endif

#endif /* _SIMCOM_COMMON_H */
