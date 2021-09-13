#ifndef _WDS_CONTROL_H_
#define _WDS_CONTROL_H_
#include "qmi_client.h"
#include "wireless_data_service_v01.h"
#include "simcom_common.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef unsigned char boolean;
typedef unsigned char uint8;

int wds_init();
void wds_deinit();
int get_apnInfo(int profile_index, int* pdp_type, char* apn_str, char *username, char *password);
int set_apnInfo(int profile_index, int pdp_type, char* apn_str, char *username, char *password);

#endif


