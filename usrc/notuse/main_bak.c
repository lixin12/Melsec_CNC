#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include "ATControl.h"
#include "OTAUpgrade.h"
#include "LedControl.h"
#include "VoiceCall.h"
#include "simcom_common.h"
#include "WiFiControl.h"
#include "SMSControl.h"
#include "DataCall.h"
#include "WDSControl.h"
#include "NASControl.h"
#include "GpioControl.h"
#include "GPSControl.h"
#include "simcom_test_main.h"

#define APN1  "3gnet"
#define APN2  "3gwap"

char target_ip[16] ={0};
static int  IS_USE_URL = 0;

void process_simcom_ind_message(simcom_event_e event,void *cb_usr_data)
{
    int i;
    switch(event)
    {
        case SIMCOM_EVENT_VOICE_CALL_IND:
            {
                memcpy((void *)&call_info_msg, cb_usr_data, sizeof(call_info_msg));
                for(i = 0; i < call_info_msg.call_num; i++)
                {
                    if (call_info_msg.call_info[i].call_id != 0 && call_info_msg.call_info[i].direction == CALL_DIRECTION_MO_V02) 
                    {
                        switch(call_info_msg.call_info[i].call_state)
                        {
                            case CALL_STATE_CC_IN_PROGRESS_V02:
                                printf("MO Ready to dial\n");
                                break;
                            case CALL_STATE_ORIGINATING_V02:
                                printf("MO Dialing...\n");
                                break;
                            case CALL_STATE_ALERTING_V02:
                                printf("MO Alerting...\n");
                                break;
                            case  CALL_STATE_DISCONNECTING_V02:
                                printf("MO call handing up\n");
                                break;
                            case CALL_STATE_END_V02:
                                call_info_msg.call_info[i].call_id = 0;
                                printf("MO call hung up\n");
                                break;
                            case CALL_STATE_CONVERSATION_V02:
                                printf("MO Call established\n");
                                break;
                            default:
                                printf("MO unprocess call_state %d\n", call_info_msg.call_info[0].call_state);
                                break;
                        }
                    }
                    else if (call_info_msg.call_info[i].call_id != 0 && call_info_msg.call_info[i].direction == CALL_DIRECTION_MT_V02) 
                    {
                        switch(call_info_msg.call_info[i].call_state)
                        {
                            case  CALL_STATE_DISCONNECTING_V02:
                                printf("MT call handing up\n");
                                break;
                            case CALL_STATE_END_V02:
                                call_info_msg.call_info[i].call_id = 0;
                                printf("MT call hung up\n");
                                break;
                            case CALL_STATE_CONVERSATION_V02:
                                printf("MT Call established\n");
                                break;
                            case CALL_STATE_INCOMING_V02:
                                printf("MT Incoming call %s\n", call_info_msg.call_info[i].phone_number);
                                break;
                            case CALL_STATE_WAITING_V02:
                                printf("MT New Waiting call %s waiting\n", call_info_msg.call_info[i].phone_number);
                                break;
                            default:
                                printf("MT unprocess mt call_state %d\n", call_info_msg.call_info[0].call_state);
                                break;

                        }
                    }
                    else
                    {
                        printf(">>>>>>voice callback:\n");
                        printf("index[%d], call_id=%d, state=%d, direction = %d, number=%s\n",
                                  i,
                                  call_info_msg.call_info[i].call_id,
                                  call_info_msg.call_info[i].call_state,
                                  call_info_msg.call_info[i].direction,
                                  strlen(call_info_msg.call_info[i].phone_number) >= 1?call_info_msg.call_info[i].phone_number:"unkonw"
                                  );
                        printf("<<<<<<<<<<< \n");
                    }
                }  
                #ifdef SIMCOM_TEST_UI
                    //simcom_voice_test_ind_handler(&call_info_msg);
                #endif
            }
            break;

        case SIMCOM_EVENT_SMS_PP_IND:
            {
                sms_info_type sms_info;
		  int i;
                memcpy((void *)&sms_info, cb_usr_data, sizeof(sms_info));
                
                printf("\n-----------receive message ---------------------\n");
                printf("is_concate = %d\n", sms_info.is_concate);
                printf("msg_ref = %d\n", sms_info.msg_ref);
                printf("seq_num = %d\n", sms_info.seq_num);
		  printf("total_sm = %d\n", sms_info.total_sm);
                printf("format = %d\n", sms_info.format);
                printf("message_len = %d\n", sms_info.message_len);
                printf("address=%s\n",sms_info.source_address);
		  if (sms_info.format == SMS_MSG_FORMAT_TEXT_UNICODE)
		  {
		  	printf("content:");
			for (i=0; i<sms_info.message_len; i++)
				printf("%02X", sms_info.message_content[i]);
			printf("\n");
		  }
		  else
                	printf("content:%s\n", sms_info.message_content);
            }
            break;

        case SIMCOM_EVENT_NETWORK_IND:
            {
                network_info_type network_info;
                memcpy((void *)&network_info, cb_usr_data, sizeof(network_info));
                printf("\n---------network info---------------------------\n");
                printf("network_info: register=%d, cs=%d, ps=%d,radio_if=%d\n",
                        network_info.registration_state,
                        network_info.cs_attach_state,
                        network_info.ps_attach_state,
                        network_info.radio_if_type);
                if(network_info.registration_state == NAS_REGISTERED_V01)
                {
                    //lte_led_blink(500,500);
                }
                if(network_info.registration_state != NAS_REGISTERED_V01)
                {
                    //lte_led_on();
                }
            }
            break;
        case SIMCOM_EVENT_DATACALL_CONNECTED_IND:
            {
                int ret;
                char target_ip_new[16] = {0};
                datacall_info_type datacall_info;
                dsi_hndl_t handle = (dsi_hndl_t)cb_usr_data;
   
                get_datacall_info_by_handle(handle, &datacall_info);
                if(datacall_info.status == DATACALL_CONNECTED)
                {             
                    printf("datacall_ind1: profile_index=%d\n", 
                                          datacall_info.profile);  
                    printf("datacall_ind2: if_name=%s,ip=%s,mask=%d\n", 
                                          datacall_info.if_name,
                                          datacall_info.ip_str,
                                          datacall_info.mask);
                    printf("datacall_ind3: dns1=%s,dns2=%s,gw=%s\n", 
                                          datacall_info.pri_dns_str,
                                          datacall_info.sec_dns_str,
                                          datacall_info.gw_str); 
                    
                    /*****************************************************************************
                    * 注意：
                    * 如果服务器的地址是URL 而不是IP,需要通过专网的DNS进行解析，并设置路由。
                    * 如果提供的是IP，则直接设置路由
                    ********************************************************************************/               
                    if(IS_USE_URL)
                    {
                        ret = query_ip_from_dns((unsigned char *)"www.baidu.com", datacall_info.pri_dns_str ,datacall_info.pri_dns_str , target_ip_new);
                        if(ret != 0)
                        {
                            printf("query ip fail\n");
                            break;
                        }

                        printf("target_ip:%s\n",target_ip_new);

                        set_host_route(target_ip, target_ip_new, datacall_info.if_name);

                        strncpy(target_ip, target_ip_new, sizeof(target_ip_new));
                    }
                    else
                    {
                        set_host_route(target_ip, target_ip, datacall_info.if_name);
                    }
                }
            }
            break;
        case SIMCOM_EVENT_DATACALL_DISCONNECTED_IND: 
            {
                dsi_ce_reason_t *pend_reason = (dsi_ce_reason_t *)cb_usr_data;
                printf("disconnect ind\n");
                printf("reason_type=%d, reason_code=%d\n", pend_reason->reason_type, pend_reason->reason_code);
            }    
            break;
        case SIMCOM_EVENT_NMEA_IND:
            printf((char *)cb_usr_data);
            break;
        case SIMCOM_EVENT_AUDIO:
            printf("%d\n",*(char *)cb_usr_data);
            printf("audioaudio\n");
            break;   
        case SIMCOM_EVENT_LOC_IND:
            {
                GpsInfo *location = (GpsInfo *)cb_usr_data;
                printf("\nLATITUDE:  %f\nLONGITUDE: %f\nALTITUDE:  %f\nSPEED:     %f\nTIME:      %02d-%02d-%02d %02d:%02d:%02d\n",
                         location->latitude, location->longitude, location->altitude,location->speed, location->time[0], location->time[1],location->time[2],location->time[3],location->time[4],location->time[5]);
            }
            break;           
        default:
            break;
    }

}


#ifdef SIMCOM_TEST_UI
int main(int argc,char* argv[]) 
{
    printf("SDK_VER : %s\n",get_simcom_sdk_version());
    printf("DEMO_VER: %s\n",get_simcom_sdk_version());
    simcom_test_main();
    return 0;
}
#else

int main(int argc,char* argv[]) 
{
	return 0;
}
#endif
