#ifndef _DATA_CALL_H_
#define _DATA_CALL_H_
#include <math.h>
#include "simcom_common.h"
#include "dsi_netctrl.h"
#include "DnsResolv.h"
#define MAX_DATACALL_NUM    4
#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

typedef enum{
    DATACALL_DISCONNECTED=0,
    DATACALL_CONNECTED
}datacall_status_type;

typedef unsigned char boolean;

typedef enum 
{
  app_tech_min = 0,
  app_tech_umts = app_tech_min,
  app_tech_cdma,
  app_tech_1x,
  app_tech_do,
  app_tech_lte,
  app_tech_auto,
  app_tech_max
}app_tech_e;

typedef enum 
{
  app_call_status_idle,
  app_call_status_connecting,
  app_call_status_connected,
  app_call_status_disconnecting
}app_call_status_e;

typedef struct {
  dsi_hndl_t handle;
  app_tech_e tech;
  int family;
  int profile;
  app_call_status_e call_status;
} app_call_info_t;


typedef struct {
  dsi_hndl_t handle;
  app_tech_e tech;
  int family;
  int profile;
  datacall_status_type status;
  char if_name[32];
  char ip_str[16];
  char pri_dns_str[16];
  char sec_dns_str[16];
  char gw_str[16];
  unsigned int mask;
  dsi_ce_reason_t end_reason; 
}datacall_info_type;




extern int dataCall_init();
//extern int query_ip_from_dns(char *url, char *pri_dns_ip, char *sec_dns_ip, char *ip);
extern int set_host_route(char *old_ip, char *new_ip, char *if_name);

int datacall_init();
int datacall_deinit();
int get_datacall_info_by_profile(int profile, datacall_info_type *pcallinfo);
int get_datacall_info_by_handle(dsi_hndl_t handle, datacall_info_type *pcallinfo);
int start_dataCall(app_tech_e tech, int ip_family, int profile, char *apn, char *username, char *password);
int stop_dataCall(int profile);

#endif

