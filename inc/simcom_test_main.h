#ifndef _SIMCOM_TEST_MAIN_H_
#define _SIMCOM_TEST_MAIN_H_
#include "simcom_common.h"


extern call_info_type call_info_msg;

#ifdef SIMCOM_TEST_UI
void simcom_test_main();
void simcom_voice_test_ind_handler(void *usrdata);
#endif
#endif


