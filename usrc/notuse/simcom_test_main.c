#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <ctype.h>
#include "simcom_common.h"
#include "VoiceCall.h"
#include "DataCall.h"
#include "WDSControl.h"
#include "NASControl.h"
#include "WiFiControl.h"
#include "SMSControl.h"
#include "ATControl.h"
#include "UIMControl.h"
#include "DMSControl.h"
#include "OTAUpgrade.h"
#include "BTControl.h"
#include "TTSControl.h"
#ifdef _SIMCOM_WINDOWS_
#include "app_timer.h"
#else
#include "LinuxTimer.h"
#endif
#include "ADC.h"
#include "I2C.h"
#include "GPSControl.h"
#include "GpioControl.h"
#include "EthControl.h"
#include "simcom_demo_version.h"
#include "simcom_test_main.h"
#include "FTPControl.h"
#include "ALSAControl.h"
#include "DEVControl.h"
#include "ftp_s.h"
#include "http_s.h"
#include "mqtt_s.h"
#include "openssl.h"
#include "simcom_lbs.h"

#define SIG SIGALRM

#ifdef SIMCOM_TEST_UI
static pthread_mutex_t lock;
static pthread_cond_t cond;
static simcom_event_e event;

static  bt_msg      s_bt_msg;

call_info_type call_info_msg;



static pthread_mutex_t s_btmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  s_btcond = PTHREAD_COND_INITIALIZER;
 
static void simcom_wifi_test();
static void simcom_nas_test();
static void simcom_wds_test();
static void simcom_voice_test();
static void simcom_datacall_test();
static void simcom_sms_test();
static void simcom_ota_test();
static void simcom_at_test();
static void simcom_tts_test();
static void simcom_gpio_test();
static void simcom_gps_test();
static void simcom_bt_test();
static void simcom_timer_test();
static void simcom_tcp_test();
static void simcom_adc_test();
static void simcom_uim_test();
static void simcom_dms_test();
static void simcom_i2c_test();
static void simcom_uart_test(); 
static void simcom_spi_test();
static void simcom_get_version();
static void simcom_eth_test();
static void simcom_ftp_test();
#ifdef FEATURE_NETWORK_SDK
static void simcom_ssl_test();
static void simcom_http_s_test();
static void simcom_ftp_s_test();
static void simcom_mqtt_s_test();
#endif
static void simcom_alsa_test();
static void simcom_dev_test(void);
static void simcom_audio_test();
#ifdef FEATURE_JSON_SDK
static void simcom_json_test();
#endif

void print_option_menu(const char* options_list[], int array_size)
{
    int i,j;
    for (i=0; i<(array_size/2); i++)
    {
        printf("%s",options_list[2*i]);
        for(j=30-strlen(options_list[2*i]); j>0; j--)
        {
            printf(" ");
        }
        printf("%s\n",options_list[2*i+1]);
    }

    if(array_size%2 != 0)
    {
        printf("%s\n",options_list[array_size -1]);
    }
}

void simcom_test_main() 
{
    int opt = 0;
    int i,j;
    char scan_string[100] = {0};
    const char *options_list[] = {
        "1. WIFI",
        "2. VOICE CALL",
        "3. DATA CALL",
        "4. SMS",
        "5. WDS(APN)",
        "6. NAS",
        "7. AT",
        "8. OTA",
        "9. TTS",
        "10. GPIO",
        "11. GPS",
        "12. Bluetooth",
        "13. TCP/UDP",
        "14. Timer",
        "15. ADC",
        "16. I2C",
        "17. UIM(SimCard)",
        "18. DMS(IMEI,MEID)",
        "19. UART",
        "20. SPI",
        "21. Version",
        "22. Ethernet",
        "23. FTP",
        "24. SSL",
        "25. HTTP(S)",
        "26. FTP(S)",
        "27. MQTT(S)",
        "28. ALSA",
        "29. DEV",
        "30. AUDIO",
        "31. JSON",
        "32. LBS",
        "99. EXIT",
    };

    int array_size = sizeof(options_list)/sizeof(options_list[0]);
    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                simcom_wifi_test();
                break;
            case 2:
                simcom_voice_test();
                break;
            case 3:
                simcom_datacall_test();
                break;
            case 4:
                simcom_sms_test();
                break;
            case 5:
                simcom_wds_test();
                break;
            case 6:
                simcom_nas_test();
                break;
            case 7:
                simcom_at_test();
                break;
            case 8:
                simcom_ota_test();
                break;
            case 9:
                simcom_tts_test();
                break;
            case 10:
                simcom_gpio_test();
                break;
            case 11:
                simcom_gps_test();
                break;
            case 12:
                simcom_bt_test();
                break;
            case 13:
                simcom_tcp_test();
                break;
            case 14:
                simcom_timer_test();
                break;
            case 15:
                simcom_adc_test();
                break;
            case 16:
                simcom_i2c_test();
                break;
            case 17:
                simcom_uim_test();
                break;
            case 18:
                simcom_dms_test();
                break;
            case 19:
                simcom_uart_test();
                break;
            case 20:
                simcom_spi_test();
                break;
            case 21:
                simcom_get_version();
                break;
            case 22:
                simcom_eth_test();
                break;
            case 23:
                simcom_ftp_test();
                break;
#ifdef FEATURE_NETWORK_SDK
            case 24:
                simcom_ssl_test();
                break;
            case 25:
                simcom_http_s_test();
                break;
            case 26:
                simcom_ftp_s_test();
                break;
            case 27:
                simcom_mqtt_s_test();
                break;
#endif
            case 28:
                simcom_alsa_test();
                break;
            case 29:
                simcom_dev_test();
                break;
            case 30:
                simcom_audio_test();
                break;
#ifdef FEATURE_JSON_SDK
            case 31:
                simcom_json_test();
#else
                printf("the function is debuging...\n");
#endif              
                break;
            case 32:
                simcom_lbs_test();
                break;
            case 99:
                exit(0);
            default:
                break;
        }
    }
}

static void simcom_alsa_test()
{
    const char *options_list[] = {
        "1. set clvl",
        "2. get clvl",
        "3. set mic gain",
        "4. get mic gain",
        "5. set csdvc",
        "6. get csdvc",
        "7. set codecctrl",
        "8. get codecctrl",
        "99. back",
    };

    int ret;
    int opt = 0;
    int i,j;
    char scan_string[100] = {0};

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
            {
                int clvl = 0;
                printf("\nPlease input clvl value\n");
                printf("0-5\n");
                memset(scan_string, 0, sizeof(scan_string));
                if(fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    continue;

                clvl = atoi(scan_string);

                if((strlen(scan_string) != 2) || clvl < 0 || clvl > 5){
                  printf("error: please input 0-5 !\n");
                  break;    
                }

                ret = set_clvl_value(clvl);
                if(ret != 0)
                {
                    printf("set clvl fail!\n");
                    break;
                }

            }
            break;

            case 2:
            {
                int clvl = 0;
                clvl = get_clvl_value();
                if(clvl < 0)
                {
                    printf("get clvl fail!\n");
                    break;
                }

                printf("clvl %d.\n", clvl);
            }
            break;

            case 3:
            {
                int micgain = 0;
                printf("\nPlease input mic gain value\n");	
                printf("0-8\n");
                memset(scan_string, 0, sizeof(scan_string));
                if(fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    continue;

                micgain = atoi(scan_string);

                if((strlen(scan_string) != 2) || micgain < 0 || micgain > 8){
                  printf("error: please input 0-8.\n");
                  break;    
                }
                
                ret = set_micgain_value(micgain);
                if(ret != 0)
                {
                    printf("set micgain fail!\n");
                    break;
                }

            }
            break;

            case 4:
            {
                int micgain = 0;
                micgain = get_micgain_value();
                if(micgain < 0)
                {
                    printf("get micgain fail!\n");
                    break;
                }

                printf("micgain %d.\n", micgain);
            }
            break;

            case 5:
            {
                int csdvc = 0;
                printf("\nPlease input csdvc value\n");
                printf("0/1/3\n");
                memset(scan_string, 0, sizeof(scan_string));
                if(fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    continue;

                csdvc = atoi(scan_string);

                if(csdvc != 0 && csdvc != 1 && csdvc != 3){
                  printf("error: please input 0/1/3.\n");
                  break;    
                }

                ret = set_csdvc_value(csdvc);
                if(ret != 0)
                {
                    printf("set csdvc fail!\n");
                    break;
                }

            }
            break;

            case 6:
            {
                int csdvc = 0;
                csdvc = get_csdvc_value();
                if(csdvc < 0)
                {
                    printf("get csdvc fail!\n");
                    break;
                }

                printf("csdvc %d.\n", csdvc);
            }
            break;
           case 7:
           {
                int val_ctl = 0;
                printf("\nPlease input val_ctl\n");
                printf("0 or 1\n");
                memset(scan_string, 0, sizeof(scan_string));
                if(fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    continue;

                val_ctl = atoi(scan_string);

                if((strlen(scan_string) != 2) || val_ctl < 0 || val_ctl > 1){
                  printf("error: please input 0 or 1 !\n");
                  break;    
                }

                ret =set_codec_ctrl(val_ctl);
                if(ret == -1)
                {
                    printf("set val_ctl fail!\n");
                    break;
                }
           }
           break;
          case 8:
          {
               	int val_ctl = 0;
                val_ctl = get_codec_ctrl();
                if(val_ctl == -1)
                {
                    printf("get val_ctl fail!\n");
                    break;
                }

                printf("val_ctl = %d\n", val_ctl);
          }
          break;		
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

}


static void simcom_dev_test(void)
{
    const char *options_list[] = {
        "1. exec cdelta cmd",
        "2. exec cusbadb cmd",
        "99. back",
    };

    int ret;
    int opt = 0;
    int i,j;
    char scan_string[100] = {0};

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
            {
                //printf("\nPlease input ota path\n");	
                //memset(scan_string, 0, sizeof(scan_string));
                //if(fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                //continue;
                
                ret = exec_cdelta_cmd("/cache/update_ota.zip");
                if(ret != 0)
                {
                    printf("cdelta fail!\n");
                    break;
                }

            }
            break;

            case 2:
            {
                int adb = 0;
                printf("\nPlease input cusbadb value\n");
                printf("0:close  1:open\n");
                memset(scan_string, 0, sizeof(scan_string));
                if(fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    continue;

                adb = atoi(scan_string);


                ret = exec_cusbadb_cmd(adb);
                if(ret != 0)
                {
                    printf("cusbadb fail!\n");
                    break;
                }
            }
            break;
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

}


static void simcom_datacall_test()
{
    const char *options_list[] = {
        "1. start data call",
        "2. stop data call",
        "3. get call Info",
        "4. set default route and dns server",
        "99. back",
    };

    int opt = 0;
    int i,j;
    char scan_string[200] = {0};
    dsi_hndl_t hhhandle;
    if(datacall_init() != 0)
    {
        return;
    }

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1: /* start data call*/
                {
                    int     profile;
                    int     tech;
                    char    apn[QMI_WDS_APN_NAME_MAX_V01 + 1] = {0};
                    char    username[QMI_WDS_USER_NAME_MAX_V01 + 1] = {0};
                    char    password[QMI_WDS_PASSWORD_MAX_V01 + 1] = {0};  
                    char    *papn = NULL;
                    char    *pusername = NULL;
                    char    *ppassword = NULL;
                    printf("\nPlease input profile index:\n");             
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                    profile = atoi(scan_string);
                    if(profile > 16 || profile == 6)     //Profile = 1 for system. profile=6 for wifi
                    {
                        printf("please select other profile.\n");
                        break;
                    }
                    
                    printf("\nPlease input tech:\n");    
                    printf("0: umts 1: cdma 2: 1x 3: do  4: lte  5: auto (default 5)\n");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                        
                    for (i = 0; i <= strlen(scan_string) -1 ; i++){
                        if((scan_string[i] == 0x0A) || (scan_string[i] == 0x0D)){
                            scan_string[i] = '\0';
                            break;
                        }
                    }        
                    if(strlen(scan_string) == 0)
                    {
                        tech = app_tech_auto;
                    }
                    else
                    {
                        tech = atoi(scan_string);
                        if(tech > 5)
                        {
                            tech = 5;
                        }
                    }

                    //--------------------------- apn -----------------------------------------
                    printf("\nPlease input profile apn(if no need, press Enter key):\n");             
                    
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                        
                    for (i = 0; i <= strlen(scan_string) -1 ; i++){
                        if((scan_string[i] == 0x0A) || (scan_string[i] == 0x0D)){
                            scan_string[i] = '\0';
                            break;
                        }
                    }        
                    if(strlen(scan_string) > 0 && strlen(scan_string) <= QMI_WDS_APN_NAME_MAX_V01)
                    {
                        memcpy(apn, scan_string, strlen(scan_string));
                        papn = apn;
                    }

                    //--------------------------- username -------------------------------------    
                    printf("\nPlease input profile username(if no need, press Enter key):\n");             
                    
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                        
                    for (i = 0; i <= strlen(scan_string) -1 ; i++){
                        if((scan_string[i] == 0x0A) || (scan_string[i] == 0x0D)){
                            scan_string[i] = '\0';
                            break;
                        }
                    }        
                    if(strlen(scan_string) > 0 && strlen(scan_string) <= QMI_WDS_USER_NAME_MAX_V01)
                    {
                        memcpy(username, scan_string, strlen(scan_string));
                        pusername = username;
                    }


                    //--------------------------- password -------------------------------------   
                    printf("\nPlease input profile password(if no need, press Enter key):\n");
                    
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                        
                    for (i = 0; i <= strlen(scan_string) -1 ; i++){
                        if((scan_string[i] == 0x0A) || (scan_string[i] == 0x0D)){
                            scan_string[i] = '\0';
                            break;
                        }
                    }
                    
                    if(strlen(scan_string) > 0 && strlen(scan_string) <= QMI_WDS_PASSWORD_MAX_V01)
                    {
                        memcpy(password, scan_string, strlen(scan_string));
                        ppassword = password;
                    }                      

                    
                    start_dataCall(tech, DSI_IP_VERSION_4, profile, papn, pusername, ppassword);
                    
                }
                break;
            case 2: /*stop data call*/
                {
                    int profile;
                    printf("\nPlease input profile index:\n");             
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                    profile = atoi(scan_string);
                    if(profile <= 1 || profile > 16 || profile == 6)     //Profile = 1 for system. profile=6 for wifi
                    {
                        printf("please select other profile.\n");
                        break;
                    }
                    stop_dataCall(profile);
                }
                break;
            case 3:
                {
                    /* get data call infomation*/
                    int profile;
                    int ret;
                    datacall_info_type callinfo;
                    printf("\nPlease input profile index:\n");             
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                    profile = atoi(scan_string);
                    
                    ret = get_datacall_info_by_profile(profile,&callinfo);
                    if(ret == 0)
                    {
                        hhhandle = callinfo.handle;
                        printf("DC: profile=%d\n", callinfo.profile);
                        printf("DC: handle=%d\n", callinfo.handle);
                        printf("DC: if_name=%s\n", callinfo.if_name);
                        printf("DC: ip_str=%s\n", callinfo.ip_str);
                        printf("DC: mask=%d\n", callinfo.mask);
                        printf("DC: pri_dns_str=%s\n", callinfo.pri_dns_str);
                        printf("DC: sec_dns_str=%s\n", callinfo.sec_dns_str);
                        printf("DC: handle=%d\n", callinfo.status);
                    }
                }
                break;
            case 4:
                {
                    /* set route and dns*/
                    int profile;
                    int ret;
                    datacall_info_type callinfo;
                    char command[50];
                    printf("\nPlease input profile index for the default route and dns server:\n");             
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                    profile = atoi(scan_string);
                    
                    ret = get_datacall_info_by_profile(profile,&callinfo);
                    if(ret == 0)
                    {
                        hhhandle = callinfo.handle;
                        printf("DC: if_name=%s\n", callinfo.if_name);
                        sprintf(command, "route add default dev %s", callinfo.if_name);
                        system(command);

                        if(strlen(callinfo.pri_dns_str) > 0)
                        {
                            printf("DC: pri_dns_str=%s\n", callinfo.pri_dns_str);
                            sprintf(command, "echo 'nameserver %s' > /etc/resolv.conf", callinfo.pri_dns_str);
                            system(command);
                        }

                        if(strlen(callinfo.sec_dns_str) > 0)
                        {
                            printf("DC: sec_dns_str=%s\n", callinfo.sec_dns_str);
                            sprintf(command, "echo 'nameserver %s' >> /etc/resolv.conf", callinfo.sec_dns_str);
                            system(command);
                        }
                        
                        printf("Set SUCCESS\n");
                    }
                    else
                    {
                        printf("Please start data call first.\n");
                    }
                }
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

    datacall_deinit();
}


static void simcom_wds_test()
{
    const char *options_list[] = {
        "1. get apn Info",
        "2. set apn Info",
        "99. back",
    };

    int opt = 0;
    int i,j;
    char scan_string[100] = {0};
    if(wds_init() != 0)
    {
        return;
    }

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                {
                    /* get APN infomation*/
                    int profile_index;
                    int pdp_type;
                    int ret;
                    char apn_str[128]={0};
                    char username[128]={0};
                    char password[128]={0};

                    printf("\nPlease input profile_index:");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;     
                    profile_index = atoi(scan_string);
                    ret = get_apnInfo(profile_index, &pdp_type, apn_str, username, password);

                    printf("WDS: ==============================================\n");
                    if(ret == 0)
                    {
                        printf("WDS: profile_index=%d\n", profile_index);
                        printf("WDS: apn=%s\n", apn_str);
                        printf("WDS: pdp_type=%d\n", pdp_type);
                        printf("WDS: username=%s\n", username);
                        printf("WDS: password=%s\n", password);
                    }else{
                        printf("\nProfile_index error !\n");
                    }
                }
                break;
            case 2:
                {
                    /* set APN infomation*/
                    int level, mode;
                    int profile_index;
                    int pdp_type;
                    char apn_str[128]={0};
                    char username[128]={0};
                    char password[128]={0};

                    int ret;

                    int i = 0 ;

                    printf("\nPlease input profile_index:");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;  
                    profile_index = atoi(scan_string);

                    memset(scan_string, 0, sizeof(scan_string));
                    printf("\nPlease input pdp_type:(0:IP 1:PPP 2:IPV6 3:IPV4V6)>");
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                    
                    pdp_type = atoi(scan_string);
                    
                    if((pdp_type > 3) || (pdp_type < 0)) {                      
                      printf("\nPlease enter the correct pdp_type:(0:IP 1:PPP 2:IPV6 3:IPV4V6)\n");
                      break;
                    }

                    printf("\nPlease input apn:");
                    if (fgets(apn_str, sizeof(apn_str), stdin) == NULL)
                        break;

                    for (i = 0; i <= strlen(apn_str) -1 ; i++){
                      if((apn_str[i] == 0x0A) || (apn_str[i] == 0x0D)){
                        apn_str[i] = '\0';
                        break;
                      }
                    }

                    printf("\nPlease input username:");
                    if (fgets(username, sizeof(username), stdin) == NULL)
                        break;

                    for (i = 0; i <= strlen(username) -1 ; i++){
                        if((username[i] == 0x0A) || (username[i] == 0x0D)){
                            username[i] = '\0';
                            break;
                        }
                    }

                    printf("\nPlease input password:");
                    if (fgets(password, sizeof(password), stdin) == NULL)
                        break;

                    for (i = 0; i <= strlen(password) -1 ; i++){
                        if((password[i] == 0x0A) || (password[i] == 0x0D)){
                            password[i] = '\0';
                            break;
                        }
                    }

                    ret = set_apnInfo(profile_index, pdp_type, apn_str, username, password);

                    printf("WDS =====================================\n");
                    if(ret == 0)
                    {
                        printf("apn set success!\n");
                    }
                    else
                    {
                        printf("apn set fail!\n");
                    }

                }
            case 99:
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

    wds_deinit();
}

static int simcomvoice_get_call(uint8_t type, uint8_t *call_id)
{
    int ret = -1;
    int i;
    if (type == 0) //MO
    {
        for (i=0; i<call_info_msg.call_num; i++)
        {
            if (call_info_msg.call_info[i].call_id != 0 && call_info_msg.call_info[i].direction == CALL_DIRECTION_MO_V02)
            {
                *call_id = call_info_msg.call_info[i].call_id;
                ret = 0;
                break;
            }
        }
    }
    if (type == 1) //MT
    {
        for (i=0; i<call_info_msg.call_num; i++)
        {
            if (call_info_msg.call_info[i].call_id != 0 && call_info_msg.call_info[i].direction == CALL_DIRECTION_MT_V02)
            {
                *call_id = call_info_msg.call_info[i].call_id;
                ret = 0;
                break;
            }
        }
    }
    if (type == 2) //END
    {
        for (i=0; i<call_info_msg.call_num; i++)
        {
            if (call_info_msg.call_info[i].call_id != 0)
            {
                *call_id = call_info_msg.call_info[i].call_id;
                ret = 0;
                break;
            }
        }
    }
    return ret;
}


static void simcom_voice_test()
{
    const char *options_list[] = {
        "1. Dial",
        "2. Answer",
        "3. End",
        "99. Back",
    };

    int opt = 0;
    char scan_string[100] = {0};
    uint8_t call_id;

    if(voice_init() != 0)
    {
        return;
    }

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1: //DIAL
                {
                    if (simcomvoice_get_call(0, &call_id) == 0)
                    {
                        printf("Now is calling,Please waiting...\n");
                        break;
                    }
                    printf("\nPlease input phone_num:");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;
                    scan_string[strlen(scan_string)-1] = 0;
                    voice_dial(scan_string); 
                }
                break;
            case 2: //ANSWER
                {
                  //get call number
                  call_info_type pcall_info_list = {0};
                  memset(&pcall_info_list, 0, sizeof(call_info_type));
                  get_all_call_info(&pcall_info_list);
                    
                  //get current MO callid 
                    if (simcomvoice_get_call(1, &call_id) == -1)
                    {
                        printf("No Incoming call...\n");
                        break;
                    }
                    //Only one mo call
                    if(pcall_info_list.call_num < 1){
                      break;  
                    }
                    else if(pcall_info_list.call_num == 1){
                      voice_mtcall_process(VOICE_CALL_ANSWER, call_id);
                    }
                    else{
                      //Multiple calls
                      int i = 0;
                      int call_cmd = 0;
                      printf("\nThere are many calls at the moment:\n");
                      
                      for(i = 0; i < pcall_info_list.call_num; i++) {    
                        printf("MULTIPLE CALL:%d,%d,%s\n", pcall_info_list.call_info[i].call_id,  
                          pcall_info_list.call_info[i].call_state, 
                          pcall_info_list.call_info[i].phone_number);
                      }
                      printf("\nplease select option:\n");
                      printf("1:Accept the new call and place the active calls on hold\n");
                      printf("2:Accept the new call and terminate the active calls\n");
                      printf("3:Terminate the new call\n");
                      memset(scan_string, 0, sizeof(scan_string));
                      if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                          continue;
                      call_cmd = atoi(scan_string);

                      if(call_cmd == 1){
                        voice_mtcall_process(VOICE_CALL_HOLD, call_id);
                      }
                      else if (call_cmd == 2){
                        voice_mtcall_process(VOICE_END_FG, call_id);
                      }
                      else if (call_cmd == 3){
                        voice_mtcall_process(VOICE_END_BG, call_id);
                      }
                      else{
                        continue;
                      }                      
                    }
                }
                break;
            case 3: //END
                {                  
                  //get call number
                  call_info_type pcall_info_list = {0};
                  memset(&pcall_info_list, 0, sizeof(call_info_type));
                  get_all_call_info(&pcall_info_list);

                    
                    if (simcomvoice_get_call(2, &call_id) == -1)
                    {
                        printf("Nowing no calling...\n");
                        break;
                    }
                     //Only one mo call
                    if(pcall_info_list.call_num < 1){
                      break;  
                    }
                    else if(pcall_info_list.call_num == 1){
                      voice_mtcall_process(VOICE_CALL_HANDUP, call_id);
                    }
                    else{
                      //Multiple calls
                      int i = 0;
                      int call_cmd = 0;
                      printf("\nThere are many calls at the moment:\n");                      
                      for(i = 0; i < pcall_info_list.call_num; i++) {    
                        printf("MULTIPLE:%d,%d,%s\n", pcall_info_list.call_info[i].call_id,  
                          pcall_info_list.call_info[i].call_state, 
                          pcall_info_list.call_info[i].phone_number);
                      }
                      printf("\nplease select option:\n");
                      printf("1:Terminate all calls\n");
                      printf("2:Terminate all active calls and accept the other calls\n");
                      printf("3:Terminate all hold calls\n");
                      memset(scan_string, 0, sizeof(scan_string));
                      if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                          continue;
                      call_cmd = atoi(scan_string);

                      if(call_cmd == 1){
                        voice_mtcall_process(VOICE_END_ALL, call_id);
                      }
                      else if (call_cmd == 2){
                        voice_mtcall_process(VOICE_END_FG, call_id);
                      }
                      else if (call_cmd == 3){
                        voice_mtcall_process(VOICE_END_BG, call_id);
                      }
                      else{
                        continue;
                      }                      

                    }
                }
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

    voice_deinit();
}

void simcom_voice_test_ind_handler(void *usrdata)
{
    int call_num=0;
    int i;
    char scan_string[100];
    unsigned char call_id = 0;
    voice_call_option opt,option;
    int opt_selected = 0;

    call_info_type *pcall_list = (call_info_type *)usrdata;

    while(1)
    {
        if(pcall_list->call_num == 1)
        {
            if(pcall_list->call_info[0].direction == CALL_DIRECTION_MT_V02)
            {
                printf("please select option:\n");
                printf("1. handup              2.answer\n");
                memset(scan_string, 0, sizeof(scan_string));
                if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    return;
                opt = (voice_call_option)atoi(scan_string);

                if(opt == 1)
                {
                    call_id = pcall_list->call_info[0].call_id;
                    option = VOICE_CALL_HANDUP;
                    opt_selected = 1;
                }
                else if(opt == 2)
                {
                    call_id = pcall_list->call_info[0].call_id;
                    option = VOICE_CALL_ANSWER;
                    opt_selected = 1;
                }
            }
            else
            {
                printf("please select option: \n");
                printf("1. handup\n");
                memset(scan_string, 0, sizeof(scan_string));
                if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    return;	
                opt = (voice_call_option)atoi(scan_string);

                if(opt == 1)
                {
                    call_id = pcall_list->call_info[0].call_id;
                    option = VOICE_CALL_HANDUP;
                    opt_selected = 1;
                }
            }
        }
        else
        {
            printf("please select option:\n");
            printf("3. hold             	4.end all call\n");
            printf("5. end background call  6.end foreground call\n");
            memset(scan_string, 0, sizeof(scan_string));
            if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                return;
            opt = (voice_call_option)atoi(scan_string);

            if(opt >= VOICE_CALL_HOLD && opt <= VOICE_END_FG)
            {
                option = opt;
                opt_selected = 1;
            } 
        }
        if(opt_selected == 1)
        {
            voice_mtcall_process(option, call_id);
            break;
        }
    }

}

int app_nas_printf_serving_system(nas_serving_system_type_v01 serving_system)
{
    int index = 0;
    int ret = 1;

    if(serving_system.registration_state == 1){
        printf("NAS: REGISTERED!\n");
    }
    else if(serving_system.registration_state == 2){
        printf("NAS: NOT_REGISTERED_SEARCHING!\n");
    }
    else if(serving_system.registration_state == 3){
        printf("NAS: REGISTRATION_DENIED!\n");
    }
    else if(serving_system.registration_state == 4){
        printf("NAS: REGISTRATION_UNKNOWN!\n");
    }
    else{
        printf("NAS: NOT_REGISTERED!\n");
    }

    if(serving_system.cs_attach_state == 1){
        printf("NAS: CS_ATTACHED!\n");
    }
    else if(serving_system.cs_attach_state == 2){
        printf("NAS: CS_DETACHED!\n");
    }
    else{
        printf("NAS: CS_UNKNOWN!\n");
    }

    if(serving_system.ps_attach_state == 1){
        printf("NAS: PS_ATTACHED!\n");
    }
    else if(serving_system.ps_attach_state == 2){
        printf("NAS: PS_DETACHED!\n");
    }
    else{
        printf("NAS: PS_UNKNOWN!\n");
    }

    for(index=0; index < serving_system.radio_if_len; index++)
    {
        if(serving_system.radio_if[index]==1){
            printf("NAS: [%d]RADIO_IF_CDMA_1X!\n", index);
            ret = 0;
        }
        else if(serving_system.radio_if[index]==2){
            printf("NAS: [%d]RADIO_IF_CDMA_1XEVDO!\n", index);
            ret = 0;
        }
        else if(serving_system.radio_if[index]==3){
            printf("NAS: [%d]RADIO_IF_AMPS!\n", index);
        }
        else if(serving_system.radio_if[index]==4){
            printf("NAS: [%d]RADIO_IF_GSM!\n", index);
        }
        else if(serving_system.radio_if[index]==5){
            printf("NAS: [%d]RADIO_IF_UMTS!\n", index);
        }
        else if(serving_system.radio_if[index]==8){
            printf("NAS: [%d]RADIO_IF_LTE!\n", index);
        }
        else{
            printf("NAS: [%d]RADIO_IF_NO_SVC!\n", index);
        }
    }
    return ret;
}


static int cdma_convert_to_csq(int rssi)
{
    int csqval;
    if(rssi <= -125)
    {
        csqval = 0;
    }
    else if(rssi >= -75)
    {
        csqval = 31;
    }
    else
    {
        //csqval = floor((125.0 + rssi) * 31.0 / 50);
    }
    return csqval;
}

static int gsm_convert_to_csq(int rssi)
{
    int csqval;
    if(rssi <= -113)
    {
        csqval = 0;
    }
    else if(rssi >= -51)
    {
        csqval = 31;
    }
    else
    {
        //csqval = floor((182.26 + 100.0/62 * rssi) * 31.0 / 100 + 0.5);
    }    
    return csqval;
}


static int tds_convert_to_csq(int rssi)
{
    int csqval;
    if(rssi <= -116)
    {
        csqval = 0;
    }
    else if(rssi >= -25)
    {
        csqval = 31;
    }
    else
    {
       //csqval = floor((113.0 + 100.0/191 * rssi) * 31 / 100 + 0.5);
    }    
    return csqval;
}


static int get_csq_val(int radio_if, int rssi)
{
    int csqval = 0;
    if(radio_if ==  RADIO_IF_NO_SVC)
    {
        return csqval;
    }
    //printf("get_csq_val radio_if = %d, rssi = %d\n", radio_if, rssi);
    switch(radio_if)
    {
        case RADIO_IF_NO_SVC:
            csqval = 0;
            break;
        case RADIO_IF_CDMA_1X:
        case RADIO_IF_CDMA_1XEVDO:
            csqval = cdma_convert_to_csq(rssi);
            break;
        case RADIO_IF_GSM:
        case RADIO_IF_UMTS:
        case RADIO_IF_LTE:
            csqval = gsm_convert_to_csq(rssi);
            break;
        case RADIO_IF_TDSCDMA:
            csqval = tds_convert_to_csq(rssi);
            break;
        default:
            break;
    }
   
    return csqval;  
}

static void simcom_nas_test()
{
   const char *options_list[] = {
        "1. get serving system",
        "2. get signal strength",
        "3. get network preference mode",
        "4. set network preference mode",
        "5. get gsm nmr cell info",
        "99. back",
    };
    int ret = 0;
    int opt = 0;
    char scan_string[100] = {0};
    int index;
    int level, mode;  
  
    nas_get_signal_strength_resp_msg_v01 signal_strength;
    nas_get_serving_system_resp_msg_v01 serving_system;
    
    if(nas_init() != 0)
    {
        return;
    }

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {  
            case 1:
                {
                    nas_get_serving_system(&serving_system);                    
                    if (ret != 0){
                    break;
                    }

                    /*  Serving System */
                    if (1 == app_nas_printf_serving_system(serving_system.serving_system))
                    {
                        /*  Current PLMN */                  
                        printf("PLMN: MCC=%d,MNC=%d\n", serving_system.current_plmn.mobile_country_code, 
                        serving_system.current_plmn.mobile_network_code);

                        /*  3GPP Location Area Code */
                        printf("LAC: %d\n", serving_system.lac);

                        /*  3GPP Cell ID */
                        printf("CELL_ID: %d\n", serving_system.cell_id);

                        /*  TAC Information for LTE */
                        printf("TAC: %d\n", serving_system.tac);

                        /*  UMTS Primary Scrambling Code */
                        printf("PSC: %d\n", serving_system.umts_psc);                    
                    }
                }

                if(serving_system.detailed_service_info_valid)
                {
                    printf("detailed_service_info = %d\n", serving_system.detailed_service_info);
                    int srv_status = serving_system.detailed_service_info.srv_status;
                    switch(srv_status)
                    {
                        case 0:
                            printf("No service!\n");
                            break;
                        case 1:
                            printf("Limited service!\n");
                            break;
                        case 2:
                            printf("Service available!\n");
                            break;
                        case 3:
                            printf("Limited regional service!\n");
                            break;
                        case 4:
                            printf("MS in power save or deep sleep!\n");
                            break;
                        default:
                            printf("Unknow!\n");
                            break;                           
                    }
                       
                }

                break;
            case 2:
                {
                    int i = 0;  
                    int rssi_len;
                    int rssi;
                    int rf;
                    int csqval;
                    nas_radio_if_enum_v01 radio_if;
                    ret = nas_get_signal_strength(&signal_strength);
                    if (ret != 0)
                    {
                        printf("get sig err!\n");
                        break;
                    }
                    /*------------------ radio_if ------------------------------
                    -0x00 -- RADIO_IF_NO_SVC      -- None (no service)
                    -0x01 -- RADIO_IF_CDMA_1X     -- cdma2000\textsuperscript{\textregistered} 1X
                    -0x02 -- RADIO_IF_CDMA_1XEVDO -- cdma2000\textsuperscript{\textregistered} HRPD (1xEV-DO)
                    -0x04 -- RADIO_IF_GSM         -- GSM
                    -0x05 -- RADIO_IF_UMTS        -- UMTS
                    -0x08 -- RADIO_IF_LTE         -- LTE
                    -0x09 -- RADIO_IF_TDSCDMA     -- TDSCDMA
                    -------------------------------------------------*/

                    rssi_len = signal_strength.rssi_len;
                    
                    
                    if(rssi_len >= 2)
                    {
                        if(signal_strength.rssi[1].radio_if == RADIO_IF_LTE)
                        {
                            rssi = signal_strength.rssi[1].rssi;
                            rf = RADIO_IF_LTE;
                        }
                        else if(signal_strength.rssi[0].radio_if == RADIO_IF_LTE)
                        {
                            rssi = signal_strength.rssi[1].rssi;
                            rf = RADIO_IF_LTE;
                        }
                        else if(signal_strength.rssi[0].radio_if == RADIO_IF_CDMA_1X)
                        {
                            rssi = signal_strength.rssi[0].rssi;
                            rf = RADIO_IF_CDMA_1X;
                        }
                    }
                    else
                    {
                        rssi = signal_strength.rssi[0].rssi;
                        rf = signal_strength.rssi[0].radio_if;
                    }

                    csqval = get_csq_val(rf, (-1) * rssi);

                    printf("radio_if = %d, rssi= %d, csq= %d\n", rf, (-1) * rssi, csqval);
                    
                    #if 0
                    radio_if = signal_strength.signal_strength.radio_if;
                    printf("radio_if = %d\n",radio_if);
                    switch(radio_if)
                    {
                        case NAS_RADIO_IF_CDMA_1X_V01:
                            printf("CDMA\n");
                            break;
                        case NAS_RADIO_IF_CDMA_1XEVDO_V01:
                            printf("CDMA DVDO\n");
                            break;                          
                        case NAS_RADIO_IF_GSM_V01:
                            printf("GSM\n");
                            break;                             
                        case NAS_RADIO_IF_UMTS_V01:
                            printf("WCDMA\n");
                            break;                           
                        case NAS_RADIO_IF_LTE_V01:
                            printf("LTE\n");
                            break; 
                        case NAS_RADIO_IF_TDSCDMA_V01:
                            printf("TDSCDMA\n");
                            break; 
                        default:
                            break;

                    }

                  
                    printf("sig_strength = %d\n",signal_strength.signal_strength.sig_strength);
                  
                  
                    /*  Signal Strength List */
                    if(signal_strength.signal_strength_list_len <= NAS_SIG_STRENGTH_LIST_MAX_V01){
                        for (i =0; i < signal_strength.signal_strength_list_len; i++){
                            printf("Signal Strength: %d,radio_if=%d\n", signal_strength.signal_strength_list[i].sig_strength, signal_strength.signal_strength_list[i].radio_if);
                        }
                    }

                    /*  RSSI List */              
                    if(signal_strength.rssi_len <= NAS_RSSI_LIST_MAX_V01){
                        for (i =0; i < signal_strength.rssi_len; i++){
                            printf("RSSI: -%d,radio_if=%d\n", signal_strength.rssi[i].rssi, signal_strength.rssi[i].radio_if);
                        }
                    }

                    /*  ECIO List */               
                    if(signal_strength.ecio_len <= NAS_ECIO_LIST_MAX_V01){
                        for (i =0; i < signal_strength.ecio_len; i++){
                            printf("ECIO: %d,radio_if=%d\n", signal_strength.ecio[i].ecio, signal_strength.ecio[i].radio_if);
                        }
                    }

                    /*  IO */
                    printf("IO: %d(dBm)\n", signal_strength.io);

                    /*  SINR */
                    printf("SINR: %d(dBm)\n", signal_strength.sinr);
                    /**<   SINR level. SINR is only applicable for 1xEV-DO.
                       Valid levels are 0 to 8, where the maximum value for:        \n
                       - 0x00 -- SINR_LEVEL_0 is -9 dB     \n
                       - 0x01 -- SINR_LEVEL_1 is -6 dB     \n
                       - 0x02 -- SINR_LEVEL_2 is -4.5 dB   \n
                       - 0x03 -- SINR_LEVEL_3 is -3 dB     \n
                       - 0x04 -- SINR_LEVEL_4 is -2 dB     \n
                       - 0x05 -- SINR_LEVEL_5 is +1 dB     \n
                       - 0x06 -- SINR_LEVEL_6 is +3 dB     \n
                       - 0x07 -- SINR_LEVEL_7 is +6 dB     \n
                       - 0x08 -- SINR_LEVEL_8 is +9 dB
                    */

                    /*  Error Rate List */                             
                    if(signal_strength.error_rate_len <= NAS_ERROR_RATE_LIST_MAX_V01){
                        for (i =0; i < signal_strength.error_rate_len; i++){
                            printf("ERROR RATE: %d,radio_if=%d\n", signal_strength.error_rate[i].error_rate, signal_strength.error_rate[i].radio_if);
                        }
                    }

                    /*  RSRQ */          
                    printf("RSRQ: rsrq=%d,radio_if=%d\n", signal_strength.rsrq.rsrq, signal_strength.rsrq.radio_if);

                    if(signal_strength.rsrq.radio_if == 8){
                        /*  LTE SNR */
                        printf("SNR: %d(dB)\n", signal_strength.snr);
                        /**<   LTE SNR level as a scaled integer in units of 0.1 dB; 
                        e.g., -16 dB has a value of -160 and 24.6 dB has a value of 246.
                        LTE SNR is included only when the current serving system is LTE.
                        */

                        /*  LTE RSRP */
                        printf("LTE RSRP: %d(dBm)\n", signal_strength.lte_rsrp);
                        /**<   Current LTE RSRP in dBm as measured by L1. 
                        Range: -44 to -140 (-44 means -44 dBm, -140 means -140 dBm).
                        LTE RSRP is included only if the current serving system is LTE.
                        */
                    }
                    #endif
                }
                break;
            case 3:
                {
                    mode_pref_mask_type_v01  mode_pref;
                    nas_get_preference_network(&mode_pref);
                    printf("mode pref = %d\n", mode_pref);

                    if(mode_pref & QMI_NAS_RAT_MODE_PREF_LTE_V01)
                    {
                        printf("LTE mode enable!\n");
                    }
                    if(mode_pref & QMI_NAS_RAT_MODE_PREF_UMTS_V01)
                    {
                        printf("WCDMA mode enable!\n");
                    }
                    if(mode_pref & QMI_NAS_RAT_MODE_PREF_GSM_V01)
                    {
                        printf("GSM mode enable!\n");
                    }                    
                    if(mode_pref & QMI_NAS_RAT_MODE_PREF_CDMA2000_1X_V01)
                    {
                        printf("CDMA mode enable!\n");
                    }
                    if(mode_pref & QMI_NAS_RAT_MODE_PREF_CDMA2000_HRPD_V01)
                    {
                        printf("EVDO mode enable!\n");
                    }
                    if(mode_pref & QMI_NAS_RAT_MODE_PREF_TDSCDMA_V01)
                    {
                        printf("TDSCDMA mode enable!\n");
                    }
                  
                }
                break;
            case 4:
                {
                    int mode_LTE;
                    int mode_GSM;
                    int mode_WCDMA;
                    int mode_CDMA;
                    int mode_EVDO;
                    int mode_TDSCDMA;
                    mode_pref_mask_type_v01  mode_pref = 0;
                    int ret;
                    
                    printf("\nPlease input LTE       mode (0: disable 1: enable)\n");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                    mode_LTE = atoi(scan_string); 
                    
                    printf("\nPlease input WCDMA     mode (0: disable 1: enable)\n");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                    mode_WCDMA = atoi(scan_string); 
                    
                    printf("\nPlease input GSM       mode (0: disable 1: enable)\n");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                    mode_GSM = atoi(scan_string); 
                    
                    printf("\nPlease input CDMA      mode (0: disable 1: enable)\n");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                    mode_CDMA = atoi(scan_string); 
                    
                    printf("\nPlease input EVDO      mode (0: disable 1: enable)\n");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                    mode_EVDO = atoi(scan_string); 
                    
                    printf("\nPlease input TDSCDCMA  mode (0: disable 1: enable)\n");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        break;
                    mode_TDSCDMA = atoi(scan_string); 

                    if(mode_LTE)
                    {
                        mode_pref += QMI_NAS_RAT_MODE_PREF_LTE_V01;
                    }
                    if(mode_WCDMA)
                    {
                        mode_pref += QMI_NAS_RAT_MODE_PREF_UMTS_V01;
                    }
                    if(mode_GSM)
                    {
                        mode_pref += QMI_NAS_RAT_MODE_PREF_GSM_V01;
                    }
                    if(mode_CDMA)
                    {
                        mode_pref += QMI_NAS_RAT_MODE_PREF_CDMA2000_1X_V01;
                    }
                    if(mode_EVDO)
                    {
                        mode_pref += QMI_NAS_RAT_MODE_PREF_CDMA2000_HRPD_V01;
                    }
                    if(mode_TDSCDMA)
                    {
                        mode_pref += QMI_NAS_RAT_MODE_PREF_TDSCDMA_V01;
                    }
                    printf("mode_pref = %d\n", mode_pref);
                    ret = nas_set_preference_network(mode_pref);
                    if(ret == 0)
                    {
                        printf("set preference success\n", ret);
                    }
                    else
                    {
                        printf("set preference fail ret = %d\n", ret);
                    }
                }
                break;     
            case 5:
                {
                    int i;
    		        nas_gsm_cell_info_t cell_info;
                    
                    if (-1 == nas_get_gsm_cell_info(&cell_info))
                    {
                        printf("get gsm cell info failure");
                        break;
                    }
                    
                    printf("Primary Cell ID = %d\n", cell_info.cell_info.cell_id);
                    printf("Primary LAC = %d\n", cell_info.cell_info.lac);
                    printf("Primary RSSI = %ddBm\n", cell_info.cell_info.rssi);
                    
                    for (i=0; i<cell_info.nmr_cell_len; i++)
                    {
                        printf("nmr%d Cell ID = %d\n", i, cell_info.nmr_cell_info[i].cell_id);
                        printf("nmr%d LAC = %d\n", i, cell_info.nmr_cell_info[i].lac);
                        printf("nmr%d RSSI = %ddBm\n", i, cell_info.nmr_cell_info[i].rssi);
                    }
                }
                break;
            case 99:
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

    nas_dinit();
}

static boolean select_ap_index(ap_index_type *ap_index)
{
    char ap_index_str[64] = {0};
    wifi_mode_type mode = get_wifi_mode();
    int ap_index_val;
    if(mode == APAP_MODE)
    {
        printf("Please input the ap_index: ");
        fgets(ap_index_str, sizeof(ap_index_str), stdin);
           
        //ap_index_val == atoi(ap_index_str);
        if(strlen(ap_index_str) != 2)
        {
            return FALSE;
        }

        switch(ap_index_str[0])
        {
            case '0':
                ap_index_val = AP_INDEX_0;
                break;

            case '1':
                ap_index_val = AP_INDEX_1;
                break;

            default:
                ap_index_val = 2;
                break;
        }
        
        if(ap_index_val != AP_INDEX_0 && ap_index_val != AP_INDEX_1)
        {
            printf("ap_index input wrong!\r\n");
            return FALSE;
        }
        else
        {
            *ap_index = (ap_index_type)ap_index_val;
        }
    }
    else if(mode == APSTA_MODE)
    {
        *ap_index = AP_INDEX_STA;
    }
    else if(mode == AP_MODE)
    {
        *ap_index = AP_INDEX_0;
    }
    else
    {
         printf("ap mode is invalid!\r\n");
         return FALSE;
    }

    return TRUE;
}

static int is_ascii_key_string(char *str, int len)
{
    int i;
    char c;
    int ret = TRUE;
    for(i = 0; i < len; i++)
    {
        c = *(str + i);
        if( c < 0x20 || c > 0x7F)
        {
            ret = FALSE;
        }
    }

    return ret;
}


static int simcom_check_wep_password(char* pwstr)
{
    int ret = 0;
    int len = 0;
    int i = 0;
    
    if(pwstr == NULL){
        return -1;
    }
    
    len = strlen(pwstr);
    if(len < 5 || len > 26)
    {
        return -1;
    }
    
    if(len == 5 || len == 13)
    {
        if(!is_ascii_key_string(pwstr, len))
        {
            return -1;
        }
    }
    else if(len == 10 || len == 23)
    {
        for(i = 0;i < len; i++)
        {
            if(!isxdigit(pwstr[i]));
            {
                return -1;
            }
        }        
    }
    return 0;    
    
}
static int simcom_check_wpa_password(char* pwstr)
{
    int ret = 0;
    int len = 0;
    int i = 0;
    
    if(pwstr == NULL){
        return -1;
    }
    
    len = strlen(pwstr);
    if(len < 8 || len > 64)
    {
        return -1;
    }

    if(len == 64)
    {
        for(i = 0;i < len; i++)
        {
            if(!isxdigit(pwstr[i]));
            {
                return -1;
            }
        }        
    }
    else
    {
        if(!is_ascii_key_string(pwstr, len))
        {
            return -1;
        }   
    }
    return 0;    
    
}

static void simcom_wifi_test()
{
    char scan_string[130] = {0};
    int opt = 0;
    boolean ret = FALSE;
    int i = 0;
    int j = 0;
    /* Arry for wifi_daemon options, Should always have and even number of elements */
    const char *options_list[] = {
        "1. SET SSID",
        "2. GET SSID",
        "3. SET AUTH",
        "4. GET AUTH",
        "5. SET BCAST",
        "6. GET BCAST",
        "7. ------",
        "8. ------",
        "9. GET DHCP",
        "10. GET CLIENT_COUNT",
        "11. OPEN/CLOSE WIFI",
        "12. GET WIFI_STATUS",
        "13. GET NET_STATUS",
        "14. SET USER_INFO",
        "15. GET USER_INFO",
        "16. GET MAC_ADDR",
        "17. GET AP_CFG(for W58)",
        "18. SET AP_CFG(for W58)",	
        "19. GET STA_IP",
        "20. SET STA_CFG",
        "21. GET STA_CFG",
        "22. STA SCAN",
        "23. SET_SSID_AND_AUTH",
        "24. OPEN/CLOSE STA MODE(for W58L)",
        "25. GET STA_STATE(for W58L)",
        "26. CONNECT/DISCONNECT THE NET",
        "27. GET THE NET STATUS",
        "28. RESTORE",
        "29. GET OPERATION RESULT CODE",
        "99. BACK",
    };

    boolean be_exit = FALSE;

    while(TRUE)
    {
        /* Display menu of options. */
        printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");

         
         /* Read the option from the standard input. */
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                {
                    char ssid[65] = {0};
                    ap_index_type    ap_index;
                    wifi_mode_type   mode;
                    printf("   Please input the SSID: ");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    memcpy(ssid, scan_string, strlen(scan_string)-1);

                    mode = get_wifi_mode();
                    if(mode == APAP_MODE)
                    {
                         printf("   Please input the index: ");
                         memset(scan_string, 0, sizeof(scan_string));
                         if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                             continue;
                          //ap_index == atoi(scan_string);
                         if(strlen(scan_string) != 2)
                        {
                        	break;
                        }

                        switch(scan_string[0])
                        {
                            case '0':
                                ap_index = AP_INDEX_0;
                                break;
                        
                            case '1':
                                ap_index = AP_INDEX_1;
                                break;
                                
                            default:
                                ap_index = AP_INDEX_STA;
                                break;
                        }
                          
                        if(ap_index != AP_INDEX_0 && ap_index != AP_INDEX_1)
                        {
                            // error index
                            break;
                        }
                    }
                    else if(mode == AP_MODE)
                    {
                        ap_index = AP_INDEX_0;
                    }
                    else
                    {
                        ap_index =  AP_INDEX_STA;
                    }
                    ret = set_ssid(ssid, ap_index);
                    if(ret)
                    {
                        printf("\nset SSID success!\n");
                    }
                    else
                    {
                        printf("\nset SSID failed!\n");
                    }
                }
                break;

            case 2:
                {
                    char ssid[69] = {0};
                    ap_index_type ap_index;
                    if(!select_ap_index(&ap_index))
                    {
                        break;
                    }
                    ret = get_ssid(ssid, ap_index);
                    if(ret)
                    {
                        printf("\nSSID: %s\n", ssid);
                    }
                    else
                    {
                        printf("\nget SSID failed!\n");
                    }
                }
                break;

            case 3:
                {
                    int auth_type = 0;
                    char str_encrypt_mode[8] = {0};
                    int encrypt_mode = 0;
                    char pwd_str[65] = {0};
                    ap_index_type ap_index;
                    if(!select_ap_index(&ap_index))
                    {
                        break;
                    }             
                    printf("Please input the auth type(%d-%d): ", AUTH_MIN, AUTH_MAX);
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;
                    auth_type = atoi(scan_string);

                    if(auth_type >= AUTH_WPA)  // wpa 
                    {
                        printf("   Please input the encrypt mode(%d-%d): ", E_TKIP, E_AES_TKIP);
                    }
                    else
                    {
                        printf("   Please input the encrypt mode(%d-%d): ", E_NULL, E_WEP);
                    }
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    simcom_handle_input_string(scan_string, strlen(scan_string), str_encrypt_mode, 7, 0);
                        
                    encrypt_mode = atoi(str_encrypt_mode);

                    //check  encrypt_mode value
                    if(auth_type >= AUTH_WPA)  // wpa 
                    {
                        if((encrypt_mode < E_TKIP) || (encrypt_mode > E_AES_TKIP)){
                            printf("   Input the encrypt mode failed!\n");
                            break;
                        }
                    }
                    else
                    {                   
                        if((encrypt_mode < E_NULL) || (encrypt_mode > E_WEP)){
                            printf("   Input the encrypt mode failed!\n");
                            break;
                        }
                    }                    

                    /* share mode need set wep key */
                    if( !(auth_type == AUTH_SHARE && encrypt_mode == E_NULL) )
                    {
                        if(auth_type == AUTH_SHARE && encrypt_mode == E_WEP ) 
                        {
                            printf("   Please input the password(ASC 5-13/hex 10-26): ");
                            memset(scan_string, 0, sizeof(scan_string));
                            if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                                continue;
                            memcpy(pwd_str, scan_string, strlen(scan_string)-1);
                        }
                        else if(auth_type == AUTH_AUTO && encrypt_mode == E_WEP ) 
                        {
                            printf("   Please input the password(ASC 5 or 13/hex 10 or 26): ");
                            memset(scan_string, 0, sizeof(scan_string));
                            if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                                continue;
                            memcpy(pwd_str, scan_string, strlen(scan_string)-1);
                            
                        }
                        else if(auth_type == AUTH_OPEN && encrypt_mode == E_WEP ) 
                        {
                            printf("   Please input the password(ASC 5 or 13/hex 10 or 26): ");
                            memset(scan_string, 0, sizeof(scan_string));
                            if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                                continue;
                            memcpy(pwd_str, scan_string, strlen(scan_string)-1);
                            
                        }
                        else if(encrypt_mode)
                        {
                            printf("   Please input the password(8-64): ");
                            memset(scan_string, 0, sizeof(scan_string));
                            if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                                continue;
                            memcpy(pwd_str, scan_string, strlen(scan_string)-1);
                        }else if(!encrypt_mode)
                        {
                            memset(pwd_str, 0, sizeof(pwd_str));
                        }       
                    }

                    ret = set_auth(pwd_str, auth_type, encrypt_mode, ap_index);
                    if(ret)
                    {
                        printf("\nset AUTH success!\n");
                    }
                    else
                    {
                        printf("\nset AUTH failed!\n");
                    }
                }
                break;

            case 4:
                {
                    int auth_type = 0;
                    int encrypt_mode = 0;
                    char pwd_str[65] = {0};
                    ap_index_type ap_index;
                    if(!select_ap_index(&ap_index))
                    {
                        break;
                    }
                    ret = get_auth(&auth_type, &encrypt_mode, pwd_str, ap_index);
                    if(ret)
                    {
                        printf("\nAUTH: %d,%d,\"%s\"\n", auth_type, encrypt_mode, pwd_str);
                    }
                    else
                    {
                        printf("\nget AUTH failed!\n");
                    }
                }
                break;

            case 5:
                {
                    int broadcast = 0;
                    ap_index_type ap_index;
                    if(!select_ap_index(&ap_index))
                    {
                        break;
                    }                  
                    printf("   Please input the broadcast: ");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    broadcast = atoi(scan_string);

                    ret = set_bcast(broadcast, ap_index);
                    if(ret)
                    {
                        printf("\nset set_bcast success!\n");
                    }
                    else
                    {
                        printf("\nset set_bcast failed!\n");
                    }
                }
                break;

            case 6:
                {
                    int broadcast = 0;
                    ap_index_type ap_index;
                    if(!select_ap_index(&ap_index))
                    {
                        break;
                    }

                    ret = get_bcast(&broadcast, ap_index);
                    if(ret)
                    {
                        printf("\nbroadcast: %d\n", broadcast);
                    }
                    else
                    {
                        printf("\nget broadcast failed!\n");
                    }
                }
                break;
            #if 0
            case 7:
                {
                    int channel = 0;
                    int mode = 0;
                    ap_index_type ap_index;
                    break;
                    if(!select_ap_index(&ap_index))
                    {
                        break;
                    }                    
                    printf("   Please input the channel: ");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;
                    channel = atoi(scan_string);

                    
                    printf("   Please input the mode: ");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;
                    mode = atoi(scan_string);

                    ret = set_channel_mode(channel, mode, ap_index);
                    if(ret)
                    {
                        printf("\nset set_channel_mode success!\n");
                    }
                    else
                    {
                        printf("\nset set_channel_mode failed!\n");
                    }
                }
                break;

            case 8:
                {
                    char channel_str[8] = {0};
                    int mode = 0;
                    ap_index_type ap_index;
                    break;
                    if(!select_ap_index(&ap_index))
                    {
                        break;
                    }

                    ret = get_channel_mode(channel_str, &mode, ap_index);
                    if(ret)
                    {
                        printf("\n channel_mode: %s,%d\n", channel_str, mode);
                    }
                    else
                    {
                        printf("\n get_channel_mode failed!\n");
                    }
                }
                break;
            #endif
            case 9:
                {
                    char   host_ip_str[16] = {0};
                    char   start_ip_str[16] = {0};
                    char   end_ip_str[16] = {0};
                    char   time_str[16] = {0};

                    ret = get_dhcp(host_ip_str, start_ip_str, end_ip_str, time_str);
                    if(ret)
                    {
                        printf("\n DHCP: \"%s\",\"%s\",\"%s\",%s\n", host_ip_str, start_ip_str, end_ip_str, time_str);
                    }
                    else
                    {
                        printf("\n get_dhcp failed!\n");
                    }
                }
                break;

            case 10:
                {
                    ap_index_type ap_index;
                    int count1,count2;
                    wifi_mode_type mode = get_wifi_mode();
                    if(mode == AP_MODE || mode == APSTA_MODE)
                    {
                        count1 = get_client_count(AP_INDEX_0);
                        printf("\n count: %d\n", count1);
                    }
                    else
                    {
                        count1 = get_client_count(AP_INDEX_0);
                        count2 = get_client_count(AP_INDEX_1);
                        printf("\n AP1_count: %d  AP2_count: %d\n", count1, count2);
                    }
                }
                break;

            case 11:
                {
                    int flag = 0;
                    printf("   Please input the operation( 0-Disable AP; 1-Enable AP): ");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;
                    flag = atoi(scan_string);

                    ret = wifi_power(flag);
                    if(ret)
                    {
                        printf("\n set_map success!\n");
                    }
                    else
                    {
                        printf("\n set_map failed!\n");
                    }
                }
                break;

            case 12:
                {
                    uint8 flag = 0;

                    ret = get_wifi_status(&flag);
                    if(ret)
                    {
                        printf("\n wifi status: %d\n", flag);
                    }
                    else
                    {
                       printf("\n wifi status: 0\n");
                    }
                }
                break;

            case 13:
                {
                    char net_enable_str[sizeof(int) + 1] ={0};
                     int save_flag = 0;
                    ret = get_net_status(net_enable_str, &save_flag);
                    if(ret)
                    {
                        printf("\n NET_STATUS: %s,%d\n", net_enable_str, save_flag);
                    }
                    else
                    {
                        printf("\n get_net_status failed!\n");
                    }
                }
                break;

            case 14:
                {
                    char sz_usrname[128] = {0};
                    char sz_usrpwd[128] = {0};
                    char ch;
                    int i = 0,j = 0;
                    int nret = 0;
                        
                    printf("   Please input the user name(length 1-127): ");
                    
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;
                    
                    nret = simcom_handle_input_string(scan_string, strlen(scan_string), sz_usrname, 127, 1);
                    if(nret < 0){
                       printf("\n user name is too short!\n");
                       break;
                    }
                    else if(nret == 1){
                       printf("\n user name is too long!\n");
                       break;
                    }
                    else if(nret == 2){
                       printf("\n user name is too long!\n");
                       memset(scan_string, 0, sizeof(scan_string));
                       fgets(scan_string, sizeof(scan_string), stdin);
                       break;
                    }
                    
                    printf("   Please input the password(length 1-127): ");
                    memset(scan_string, 0, sizeof(scan_string));
                    
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;
                    
                    nret = simcom_handle_input_string(scan_string, strlen(scan_string), sz_usrpwd, 127, 1);
                    if(nret < 0){
                       printf("\n user pwd is too short!\n");
                       break;
                    }
                    else if(nret == 1){
                       printf("\n user pwd is too long!\n");
                       break;
                    }                    
                    else if(nret == 2){
                       printf("\n user pwd is too long!\n");
                       memset(scan_string, 0, sizeof(scan_string));
                       fgets(scan_string, sizeof(scan_string), stdin);
                       break;
                    }

                    ret = set_user_name_pwd(sz_usrname, sz_usrpwd);
                    if(ret)
                    {
                        printf("\n set_user_name_pwd success!\n");
                    }
                    else
                    {
                        printf("\n set_user_name_pwd failed!\n");
                    }
                }
                break;

            case 15:
                {
                    char sz_usrname[128] = {0};
                    char sz_usrpwd[128] = {0};

                    ret = get_user_name_pwd(sz_usrname, sizeof(sz_usrname), sz_usrpwd, sizeof(sz_usrpwd));
                    if(ret)
                    {
                        printf("\n USER NAME:%s    PASSWORD:%s\n", sz_usrname, sz_usrpwd);
                    }
                    else
                    {
                        printf("\n get_user_name_pwd failed!\n");
                    }
                }
                break;

            case 16:
                {
                    char mac_addr[1024] = {0};
                    ap_index_type ap_index;
                    if(!select_ap_index(&ap_index))
                    {
                        break;
                    }

                    ret = get_mac_addr(mac_addr, ap_index);
                    if(ret)
                    {
                        printf("\n MAC_ADDR: %s\n", mac_addr);
                    }
                    else
                    {
                        printf("\n get_mac_addr failed!\n");
                    }
                }
                break;


            case 17:
                {
                    wifi_mode_type mode = get_wifi_mode();
                    printf("\n wifi mode: %d\n", mode);
                }
                break;

            case 18:
                {
                    int i = 0;
                    
                    int mode;

                    
                    printf("   Please input the wifi mode (0: AP  1:AP-AP  2:AP-STA) >");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    mode = atoi(scan_string);
                    if(mode < 0 || mode > 2)
                    {
                        printf("input mode wrong");
                        break;
                    }


                    ret = set_wifi_mode((wifi_mode_type)mode);
                    if(ret)
                    {
                        printf("\n set_wifi_mode success!\n");
                    }
                    else
                    {
                        printf("\n set_wifi_mode failed!\n");
                    }
                }
                break;

            case 19:
                {
                    char ip_addr[30] = {0};
                    int status = 0;
               
                    if(!wifi_is_sta_mode())
                    {
                        printf("mode is not AP-STA\r\n");
                        break;
                    }  
                    
                    ret = get_sta_ip(ip_addr, sizeof(ip_addr));
                    if(ret)
                    {
                        printf("\n STA_IP: %s\n", ip_addr);
                    }
                    else
                    {
                        printf("\n get_sta_ip failed!\n");
                    }
                }
                break;

            case 20:
                {
                    char ssid[65] = {0};
                    char psk_value[65] = {0};
                    int status = 0;
                    boolean is_chines = FALSE;
                    int i = 0;
                    if(!wifi_is_sta_mode())
                    {
                        printf("mode is not AP-STA\r\n");
                        break;
                    }                      
                    printf("   Please input the ssid string(1-32): ");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    if(strlen(scan_string) > 32+1)
                    {
                        printf("\n ssid is too long!\n");
                        break;
                    }

                    //ssid[0] = '\"';
                    strncpy(ssid, scan_string, strlen(scan_string)-1);
                    //ssid[strlen(ssid)] = '\"';
                    printf("\n ssid[%s]\n", ssid);


                    printf("   Please input the password of external wireless network(8-64): ");
                    memset(scan_string, 0, sizeof(scan_string));
                    if(fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    if(strlen(scan_string) > 64+1)
                    {
                        printf("\n password is too long!\n");
                        break;
                    }
                    while(i < strlen(scan_string))
                    {
                        if(scan_string[i]>=0xb0 && scan_string[i]<=0xf7 && scan_string[i+1]>=0xa1 && scan_string[i+1]<=0xfe)
                        {
                             is_chines = true;
                             i += 2;
                        }
                        else
                        {
                            i++;
                        }
                       
                    }       
                    if(is_chines)
                    {
                        printf("input Format err!\n");
                        break;
                    }

                    //psk_value[0] = '\"';
                    strncpy(psk_value, scan_string, strlen(scan_string)-1);
                    //psk_value[strlen(psk_value)] = '\"';
                    //printf("\n psk_value[%s]\n", psk_value);

                    ret = set_sta_cfg(ssid, psk_value);
                    if(ret)
                    {
                        printf("\n set_sta_cfg success!\n");
                    }
                    else
                    {
                        printf("\n set_sta_cfg failed!\n");
                    }
                }
                break;

            case 21:
                {
                    char ssid_str[69] = {0};
                    char psk_value[69] = {0};
                    int status = 0;
                    
                    if(!wifi_is_sta_mode())
                    {
                        printf("mode is not AP-STA\r\n");
                        break;
                    }
                    ret = get_sta_cfg(ssid_str, psk_value);
                    if(ret)
                    {
                        printf("\n STA_CFG: %s, %s\n", ssid_str, psk_value);
                    }
                    else
                    {
                        printf("\n get_sta_cfg failed!\n");
                    }
                }
                break;

            case 22:
                {
                    char list_str[300] = {0};
                    int status = 0;
                
                    if(!wifi_is_sta_mode())
                    {
                        printf("mode is not AP-STA\r\n");
                        break;
                    }
                    
                    ret = sta_scan(list_str);
                    if(ret)
                    {
                        printf("\n STA_SCAN LIST: %s\n", list_str);
                    }
                    else
                    {
                        printf("\n sta_scan failed!\n");
                    }
                }
                break;

        case 23:
            {
                    char ssid[66] = {0};
                    int auth_type = 0;
                    int encrypt_mode = 0;
                    char pwd_str[128] = {0};
                    int  nret;
                    ap_index_type    ap_index;
                    wifi_mode_type   mode;
                    printf("   Please input the SSID: ");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    memcpy(ssid, scan_string, strlen(scan_string)-1);

                    mode = get_wifi_mode();
                    if(mode == APAP_MODE)
                    {
                         printf("   Please input the index: ");
                         memset(scan_string, 0, sizeof(scan_string));
                         if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                             continue;
                          //ap_index == atoi(scan_string);
                         if(strlen(scan_string) != 2)
                        {
                        	break;
                        }

                        switch(scan_string[0])
                        {
                            case '0':
                                ap_index = AP_INDEX_0;
                                break;
                        
                            case '1':
                                ap_index = AP_INDEX_1;
                                break;
                                
                            default:
                                ap_index = AP_INDEX_STA;
                                break;
                        }
                          
                        if(ap_index != AP_INDEX_0 && ap_index != AP_INDEX_1)
                        {
                            // error index
                            break;
                        }
                    }
                    else if(mode == AP_MODE)
                    {
                        ap_index = AP_INDEX_0;
                    }
                    else
                    {
                        ap_index =  AP_INDEX_STA;
                    }


                    if(!select_ap_index(&ap_index))
                    {
                        break;
                    }             
                    printf("Please input the auth type(%d-%d): ", AUTH_MIN, AUTH_MAX);
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;
                    auth_type = atoi(scan_string);

                    if(auth_type > AUTH_MAX || auth_type < AUTH_MIN)
                    {
                        printf(" auth type wrong\n");
                        continue;
                    }
                    if(auth_type >= AUTH_WPA)  // wpa 
                    {
                        printf("   Please input the encrypt mode(%d-%d): ", E_TKIP, E_AES_TKIP);
                    }
                    else
                    {
                        printf("   Please input the encrypt mode(%d-%d): ", E_NULL, E_WEP);
                    }
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;
                    encrypt_mode = atoi(scan_string);
                    if(auth_type <= AUTH_OPEN)
                    {
                        if(encrypt_mode < E_NULL || encrypt_mode > E_WEP)
                        {
                            printf("encrypt mode wrong");
                            continue;
                        }
                    }
                    else if(auth_type == AUTH_SHARE)
                    {
                        if(encrypt_mode != E_WEP)
                        {
                            printf("encrypt mode wrong");
                            continue;                           
                        }
                    }
                    else if(auth_type >= AUTH_WPA)
                    {
                        if(encrypt_mode < E_TKIP || encrypt_mode > E_AES_TKIP)
                        {
                            printf("encrypt mode wrong");
                            continue;                           
                        }
                    }
                    
                    if(encrypt_mode == E_WEP)
                    {
                        printf("   Please input the wep password\n ");
                        printf("format:\n");
                        printf(" 5  ASCII character      example: sim12 \n ");
                        printf(" 10 hexadecimal number   example: 12345abcdf \n ");
                        printf(" 13 ASCII character      example: simcom12345678 \n ");
                        printf(" 26 hexadecimal number   example: abcdef1234567abcdef1234567 \n ");
                        printf("\ninput:");
                        memset(scan_string, 0, sizeof(scan_string));
                        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                            continue;                  
                        if(strlen(scan_string) >= sizeof(pwd_str) - 1)
                        {
                            printf("wep format wrong!\n");
                            continue;                            
                        }
                        memcpy(pwd_str, scan_string, strlen(scan_string)-1);
                        nret = simcom_check_wep_password(pwd_str);
                        if(nret < 0)
                        {
                            printf("wep format wrong!\n");
                            continue;
                        }                        
                    }
                    else if(encrypt_mode >= E_TKIP)
                    {
                        printf("   Please input the wpa password\n ");
                        printf("format:\n");
                        printf(" 8~63  ASCII character   example: simcom12 /simcom123456***** \n ");
                        printf(" 64 bits    example: 12345abcdf \n ");    

                        printf("\ninput:");
                        memset(scan_string, 0, sizeof(scan_string));
                        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                            continue;                  
                        if(strlen(scan_string) >= sizeof(pwd_str) - 1)
                        {
                            printf("wpa format wrong!\n");
                            continue;                            
                        }
                        memcpy(pwd_str, scan_string, strlen(scan_string)-1);
                        
                        nret = simcom_check_wpa_password(pwd_str);
                        if(nret < 0)
                        {
                            printf("wpa format wrong!\n");
                            continue;                            
                        }  
                    }
                    
                    ret = set_ssid_and_auth(ssid,pwd_str, auth_type, encrypt_mode, ap_index);
                    if(ret)
                    {
                        printf("\nset SSID_AND_AUTH success!\n");
                    }
                    else
                    {
                        printf("\nset SSID_AND_AUTH failed!\n");
                    }

                }
                break;

             case 24:
                {
                    int op; 
                    printf("   Please input the operation( 0-Close STA mode; 1-Open STA mode): >");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    op = atoi(scan_string);
                    if(op < 0 || op > 1)
                    {
                        printf("input wrong operation!\n");
                        break;
                    }

                    ret = sta_init(op);
                    if(ret)
                    {
                        printf("\n STA operation success!\n");
                    }
                    else
                    {
                        printf("\n STA operation failed!\n");
                    }
                }
                break;

            case 25:
                {
                    int status = 0;
                    ret = get_sta_status(&status);
                    if(ret)
                    {
                        printf("\n STA_STATE: %d\n", status);
                    }
                    else
                    {
                        printf("\n get_sta_status failed!\n");
                    }
                }
                break;

             case 26:
                {
                    int act = -1;
                    int save_flag;

                    printf("   Please input the operation (0: DISCONNECT  1:CONNECT) >");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    if(scan_string[0] == '0')
                    {
                        act = 0;
                    }
                    else if(scan_string[0] == '1')
                    {
                        act = 1;
                    }
                    //act = atoi(scan_string);
                    printf("\nact=[%d]\n", act);
                    if(act < 0 || act >1)
                    {
                        printf("input operation wrong");
                        break;
                    }

                    printf("   Please input the save_flag (0: not save  1:save) >");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    save_flag = atoi(scan_string);
                    if(save_flag < 0 || save_flag >1)
                    {
                        printf("input save_flag wrong");
                        break;
                    }


                    ret = wifi_net_cnct(act, save_flag);
                    if(ret)
                    {
                        printf("\n wifi_net_cnct success!\n");
                    }
                    else
                    {
                        printf("\n wifi_net_cnct failed!\n");
                    }
                }
                break;

            case 27:
                {
                    int status = 0;
                    ret = wifi_get_net_status(&status);
                    if(ret)
                    {
                        printf("\n NET_STATE: %d\n", status);
                    }
                    else
                    {
                        printf("\n wifi_get_net_status failed!\n");
                    }
                }
                break;

            case 28:
                {
                    printf("\nPlease wait to restore...\n");
                    restore_wifi();
                    printf("\nshut down...\n");
                }
                break;

            case 29:
                {
                    uint8 ret_code = wifi_get_err_code();
                    printf("\n RESULT CODE:%d\n", ret_code);
                }
                break;

            case 99:
                {
                    be_exit = TRUE;
                    printf("\nexit\n");
                }
                break;

            default:
                printf("\ninvalid selection!\n");
                break;
        };

        if(be_exit)
        {
            break;
        }
    };

    printf("\n");
}

static char CHAR_TO_OCTET( char ch)
{
  char rc;
  
  if (ch>='0' && ch<='9')  //0-9
  {
    rc = ch - '0';
  }else if (ch>='A' && ch<='F') //A-F
  {
    rc = ch - 'A' + 10;
  }else if (ch>='a' && ch<='f') //a-f
  {
    rc = ch - 'a' + 10;
  }else
  {
    rc = -1;
  }

  return rc;
}

static int hex_to_bin
(
  const unsigned char * in_str,
  const int      in_str_len,
  unsigned char *         out_str,
  int              out_max_len
)
{
  int i,j;
  
  if (in_str_len%2 !=0) return -1;

  if (out_max_len < in_str_len/2) return -1;

  for (i=0,j=0; i<in_str_len; i+=2)
  {
    if ((CHAR_TO_OCTET(in_str[i]) == 0xFF) && (CHAR_TO_OCTET(in_str[i+1]) == 0xFF))
    {
        return -1;
    }
    out_str[j++] =CHAR_TO_OCTET(in_str[i])*16 + CHAR_TO_OCTET(in_str[i+1]);
  }
  return j;
  
}

static void simcom_sms_test()
{
    const char *options_list[] = {
        "1. Send ASCII SMS",
        "2. Send UTF8 SMS",
        "3. Send UCS2 SMS",
        "99. Back",
    };

    int opt = 0;
    int i,j;
    char scan_string[500] = {0};

    if(sms_init((sms_ind_cb_fcn)process_simcom_ind_message) != 0)
    {
        return;
    }
    while(1)
    {
    	 printf("\nPlease select received sms format(2. UTF8 3. UCS2).\n");
        printf("Option > ");
		
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        opt = atoi(scan_string);
	 if (opt == 2) {
	 	sms_set_format(2);
		break;
	 }
	 else if (opt == 3) {
	 	sms_set_format(3);
		break;
	 }
	 else
	 	continue;

    }
    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1: //ASCII
                {
                    char phone_num[25] = {0};
                    while(1)
                    {
                        printf("\nPlease input phone num >");
                        memset(scan_string, 0, sizeof(scan_string));
                        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                            continue; 
                        strcpy(phone_num, scan_string);
                        phone_num[strlen(phone_num)-1] = 0;
                        if (0 == strlen(phone_num))
                        {
                            continue;
                        }
                        i = 0;
                        if ('+' == phone_num[0]) 
                        {
                            i = 1; 
                        }
                        for(; i < strlen(phone_num); i++)
                        {
                            if(!isdigit(phone_num[i]))
                            {
                                break;
                            }
                        }
                        if (i == strlen(phone_num)) 
                            break;
                        else 
                        {
                            printf("Input err!\n ");
                            continue;
                        }
                    }
                    while(1)
                    {
                        printf("\nPlease input message content>");
                        memset(scan_string, 0, sizeof(scan_string));
                        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                            continue; 
                        scan_string[strlen(scan_string)-1] = 0;
                        if (0 == strlen(scan_string))
                        {
                            continue;
                        }
                        if (send_message(1,phone_num, (unsigned char *)scan_string, strlen(scan_string)) == 0)
                        {
                            printf("send sms successful!\n");
                        }
                        else
                        {
                            printf("send sms failed!\n");
                        }
                        break;
                    }
                }
                break;
            case 2:   //UTF8
                {
                    char phone_num[25] = {0};
                    while(1)
                    {
                        printf("\nPlease input phone num >");
                        memset(scan_string, 0, sizeof(scan_string));
                        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                            continue; 
                        strcpy(phone_num, scan_string);
                        phone_num[strlen(phone_num)-1] = 0;
                        if (0 == strlen(phone_num))
                        {
                            continue;
                        }
                        i = 0;
                        if ('+' == phone_num[0]) 
                        {
                            i = 1; 
                        }
                        for(; i < strlen(phone_num); i++)
                        {
                            if(!isdigit(phone_num[i]))
                            {
                                break;
                            }
                        }
                        if (i == strlen(phone_num)) 
                            break;
                        else 
                        {
                            printf("Input err!\n ");
                            continue;
                        }
                    }
                    while(1)
                    {
                        printf("\n***************************************************************************************\n");
                        printf("There was s mistake when get UTF8 chinese from console,so the sms comtent were settled ");
                        printf("\n***************************************************************************************\n");

                        printf("\nPlease input message content (Input ENTER,Send default strings automatically.)\n>");
                        memset(scan_string, 0, sizeof(scan_string));
                        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                            continue;
                        scan_string[strlen(scan_string)-1] = 0;
                        if (0 == strlen(scan_string))
                        {
                            if (send_message(2,phone_num, (unsigned char *)("你好!"), strlen("你好!")) == 0)
                            {
                                printf("send sms successful!\n");
                            }
                            else
                            {
                                printf("send sms failed!\n");
                            }
                        }
                        else
                        {
                            if (send_message(2,phone_num, (unsigned char *)scan_string, strlen(scan_string)) == 0)
                            {
                                printf("send sms successful!\n");
                            }
                            else
                            {
                                printf("send sms failed!\n");
                            }
                        }
                        break;
                    }
                }
                break;
            case 3:
                {
                    char phone_num[25] = {0};
                    unsigned char send_data[500] = {0};
                    int send_len; 
                    while(1)
                    {
                        printf("\nPlease input phone num >");
                        memset(scan_string, 0, sizeof(scan_string));
                        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                            continue; 
                        strcpy(phone_num, scan_string);
                        phone_num[strlen(phone_num)-1] = 0;
                        if (0 == strlen(phone_num))
                        {
                            continue;
                        }
                        i = 0;
                        if ('+' == phone_num[0]) 
                        {
                            i = 1; 
                        }
                        for(; i < strlen(phone_num); i++)
                        {
                            if(!isdigit(phone_num[i]))
                            {
                                break;
                            }
                        }
                        if (i == strlen(phone_num)) 
                            break;
                        else 
                        {
                            printf("Input err!\n ");
                            continue;
                        }
                    }
                    while(1)
                    {
                        printf("\nPlease input message content>");
                        memset(scan_string, 0, sizeof(scan_string));
                        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                            continue; 
                        scan_string[strlen(scan_string)-1] = 0;
                        if (0 == strlen(scan_string))
                        {
                            continue;
                        }
                        if (strlen(scan_string)%4 != 0)
                        {
                            printf("invalid unicode data!\n");
                            continue;
                        }
                        send_len = hex_to_bin((const unsigned char *)scan_string, strlen(scan_string), (unsigned char *)send_data, 500);
                        if (send_len == -1)
                        {
                            printf("invalid unicode data!\n");
                            continue;
                        }
                        if (send_message(3,phone_num, (unsigned char *)send_data, send_len) == 0)
                        {
                            printf("send sms successful!\n");
                        }
                        else
                        {
                            printf("send sms failed!\n");
                        }
                        break;
                    }

                }
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

    sms_deinit();
}

static void simcom_ota_test()
{
    const char *options_list[] = {
        "1. DownLoad (reserved)",
        "2. Update",
        "99. Back"
    };

    int opt = 0;
    int i,j;
    char scan_string[100] = {0};

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;
            

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                {
                    char ip[40] = {0};
                    int port;
                    char UsrName[60] = {0};
                    char Password[64] = {0};
                    char fileName[64] = {0};
                 
                    printf("\nPlease input FTP IP >");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue; 
                         memset(ip, 0, sizeof(ip));
                    if (strlen(scan_string) > 0 && strlen(scan_string) < 40)
                    {
                            strncpy(ip, scan_string, strlen(scan_string)-1);
                        }
                        else
                        {
                            strncpy(ip,scan_string,40-1);
                        }
                       
                    for(i = 0; i < strlen(ip)-1; i++)
                        {
                            if(!isdigit(ip[i]) && ip[i] != '.')
                            {
                                printf("invalid ip,i=%d,len=%d\n", i, strlen(ip));
                                break;
                            }
                        }

                        

                    printf("\nPlease input Port >");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue; 
                        
                    port = atoi(scan_string);

                    printf("\nPlease input UsrName >");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue; 
                        
                    memset(UsrName, 0, sizeof(UsrName));
                    if (strlen(scan_string) > 0 && strlen(scan_string) < 60)
                    {
                        strncpy(UsrName,scan_string,strlen(scan_string)-1);
                    }
                    else
                    {
                        strncpy(UsrName,scan_string,60-1);
                    }


                    printf("\nPlease input Password >");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;                        
            
                    memset(Password, 0, sizeof(Password));
                    if (strlen(scan_string) > 0 && strlen(scan_string) < 64)
                    {
                        strncpy(Password,scan_string,strlen(scan_string)-1);
                    }
                    else
                    {
                        strncpy(Password,scan_string,64-1);
                    }
                        
                         
                        
                    printf("\nPlease input filename >");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    memset(fileName, 0, sizeof(fileName));
                    if (strlen(scan_string) > 0 && strlen(scan_string) < 64)
                    {
                        strncpy(fileName,scan_string,strlen(scan_string)-1);
                    }
                    else
                    {
                        strncpy(fileName,scan_string,64-1);
                    }

                    //ota_downloadFile(ip,port,UsrName,Password,fileName,"/cache/update.zip");

                }
                break;
            case 2:
                {
                    ota_update();
                }
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

}

static void simcom_adc_test()
{
    const char *options_list[] = {
        "1. ADC1 value(voltage)",
        "2. ADC2 value(voltage)",	
        "99. Back"
    };

    int opt = 0;
    int i,j;
    char scan_string[100] = {0};

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                {
                    read_adc(1);
                }
                break;
            case 2:
                {
                    read_adc(2);
                }
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

}

static void simcom_i2c_test()
{
    const char *options_list[] = {
        "1. write slave device",
        "2. read from slave device",	
        "99. Back"
    };

    int opt = 0;
    int i,j;
    char scan_string[100] = {0};

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                {
                    uint8_t slave_addr;
                    uint16_t reg;
                    uint8_t *buf;
                    int len,i;
                    printf("\nPlease input slave addr format 0x34>"); //input the addr
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue; 
                    sscanf(scan_string, "%x", &slave_addr);
                    printf("slave_addr = 0x%x\n",slave_addr);
                    printf("\nPlease input slave device register format 0x25>"); //input the addr
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue; 
                    sscanf(scan_string, "%x", &reg);
                    printf("reg = 0x%x\n",reg);
                    printf("\nPlease input value format 0x2d>"); //input the addr
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue; 
                    buf = (uint8_t*)malloc((strlen(scan_string)-2)/2);
                    sscanf(scan_string, "%x", buf);
                    for(i = 0; i < (strlen(scan_string)-2)/2; i++)
                        printf("buf[%d]=0x%x\n",i,buf[i]);

                    printf("\nPlease input write len 1>"); //input the addr
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue; 
                    sscanf(scan_string, "%d", &len);
                    printf("len=%d\n",len);
                    sim_i2c_write(slave_addr, reg, buf, len);

                }
                break;
            case 2:
                {
                    uint8_t slave_addr;
                    uint16_t reg;
                    uint8_t *buf;
                    int len,i;
                    printf("\nPlease input slave addr format 0x34>"); //input the addr
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue; 
                    sscanf(scan_string, "%x", &slave_addr);
                    printf("slave_addr = 0x%x\n",slave_addr);
                    printf("\nPlease input slave device register format 0x24>"); //input the addr
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue; 
                    sscanf(scan_string, "%x", &reg);
                    printf("reg = 0x%x\n",reg);

                    printf("\nPlease input read len 2>"); //input the addr
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue; 
                    sscanf(scan_string, "%d", &len);
                    printf("len=%d\n",len);
                    buf = (uint8_t*)malloc(len);
                    sim_i2c_read(slave_addr, reg, buf, len);
                    for(i = 0; i < len; i++)
                        printf("buf[%d] = 0x%x\n", i,buf[i]);
                }
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

}

static void simcom_at_test()
{
    const char *options_list[] = {
        "1. get Module Version",
        "2. get CSQ",
        "3. get CREG",
        "4. get ICCID",
        "5. get IMEI",
        "6. get CIMI",
        "99. back"
    };

    int opt = 0;
    int i,j;
    int ret;
    char scan_string[100] = {0};
    char buff[100]; 

    ret = atctrl_init();
    if(ret != 0)
    {
        return;
    }

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                {
                    memset(buff,0,sizeof(buff));
                    getModuleRevision(buff, sizeof(buff));
                    printf("version: %s\n", (buff));
                }
                break;
            case 2:
                {
                    uint8_t rssi;
                    getCSQ(&rssi);
                    printf("csq: %d\n", rssi);
                }
                break;
            case 3:
                {
                    int reg_val;
                    reg_val = getCREG();
                    printf("reg: %d\n", reg_val);
                }
                break;
            case 4:
                {
                    memset(buff,0,sizeof(buff));
                    getICCID(buff, sizeof(buff));
                    printf("ICCID: %s\n", (buff));
                }
                break;
            case 5:
                {
                    memset(buff,0,sizeof(buff));
                    getIMEI(buff, sizeof(buff));
                    printf("IMEI: %s\n", (buff));
                }
                break;
            case 6:
                {
                    memset(buff,0,sizeof(buff));
                    getCIMI(buff, sizeof(buff));
                    printf("CIMI: %s\n", (buff));
                }
                break;

            default:
                break;
            }

        if(opt == 99)
        {
            break;
        }
    }

    atctrl_deinit();		
}
static void simcom_tts_test()
{
    const char *options_list[] = {
        "1. tts play",
        "2. tts stop",
        "3. get tts status",
        "4. set play path",
        "5. set tts parameter",
        "6. get tts parameter",
        "99. back",
    };

    int opt = 0;
    int i,j;
    char scan_string[512] = {0};

	simcom_init_tts();
    
    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:	
                {
                    TTS_CMD tts_action;

                    char tts_content[512] = {0};
                    
                    printf("\r\n please input text type(0-UCS2,1-GBK): >");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    if (atoi(scan_string) >= TTS_MAX_AT_CMD)
                    {
                        printf ("input value not support!\n");
                        continue;
                    }
                    tts_action = atoi(scan_string);
                    printf("\r\n please input text: >");
                    fgets(scan_string, sizeof(scan_string), stdin);

                    simcom_handle_input_string(scan_string, strlen(scan_string), tts_content, 512, 0);
                    
                    simcom_play_tts(tts_action, tts_content);
                }
                break;
            case 2:
                {
                    simcom_stop_tts();
                }
                break;
            case 3:
                {
                    printf("status(0-NONE,1-PLAY,2_STOP): %d\n", simcom_get_tts_state());
                }
                break;
            case 4:
                {
                    int nCurretPath = simcom_get_cdtam();
                    int nPath = 0;
                    
                    if(nCurretPath == 1) {
                        printf("Current tts play path is remote.\n");
                    }
                    else {
                        printf("Current tts play path is local.\n");
                    }
                    
                    printf("Please set tts play path = (0-local, 1-remote): >");
                    fgets(scan_string, sizeof(scan_string), stdin);

                    nPath = simcom_handle_input_number(scan_string, strlen(scan_string));
                    
                    if((nPath != 0) && (nPath != 1)) {
                        printf("Input tts path error!\n");
                        break;
                    }
                    simcom_set_cdtam(nPath);
                }
                break;
            case 5:
                {
                    int	volume=2, sysvolume=3, digitmode=0, pitch=1, speed=1;
                    int result;

                    printf("set tts parameter---volume(0-2)\n");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;
                    volume = simcom_handle_input_number(scan_string, strlen(scan_string));
                    if(volume < 0 || volume >2)
                    {
                        printf("input volume error!\n");
                        break;
                    }
                    printf("set tts parameter---sysvolume(0-3)\n");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                         continue;
                    sysvolume = simcom_handle_input_number(scan_string, strlen(scan_string));
                    if(sysvolume < 0 || sysvolume >3)
                     {
                        printf("input sysvolume error!\n");
                        break;
                     }

                    printf("set tts parameter---digitmode(0-3)\n");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                         continue;
                    digitmode = simcom_handle_input_number(scan_string, strlen(scan_string));
                    if(digitmode < 0 || digitmode >3)
                   {
                        printf("input digitmode error!\n");
                        break;
                   }

                    printf("set tts parameter---pitch(0-2)\n");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;
                    pitch = simcom_handle_input_number(scan_string, strlen(scan_string));
                    if(pitch < 0 || pitch >2)
                    {
                        printf("input pitch error!\n");
                        break;
                    }

                    printf("set tts parameter---speed(0-2)\n");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;
                     speed = simcom_handle_input_number(scan_string, strlen(scan_string));
                    if(speed < 0 || speed >2)
                    {
                        printf("input speed error!\n");
                        break;
                    }
                    printf("volume=%d,sysvolume=%d,digitmode=%d,pitch=%d,speed=%d\n",volume,sysvolume,digitmode,pitch,speed);
                    result = set_tts_parameter(volume,sysvolume,digitmode,pitch,speed);
                    if(result < 0)
                    {
                        printf("set tts parameter error!\n");
                    }
                    else
                    {
                       printf("set tts parmaeter OK!\n");
                    }

                }
                break;
             case 6:
                 {
                    int	volume=2, sysvolume=3, digitmode=0, pitch=1, speed=1;
                    int result;

                    printf("get tts parameter\n");
                    result = get_tts_parameter(&volume,&sysvolume,&digitmode,&pitch,&speed);
                    if(result < 0)
                    {
                        printf("get tts parameter error!\n");
                    }
                    else
                    {
                        printf("get tts parmaeter OK!\n\n");
                        printf("volume=%d,sysvolume=%d,digitmode=%d,pitch=%d,speed=%d\n",volume,sysvolume,digitmode,pitch,speed);
                    }
                 }
                break;
            case 99:
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            simcom_deinit_tts();
            break;
        }
    }

}


extern int modem_ri_notify_mcu(void);

static void simcom_gpio_test()
{
    const char *options_list[] = {
        "1. Open DTR(PIN72) Interrupt",
        "2. Close DTR(PIN72) Interrupt",
        "3. RI(PIN69) wakeup host",
        "99. Back"
    };
    int opt = 0;
    int ret = 0;
    int* tret = NULL;
    char scan_string[100] = {0};
    int wakeFds[2] = {-1,-1};
    pthread_t g_gpiodtr_thr;

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");

        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                if(wakeFds[1]  == -1){
                    pipe(wakeFds);  
                    fcntl(wakeFds[0], F_SETFL, O_NONBLOCK);
                    fcntl(wakeFds[1], F_SETFL, O_NONBLOCK);
                    printf("wakeFds[0]=%d, wakeFds[1]=%d\n", wakeFds[0], wakeFds[1]);
                    if((ret = pthread_create(&g_gpiodtr_thr,NULL,gipo_wakeup_init,&wakeFds[0])) != 0){
                        printf("gpio pthread_create error %d!\n",ret);
                    }
                }
                break;
            case 2:
                if(wakeFds[1] > 0){
                    int* pret = NULL;
                    write(wakeFds[1],"exit",5);
                    pthread_join(g_gpiodtr_thr,(void**)&pret);
                    close(wakeFds[0]);
                    close(wakeFds[1]);
                    wakeFds[0] = -1;
                    wakeFds[1] = -1;
                }
                break;
            case 3:
                modem_ri_notify_mcu();
                break;
            default:
                break;
        }

        if(opt == 99){
          if(wakeFds[1] > 0){
                  int* pret = NULL;
                  write(wakeFds[1],"exit",5);
                  pthread_join(g_gpiodtr_thr,(void**)&pret);
                  close(wakeFds[0]);
                  close(wakeFds[1]);
                  wakeFds[0] = -1;
                  wakeFds[1] = -1;
          }
          break;
        }
    }
}
extern void uart_main(void);
static void simcom_uart_test()
{
    uart_main();
}
extern void spi_main(void);
static void simcom_spi_test()
{
    spi_main();
}
#if 1
static void simcom_gps_test()
{
    char scan_string[20] = {0};
    uint8 type;
    int loop = 1;
    int opt;

    const char *options_list[] = {
      "1. XTRA Enable",
      "2. XTRA Disable",
      "3. Cold Start",
      "4. Hot Start",
      "5. Close GPS",
      "99. Back"
    };

    if(gps_init((gps_ind_cb_fcn)process_simcom_ind_message) != TRUE)
    {
        return;
    }
    while (loop)
    {
        /* Display menu of options. */
        printf("Please select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");         

        /* Read the option from the standard input. */
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
        continue;
        opt = atoi(scan_string);
        switch (opt)
        {
            /*=======================================================================================*/
            case 1:
            {
                gps_xtra_enable();
            }
            break;
            /*=======================================================================================*/
            case 2:
            {
                gps_xtra_disable();
            }
            break;
            /*=======================================================================================*/
            case 3:
            {
                if(gps_coldstart() == FALSE) {
                    printf("Location ColdStart Failed!\n");
                }
            }
            break;
            /*=======================================================================================*/
            case 4:
            {
                if(gps_hotstart() == FALSE) {
                    printf("Location HoldStart Failed!\n");
                }
            }
            break;
            /*=======================================================================================*/
            case 5:
            {
                if(gps_stop() == FALSE) {
                    printf("Location Stop Failed!\n");
                }
            }
            break;
            /*==================================================================================================*/
            case 99:
            {
                loop = 0;
            }
            break;
            default:
            break;
        }
    }
    gps_deinit();
}
#endif

/**********************************************************************************
         Bluetooth 
************************************************************************************/
static void bt_setTimespecRelative(struct timespec *p_ts, long long msec)
{
    struct timeval tv;
    gettimeofday(&tv, (struct timezone *) NULL);
    p_ts->tv_sec = tv.tv_sec + (msec / 1000);
    p_ts->tv_nsec = (tv.tv_usec + (msec % 1000) * 1000L ) * 1000L;
}

static void bt_wait_response(int timeout)
{
    struct timespec  ts;
    bt_setTimespecRelative(&ts, timeout * 1000 + 2000);
    pthread_cond_timedwait(&s_btcond, &s_btmutex, &ts);
}
static void bt_stop_wait_response(void)
{
    pthread_cond_signal(&s_btcond);
}

void simcom_process_response(bt_msg *recv_msg)
{
    //int result = 0;
    int i;
    char response[512] = {0};
    switch(recv_msg->command)
    {
        case BT_SEARCH_COMMAND:
            /* ------------------------------------
            扫描结果输出
            param1: search status: search end
            param2: search result index
            param3: rssi;
            address: device mac address
            data_len:  data name length
            data:   device name
            ---------------------------------------*/
            if(recv_msg->param1 == 0)
            {
                printf("BTSCAN: end\r\n");  // 扫描结束
            }
            else if(recv_msg->param1 == 1)
            {
                printf("BTSCAN:  %d, %s, %02X:%02X:%02X:%02X:%02X:%02X, %d\r\n",
                            recv_msg->param2, 
                            (recv_msg->data_len > 0) ? (char*)recv_msg->data : "unknow",
                            (recv_msg->address.nap >> 8) & 0xFF,
                            recv_msg->address.nap & 0xFF,
                            recv_msg->address.uap,
                            (recv_msg->address.lap >> 16) & 0xFF,
                            (recv_msg->address.lap >> 8) & 0xFF,
                            recv_msg->address.lap & 0xFF,
                            recv_msg->param3
                       );
            }
            break;


        case BT_BOND_COMMAND:
            {
                /* -----------------------------------
                配对返回结果
                param1: bond result 0: FAILED  1:SUCCESS
                address: device mac address
                data_len  device name length
                data:    if data_len > 0  deviceName
                ---------------------------------------*/           

                if(recv_msg->param1 == BT_BOND_SUCCESS)
                {
                    printf("+BTPAIR: %s, %02X:%02X:%02X:%02X:%02X:%02X\r\n",      
                                                (recv_msg->data_len > 0) ? (char*)recv_msg->data : "unknow", 
                                                (recv_msg->address.nap >> 8) & 0xFF,
                                                recv_msg->address.nap & 0xFF,
                                                recv_msg->address.uap,
                                                (recv_msg->address.lap >> 16) & 0xFF,
                                                (recv_msg->address.lap >> 8) & 0xFF,
                                                recv_msg->address.lap & 0xFF);
                }        
                else if(recv_msg->param1 == BT_BOND_FAILED)
                {
                    /* -----------bond fail end ---------*/
                    /*  --- result code -----
                    recv_msg->param2 is result code: 
                    CSR_BT_RESULT_CODE_SC_SUCCESS                (0x0000)
                    CSR_BT_RESULT_CODE_SC_BONDING_FAILED         (0x0001)
                    CSR_BT_RESULT_CODE_SC_BONDING_CANCELLED      (0x0002)
                    CSR_BT_RESULT_CODE_SC_SECURITY_FAIL          (0x0003)
                    CSR_BT_RESULT_CODE_SC_SECURITY_MODE_NOT_SET  (0x0004)
                    CSR_BT_RESULT_CODE_SC_INTERNAL_ERROR         (0x0005)
                    CSR_BT_RESULT_CODE_SC_ERROR_UNSPECIFIED      (0x0006)
                    CSR_BT_RESULT_CODE_SC_NO_SUCH_DEVICE         (0x0007)
                    CSR_BT_RESULT_CODE_SC_BONDING_ACCEPT_TIMEOUT (0x0008) */ 
                    printf("+BTPAIR:  pair fail\r\n"); 
                }
            }
            break;  
        case BT_ACCEPT_COMMAND:
            {   
                /*--------------------------------------------
                提示用户确认配对
                param1:  accept mode
                param2:  if ((param1 == ACCEPT_MODE_NOTIFICATION) || 
                             (param1 == ACCEPT_MODE_COMPARE) ||
                             (param1 == ACCEPT_MODE_PASSKEY))   
                          param2 is passkey value
                         else 
                            ignore
                            
                address: device address
                data_len:   data length
                data:    device name  (maybe null)
                --------------------------------------------*/
                if ((recv_msg->param1 == ACCEPT_MODE_NOTIFICATION) || 
                     (recv_msg->param1 == ACCEPT_MODE_COMPARE) ||
                     (recv_msg->param1 == ACCEPT_MODE_PASSKEY)) 
                {     
                    printf("+BTPAIRING: %d, %s, %02X:%02X:%02X:%02X:%02X:%02X, %d\r\n",
                                        recv_msg->param1, 
                                        (recv_msg->data_len > 0) ?  (char*)recv_msg->data :"unknow",
                                        (recv_msg->address.nap >> 8) & 0xFF,
                                        recv_msg->address.nap & 0xFF,
                                        recv_msg->address.uap,
                                        (recv_msg->address.lap >> 16) & 0xFF,
                                        (recv_msg->address.lap >> 8) & 0xFF,
                                        recv_msg->address.lap & 0xFF,
                                        recv_msg->param2);
                }
                else
                {
                    printf("+BTPAIRING: %d, %s, %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                                        recv_msg->param1, 
                                        (recv_msg->data_len > 0) ? (char*)recv_msg->data : "unknow",
                                        (recv_msg->address.nap >> 8) & 0xFF,
                                        recv_msg->address.nap & 0xFF,
                                        recv_msg->address.uap,
                                        (recv_msg->address.lap >> 16) & 0xFF,
                                        (recv_msg->address.lap >> 8) & 0xFF,
                                        recv_msg->address.lap & 0xFF);                 
                }
                
                if(recv_msg->param1 == ACCEPT_MODE_NOTIFICATION)
                {
                    /* just notify user the bonding information, please wait next msg */
                    break;                    
                }
                
                /*----- need  accept/reject/passkey  ----- */
                
                // because the item_test fgets will block the keyboard input.
                // goto main thread(item_test) to get keyboard input

                // 提示用户确认是否接受配对；
                memcpy(&s_bt_msg, recv_msg, sizeof(bt_msg));
                printf("If accept bond? (Y / N):\r\n"); 
                break;
                
            }            

        case BT_GET_BONDED_COMMAND:
        
            /*---------------------------------------------
             * 获取已配对设备列
             * param1 = total bonded number
             * param2 = current index;
             * param3 :  if total bonded unmber more than 0,  1: last one
             * address = devices;
             *-------------------------------------------------*/
            if(recv_msg->param1 == 0)
            {
                printf("+BTPAIRED: 0\r\n");
            }
            else
            {
                printf("+BTPAIRED: %d, %d, %s, %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                                            recv_msg->param1,
                                            recv_msg->param2,  
                                            (recv_msg->data_len > 0) ? (char *)recv_msg->data : (char *)"unknow",
                                            (recv_msg->address.nap >> 8) & 0xFF,
                                            recv_msg->address.nap & 0xFF,
                                            recv_msg->address.uap,
                                            (recv_msg->address.lap >> 16) & 0xFF,
                                            (recv_msg->address.lap >> 8) & 0xFF,
                                            recv_msg->address.lap & 0xFF);            
            }
            break; 
            
         case BT_CONNECT_COMMAND:
            /*---------------------------------------------
             * 其他蓝牙设备通过SPP连接模块，上报此消息
             * 参数说明:
             *  param1    0:连接失败       1:连接成功
             *  param2    spp:每次可以穿送的数据的最大长
             *-------------------------------------------------*/

            if(recv_msg->param1 == 0)
            {
                printf("spp connected fail!\n");
            }
            else
            {
                printf("spp connected success!  %02X:%02X:%02X:%02X:%02X:%02X\n",                                
                                            (recv_msg->address.nap >> 8) & 0xFF,
                                            recv_msg->address.nap & 0xFF,
                                            recv_msg->address.uap,
                                            (recv_msg->address.lap >> 16) & 0xFF,
                                            (recv_msg->address.lap >> 8) & 0xFF,
                                            recv_msg->address.lap & 0xFF); 
                printf("spp max data len: %d\n", recv_msg->param2);
            }

            break;

        case BT_DISCONNECT_COMMAND:
            /*---------------------------------------------
             * 非模块主动断开SPP连接，上报此消息
             * -------------------------------------------------*/
            printf("spp disconnected!\n");
            break;
            
        case BT_SPP_RECV_COMMAND:
            /*---------------------------------------------
             * SPP 收数
             * data_len:   receive data len
             * data:       receive data
             * -------------------------------------------------*/
            printf("recv data len: %d\r\n", recv_msg->data_len);
            printf("recv data:%s\r\n",recv_msg->data);
            break;            

        case BT_GATT_CONNECT_COMMAND:
             /*---------------------------------------------
              * GATT连接上，上报此消息
              * -------------------------------------------------*/
            if(recv_msg->param1 == 1)
            {
                printf("gatt connected.\r\n");
            }
            else
            {
                printf("gatt disconnected.\r\n");
            }
            break; 

        case BT_GATT_READ_IND_COMMAND:
             /*---------------------------------------------
              * 其他设备试图通过蓝牙读模块数据，上报此消息
              * -------------------------------------------------*/
            printf("+BT_GATT_READ_IND:handle:%d\r\n",recv_msg->attrHandle);
            memcpy(&s_bt_msg, recv_msg, sizeof(bt_msg));
            printf("IF Accept? Y/N\r\n");
            break;
            
        case BT_GATT_WRITE_IND_COMMAND:
            /*---------------------------------------------
             * 其他设备试图通过蓝牙写模块数据，上报此消息
             * -------------------------------------------------*/
            printf("+BT_GATT_WRITE_IND:handle:%d,",recv_msg->attrHandle);
            for(i=0;i<recv_msg->data_len;i++)
            {
                printf("%2X",recv_msg->data[i]);
            }
            memcpy(&s_bt_msg, recv_msg, sizeof(bt_msg));
            printf("\r\nIF Accept? Y/N\r\n");
            break;    
            
        case BT_GATT_NOTIFY_COMMAND:
            /*---------------------------------------------
             * NOTIFY 数据已发
             * -------------------------------------------------*/
            printf("gatt notify has been sent.\n");
            break;    
            
        case BT_GATT_INDICATION_COMMAND:
            /*---------------------------------------------
             * ind 数据已发
             * -------------------------------------------------*/
            printf("gatt indication has been sent.\n");
            break;        

        default:
            break;
    }
    
    //return result;
}


static void simcom_bt_test()
{
    const char* options_list[]=
    {
        " 1. Power",
        " 2. Gatt Register",
        " 3. Gatt Create DataBase",
        " 4. Create Service",
        " 5. Create Characteristic",
        " 6. Create Descriptor",
        " 7. Add Srvice to DataBase",
        " 8. Gatt Active",
        " 9. Gatt send notify",
        " 10. Gatt send indication",
        " 11. Get scan enable",
        " 12. Set scan enable",
        " 13. Host",
        " 14. SPP",
        " 99. Back"
    };

    int     array_size;
    char    scan_string[40];
    int     opt, i;
    int     bAccept = 0;
    int     ret;
    int inquiry_scan_status;
    int page_scan_status;       

    printf("bt_item_test\r\n");

    array_size = sizeof(options_list)/sizeof(options_list[0]);  

    /*--------------------------------------------------------------
     * 创建一个线程接收从蓝牙协议栈上报的返回值或消息等???管道的名称是固定的
     * -----------------------------------------------------------------*/

    ret = bt_init((bt_ind_cb_fcn)simcom_process_response);
    if(ret != 0)
    {
        return;
    }

    while (TRUE)
    {
        /* Display menu of options. */
        printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");


        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
        {
            continue;
        }    

        if((strncmp(scan_string, "y",1) == 0) || (strncmp(scan_string, "Y",1) == 0))
        {
            bAccept = 1;
        }
        else if((strncmp(scan_string, "n",1) == 0) || (strncmp(scan_string, "N",1) == 0))
        {
            bAccept = 0;
        }
        else 
        {
            bAccept = -1;
            if((scan_string[0] < '0')||(scan_string[0] > '9'))
            {
                continue;
            }            
        }


        if(bAccept >= 0)
        {
            /*-----------------------process bond accept or reject -------------------------------*/
            bt_msg send_msg;
            memset(&send_msg, 0, sizeof(bt_msg));

            if(s_bt_msg.command == BT_ACCEPT_COMMAND)
            {
                if(bAccept == 1 && s_bt_msg.param1 == ACCEPT_MODE_PASSKEY)
                {
                    int passkey = 0;

                    printf("\r\n please enter the passkey:");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
                    {
                        passkey = atoi(scan_string);
                    }
                    send_msg.param3 = passkey;          
                }
                
                else if(bAccept == 1 && s_bt_msg.param1 == ACCEPT_MODE_PINCODE)
                {
                    printf("\r\n please enter pincode the passkey(max len 16):");
                    memset(scan_string, 0, sizeof(scan_string));
                    fgets(scan_string, sizeof(scan_string), stdin);
                    strncpy((char*)send_msg.data, scan_string,strlen(scan_string)-1);
                }

                accept_bond(bAccept, (accept_mode)s_bt_msg.param1);
            }
            if(s_bt_msg.command == BT_GATT_READ_IND_COMMAND)
            {
               char data[50];
			   int i;
               if(bAccept==1)
               {
                    //CSR_BT_GATT_ACCESS_RES_SUCCESS  0
                    printf("\r\n please enter data to be read:");
                    memset(data, 0, sizeof(data));
                    fgets(data, sizeof(data), stdin);
					for(i=0;i<sizeof(data);i++)
						{
							if(0x0a==data[i])
								data[i]='\0';
						}
                    bt_gatt_read_cfm(s_bt_msg.attrHandle, 0, (uint8*)data, strlen(data));
                }
                else
                {
                    //#define CSR_BT_GATT_ACCESS_RES_READ_NOT_PERMITTED   2
                    bt_gatt_read_cfm(s_bt_msg.attrHandle, 2, NULL, 0);

                }
            }
            if(s_bt_msg.command==BT_GATT_WRITE_IND_COMMAND)
            {
                if(bAccept==1)
                {
                    bt_gatt_write_cfm(s_bt_msg.attrHandle, 3);
                }  
                else
                {
                    //CSR_BT_GATT_ACCESS_RES_WRITE_NOT_PERMITTED   3
                    bt_gatt_write_cfm(s_bt_msg.attrHandle, 3);
                }
                 
             }
            
            continue;
        }

        
        opt = atoi(scan_string);
        switch (opt)
        {
            case 1:
                {
                    int bPower;
                    printf("\r\n please select power status(1: power on 0: power off):");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    bPower = atoi(scan_string);
                    simcom_bt_power_on(bPower);
                }
                break;
            #if 0
            case BT_SEARCH_COMMAND:
                {
                    int timeout = 0;
                    int mode;
                    int bsubcmd;          
                    printf("\r\n please search cmd: 1: start  0: stop \r\n");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    bsubcmd = atoi(scan_string);
                    if(bsubcmd > 1 || bsubcmd < 0)
                    {
                        continue;
                    }
                    if(bsubcmd)
                    {
                        printf("\r\n search mode: 0: normal serach  1: hide paired:\r\n");
                        fgets(scan_string, sizeof(scan_string), stdin);
                        
                        mode = atoi(scan_string);
                        
                        if(mode > 1 || mode < 0)  
                        {
                            continue;
                        }

                        printf("\r\n search timeout:\r\n");
                        fgets(scan_string, sizeof(scan_string), stdin);
                        
                        timeout = atoi(scan_string);
                        
                        if(timeout > 60 || timeout < 0)
                        {
                            continue;
                        }
                    }
                    search_devices(bsubcmd,mode,timeout);
                    printf("\r\n");
                }
                break;

            case BT_IOCAP_COMMAND:
                {
                    int mode;
                    printf("\r\n please select io cap mode(0~3):");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    mode = atoi(scan_string);
                    if(mode > IO_CAP_NO_INPUT_NO_OUTPUT || mode < IO_CAP_DISPLAY_ONLY)
                    {
                        continue;
                    }   
                    ret = set_iocap_device(mode);
                    if(ret == 0)
                    {
                        printf("set io cap success!\n");
                    }
                    else
                    {
                        printf("set io cap fail!\n");
                    }
                }
                break;
                
            case BT_BOND_COMMAND:
                {
                    int index;
                    printf("\r\n please select bond id:");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    index = atoi(scan_string);
                    ret = bond_device(index);
                    if(ret < 0)
                    {
                        printf("bond_device fail!\n");
                    }
                }
                break;     

            case BT_DEBOND_COMMAND:
                {
                    int index;                    
                    printf("\r\n please select debond id:");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    index = atoi(scan_string);
                    ret = debond_device(index);
                    if(ret < 0)
                    {
                        printf("debond_device fail!\n");
                    }
                    else
                    {
                        printf("debond_device success!\n");
                    }
                }
                break;
                
            case BT_GET_BONDED_COMMAND:
                {
                    ret = get_bonded_device();
                    if(ret < 0)
                    {
                        printf("get_bonded_device fail!\n");
                    }                    
                }
                break;
            case BT_PIN_READ_COMMAND:
                {
                    int bsubcmd;       
                    char pin[50]={'\0'};
                    printf("\r\n please choose cmd: 1: set pin  0: read pin \r\n");

                    fgets(scan_string, sizeof(scan_string), stdin);
                    
                    bsubcmd = atoi(scan_string);
                    
                    if(bsubcmd > 1 || bsubcmd < 0)
                    {
                        continue;
                    }

                    if(bsubcmd)
                    {
                        printf("\r\n please input pin(numberic 4-16):\r\n");
              			    memset(scan_string, 0, sizeof(scan_string));
              			    memset(pin, 0, sizeof(pin));
                        fgets(scan_string, sizeof(scan_string), stdin);	
              			    if((strlen(scan_string)>=5 )&&(strlen(scan_string)<=17))
              			    {
                			   	  scan_string[strlen(scan_string)-1]='\0';
                            strncpy(pin, scan_string,strlen(scan_string));
              			    }
              			   else
              			   {
                			 	    printf("the length of pin is wrong,exit!!!\n");
                			  	  break;
              			   }
                        ret = pin_set(pin);
                        if(ret < 0)
                        {
                            printf("set pin fail!\n");
                        }
                        else
                        {
                            printf("set pin success!\n");
                        }
                    }
                    else
                    {
                        char pincode[32];
                        ret = pin_read(pincode);
                        if(ret < 0)
                        {
                            printf("read pin fail!\n");
                        }
                        else
                        {
                            printf("read pincode = %s.\n",pincode);
                        }                        
                    }
                }
                break;
            case BT_SPP_SERVER_COMMAND:
                {
                    int bActive;
                    printf("\r\n please set mode: 1: active  0:deactive\r\n");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    bActive = atoi(scan_string);
                    ret = spp_server(bActive);
                    if(ret < 0)
                    {
                        printf("spp_server fail!\n");
                    }
                    else
                    {
                        printf("spp_server success!\n");
                    }  
                }
                break;
            case BT_GET_PROFILE_COMMAND:
                {
                    int index;
                    int isSupportSPP;
                    printf("\r\n please select device id:\r\n");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    index = atoi(scan_string);
                    ret = bt_get_profile(index, &isSupportSPP);
                    if(ret < 0)
                    {
                        printf("bt_get_profile fail!\n");
                    }
                    else
                    {
                        printf("isSupportSPP = %d.\n", isSupportSPP);
                    }
                }
                break;
            case BT_CONNECT_COMMAND:
                {
                    int index;
                    printf("\r\n please select device id:\r\n");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    index = atoi(scan_string);
                    ret = spp_conncet_device(index);
                    if(ret < 0)
                    {
                        printf("spp_conncet_device fail!\n");
                    }
                    else
                    {
                        printf("spp_conncet_device success!\n");
                    } 
                }
                break;
            case BT_DISCONNECT_COMMAND:
                {
                    ret = spp_disconncet_device();
                    if(ret < 0)
                    {
                        printf("spp_disconncet_device fail!\n");
                    }
                    else
                    {
                        printf("spp_disconncet_device success!\n");
                    }                     
                }
                break;
            case BT_SPP_SEND_COMMAND:
                {
                    uint8_t data[2048] ={0};
                    uint16_t data_len;
                    printf("Please input the msg to send:");
                    if (fgets((char*)data, sizeof(data), stdin) != NULL)
                    {
                        data_len = strlen((char*)data) - 1;    //去掉最后一个回车字符
                        ret = spp_send_data(data, data_len);
                        if(ret < 0)
                        {
                            printf("spp_send_data fail!\r\n");
                        }
                        else
                        {
                            printf("spp_send_data success!\r\n");
                        }
                    }
                }
                break;
            case BT_SERVER_STATUS_COMMAND:
                {
                    int isActived;
                    ret = bt_get_server_status(&isActived);
                    if(ret < 0)
                    {
                        printf("bt_get_server_status fail!\n");
                    }
                    else
                    {
                        printf("spp server isActived =%d\n", isActived);
                    }
                }
                break;
            case BT_CONNECT_STATUS_COMMAND:
                {
                    int isConnected;
                    ret = bt_get_connect_status(&isConnected);
                    if(ret < 0)
                    {
                        printf("bt_get_connect_status fail!\n");
                    }
                    else
                    {
                        printf("spp  isConnected=%d\n", isConnected);
                    }
                }
                break;
            #endif
            case 2:
                {
                    int bRegister;
                    printf("\r\n please select cmd: (1: register 0: unregister) >");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    bRegister = atoi(scan_string);
                    ret = bt_gatt_register(bRegister);
                    if(ret < 0)
                    {
                        printf("bt gatt register fail!\r\n");
                    }
                    else
                    {
                        printf("bt gatt register success!\r\n");
                    }
                }            
                break;
            case BT_GATT_ADVERTISE_COMMAND:
                //bt_gatt_advertise(1);
                break;
            case 3:
                ret = bt_gatt_createdatabase(1);
                if(ret < 0)
                {
                    printf("create database fail!\r\n");
                }
                else
                {
                    printf("create database success!\r\n");
                }
                break;
            case 4:
                 {
                    int uuid,len;
                    unsigned char uuid128[16]={0};
                    char uuidtmp[2]={0};
                    printf("\r\n please select character uuid(hex,2bytes or 16bytes):>");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    len=strlen(scan_string)-1;
                    if(len==4)
                    {
                        uuid = strtoul(scan_string,NULL,16);
                    }
                    else if(len==32)
                    {
                        printf("len=32\n");
                        for(i=0;i<16;i++)
                        {
                            strncpy(uuidtmp,scan_string+2*i,2);
                            uuid128[15-i]=strtoul(uuidtmp,NULL,16);
                        }
                    }
                    else
                    {
                        printf("the length is incorrect!\r\n");
                        continue;
                    }
                    if(len == 4)
                    {
                        ret = bt_gatt_create_service_uuid16(uuid);
                    }
                    else
                    {
                        ret = bt_gatt_create_service_uuid128(uuid128);
                    }
                    if(ret < 0)
                    {
                        printf("bt gatt create service fail!\r\n");
                    }
                    else
                    {
                        printf("bt gatt create service success!\r\n");
                    } 
                }
                break;
            case 5:
                {
                    int pro,permission,uuid,len;
                    int pro_tmp, permission_tmp;
                    uint32_t attrHandle;
                    unsigned char uuid128[16];
                    printf("\r\n please select character uuid(hex,2bytes or 16bytes):>");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    len=strlen(scan_string)-1;
                    if(len==4)
                    {
                        uuid = strtoul(scan_string,NULL,16);
                    }
                    else if(len==32)
                    {
                        char uuidtmp[2];
                        for(i=0;i<16;i++)
                        {
                            strncpy(uuidtmp,scan_string+2*i,2);
                            uuid128[15-i]=strtoul(uuidtmp,NULL,16);
                        }
                    }
                    else
                    {
                        continue;
                    }
                    #if 0
                    printf("\r\n please select character property: \n(1:broadcast,2:read,4:write without respose8:write,16:notify,32:indicate,64:signed write,128:extended propertiers) >");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    pro = atoi(scan_string);
                    printf("\r\n please select character permission:\n (0:none,16:read,32:write,64:read encryption,128:read auth) >");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    permission = atoi(scan_string);
                    #else
                    pro = 0;
                    printf("\n please select character property:broadcast (0:  disable    1: enable)>");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    pro_tmp = atoi(scan_string);
                    if(pro_tmp)
                    {
                        pro = pro | 0x01;
                    }

                    printf("\n please select character property:read (0:  disable    1: enable)>");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    pro_tmp = atoi(scan_string);
                    if(pro_tmp)
                    {
                        pro = pro | 0x02;
                    }

                    printf("\n please select character property:write without respose (0:  disable    1: enable)>");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    pro_tmp = atoi(scan_string);
                    if(pro_tmp)
                    {
                        pro = pro | 0x04;
                    }

                    printf("\n please select character property:write (0:  disable    1: enable)>");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    pro_tmp = atoi(scan_string);
                    if(pro_tmp)
                    {
                        pro = pro | 0x08;
                    }

                    printf("\n please select character property:notify (0:  disable    1: enable)>");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    pro_tmp = atoi(scan_string);
                    if(pro_tmp)
                    {
                        pro = pro | 0x10;
                    }

                    printf("\n please select character property:indicate (0:  disable    1: enable)>");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    pro_tmp = atoi(scan_string);
                    if(pro_tmp)
                    {
                        pro = pro | 0x20;
                    }

                    permission_tmp = 0;
                    // printf("\r\n please select character permission:\n (0:none,16:read,32:write,64:read encryption,128:read auth) >");
                    printf("\n please select permission : (0: none  1: read  2: write  3: read and write)>");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    permission_tmp = atoi(scan_string);
                    if(permission_tmp == 0)
                    {
                        permission = 0;
                    }
                    else if(permission_tmp == 1)
                    {
                        permission = 0x10;
                    }
                    else if(permission_tmp == 2)
                    {
                        permission = 0x20;
                    }
                    else if(permission_tmp == 3)
                    {
                        permission = 0x10|0x20;
                    }
                    #endif
                    if(len == 4)
                    {
                        ret = bt_gatt_create_characteristic_uuid16(uuid,pro,permission, &attrHandle);
                    }
                    else
                    {
                        ret = bt_gatt_create_characteristic_uuid128(uuid128,pro,permission, &attrHandle);
                    }

                    if(ret < 0)
                    {
                        printf("bt gatt create characteristic fail!\r\n");
                    }
                    else
                    {
                        printf("bt gatt create characteristic success! attrHandle = %d.\n", attrHandle);
                    }
                }
                break;
            case 6:
                ret = bt_gatt_create_descriptor();
                if(ret < 0)
                {
                    printf("bt gatt create descriptor fail!\r\n");
                }
                else
                {
                    printf("bt gatt create descriptor success!\r\n");
                }

                break;
            case 7:
                ret = bt_gatt_add_service_2_db();
                if(ret < 0)
                {
                    printf("bt gatt add service to database fail!\r\n");
                }
                else
                {
                    printf("bt gatt add service to database success!\r\n");
                }

                break;
            case 8:
                ret = bt_gatt_active(1);
                if(ret < 0)
                {
                    printf("bt gatt active fail!\r\n");
                }
                else
                {
                    printf("bt gatt active success!\r\n");
                }
                break;
            case 9:
                {
                    int attrhandle;
                    char data[50];
                    printf("\r\n please input attrhandle: >");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    attrhandle = atoi(scan_string);
                    printf("\r\n please input data to be notified: >");
                    memset(data, 0, sizeof(data));
                    fgets(data, sizeof(data), stdin);
                    ret = bt_gatt_notification(attrhandle,data);
                    if(ret < 0)
                    {
                        printf("bt gatt notification fail!\r\n");
                    }
                    else
                    {
                        printf("bt gatt notification success!\r\n");
                    }                    
                }
                break;
            case 10:
                {
                    int attrhandle;
                    char data[50];
                    printf("\r\n please input attrhandle: >");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    attrhandle = atoi(scan_string);
                    printf("\r\n please input data to be notified: >");
                    memset(data, 0, sizeof(data));
                    fgets(data, sizeof(data), stdin);
                    ret = bt_gatt_indication(attrhandle,data);
                    if(ret < 0)
                    {
                        printf("bt gatt indication fail!\r\n");
                    }
                    else
                    {
                        printf("bt gatt indication success!\r\n");
                    }
                }
                break;
            case 11:
                {
                    //int inquiry_scan_status;
                    //int page_scan_status;
                    ret = bt_get_scan_enable(&inquiry_scan_status, &page_scan_status);
                    if(ret < 0)
                    {
                        printf("bt get scan enable fail!\r\n");
                    }
                    else
                    {
                        printf("inquiry scan status = %d,  page_scan_status = %d.\n", inquiry_scan_status, page_scan_status);
                    }
                }
                break;
            case 12:
                //int inquiry_scan_status;
                //int page_scan_status;       
                printf("\r\n please input inquiry scan status: 0 off  1 on >");
                fgets(scan_string, sizeof(scan_string), stdin);
                inquiry_scan_status = atoi(scan_string);

                printf("\r\n please input page scan status: 0 off  1 on >");
                fgets(scan_string, sizeof(scan_string), stdin);
                page_scan_status = atoi(scan_string);

                bt_set_scan_enable(inquiry_scan_status, page_scan_status);
                if(ret < 0)
                {
                    printf("bt set scan enable fail!\r\n");
                }
                else
                {
                    printf("bt set scan enable success!\r\n");
                }
                break;
            case 13:  
                {
                    int cmd;
                    char devicename[128] = {0};
                    SIMCOM_BT_ADDR_T addr;
                    printf("\r\n get hostname mac (0) set hostname(1):");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    cmd = atoi(scan_string);
                    if(cmd == 1)
                    {
                        printf("please input new bt name:\r\n");
                        fgets(devicename, sizeof(devicename), stdin);
                        if((strlen(devicename) <= 1) || (strlen(devicename) > 64)){
                          printf("error: bt name length must be (1-63)\r\n");                          
                          break;
                        }

                        devicename[strlen(devicename)-1]='\0';
                        
                        ret = host_device(devicename); 
                        if(ret < 0)
                        {
                            printf("set bluetooth name fail!\n");
                        }
                        else
                        {
                            printf("set bluetooth name success!\n");
                        }
                    }
                    else
                    {
                        host_device_addr(devicename, &addr);
                        if(ret < 0)
                        {
                            printf("get bluetooth name and mac fail!\n");
                        } 
                        else
                        {
                            printf("name=%s, %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                                            devicename, 
                                            (addr.nap >> 8) & 0xFF,
                                            addr.nap & 0xFF,
                                            addr.uap,
                                            (addr.lap >> 16) & 0xFF,
                                            (addr.lap >> 8) & 0xFF,
                                            addr.lap & 0xFF);
                        }
                    }
                }
                break;
            case 14:
                printf("Currently not open for use.\r\n");
                break;
            #if 0
            case BT_SET_INQUIRY_SCAN_COMMAND:
                {
                    int interval,window;
                    printf("\r\n please input interval: (18-4096 1=0.625ms even only) >");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    interval = atoi(scan_string);
                    printf("\r\n please input window: (17-4096 1=0.625ms) >");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    window = atoi(scan_string);
                    bt_ctrl->bt_set_inquiry_scan(interval,window);  
                }
                break;
            #endif
            case 99:
                break;

            default :
                break;

        }

        if(opt == 99)
            break;
    }

}	

#ifndef _SIMCOM_WINDOWS_
static void simcom_timer_test()
{
    const char *options_list[] = {
        "1. set timer interval",
        "2. create timer",
        "99. back",
    };

    int opt = 0;
    int i,j;
    char scan_string[100] = {0};

    set_freq_nanosecs(1000000000); //default 1s

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:	
            	{
                    printf("\r\n please input timer interval(ns): >");
                    fgets(scan_string, sizeof(scan_string), stdin);
                    set_freq_nanosecs(atoll(scan_string));
                }
                break;
            case 2:
            {
                create_timer();
                char cmd;
                sigset_t mask;
                while(1)
                {
                    scanf("%c", &cmd);
                    switch(cmd)
                    {
                        case '1':
                            /* Block timer signal temporarily */

                            printf("Blocking signal %d\n", SIG);
                            sigemptyset(&mask);
                            sigaddset(&mask, SIG);
                            if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
                                printf("sigprocmask fail!\n");
                            break;
                        case '2':
                            /* Unlock the timer signal, so that timer notification
                            can be delivered */

                            printf("Unblocking signal %d\n", SIG);
                            if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
                                printf("sigprocmask fail!\n");
                            break;
                        default:
                            break;
                    }
                    if(cmd == 'q')
                    {
                        destroy_timer();
                        scanf("%c", &cmd);
                        break;
                    }
                }

            }
            case 99:
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }
}

#else
timer_t timerID_1000 = 0;
timer_t timerID_10000 = 0;

void* app_timer_ind_cb_fcn_1000(union sigval v)  
{  
    DEBUGLOG("app_timer_ind_cb_fcn_1000()[%d][%d]\n", v.sival_int, timerID_1000);
}
void* app_timer_ind_cb_fcn_10000(union sigval v)  
{  
    DEBUGLOG("app_timer_ind_cb_fcn_10000()[%d][%d]\n", v.sival_int, timerID_10000);
    app_stopTimer(timerID_10000);
    timerID_10000 = 0;
}

static void simcom_timer_test()
{
    const char *options_list[] = {
        "1. create 1  second  timer",
        "2. stop   1  second  timer",
        "3. create 10 seconds timer",
        "99. back",
    };

    app_timer_init();
  
    int opt = 0;
    int i,j;
    char scan_string[100] = {0};

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                {
                    if(timerID_1000 == 0)
                        timerID_1000 = app_startTimer(1000, (timer_ind_cb_fcn)app_timer_ind_cb_fcn_1000);
                }
                break;
            case 2:
                {
                    if(timerID_1000 != 0){
                        app_stopTimer(timerID_1000);
                        timerID_1000 = 0;
                    }
                }
                break;
            case 3:
                {
                    if(timerID_10000 == 0)
                    timerID_10000 = app_startTimer(10000, (timer_ind_cb_fcn)app_timer_ind_cb_fcn_10000);
                }
            case 99:
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            timerID_1000 = 0;
            timerID_10000 = 0;
            break;
        }
    }

    app_timer_deinit();
}

#endif
extern int TCPTest(char *serverIP, int serverPort, uint8_t *send_data, uint32_t send_len);
extern int app_udp_send_data(char *serverIP, int serverPort, uint8_t *send_data, uint32_t send_len);
static void simcom_tcp_test()
{
    char ip[20] = {0};
    int port;
    const char *options_list[] = {
        "1. send and recv tcp data",
        "2. send and recv udp data",
        "99. back",		
    };

    int ret;
    int i;
    int opt = 0;
    char    scan_string[1024+1];
    char    send_string[1024+1];

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                {
                    printf("\nPlease input server ip>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    continue;

                    strncpy(ip, scan_string, (sizeof(ip) - 1));

                    for(i = 0; i < strlen(ip)-1; i++)
                    {
                        if(!isdigit(ip[i]) && ip[i] != '.')
                        {
                            printf("invalid ip,i=%d,len=%d\n", i, strlen(ip));
                            break;
                        }
                    }
                    printf("\nPlease input server port>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    port = atoi(scan_string);

                    printf("\nPlease input send data(Maximum 1024)>");
                    memset(scan_string, 0, sizeof(scan_string));
                    memset(send_string, 0, sizeof(send_string));
                    
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;
                    
                    int nret = simcom_handle_input_string(scan_string, strlen(scan_string), send_string, 1024, 0);
                    if(nret == 0){
                        TCPTest(ip, port, (uint8_t *)send_string, strlen(send_string));
                    }else if(nret ==1){
                        printf("the intput data is too long.\n");
                    }
                    else{
                        printf("intput error!\n");
                    } 
                }
                break;
            case 2:
                {
                    printf("\nPlease input server ip>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    continue;

                    strncpy(ip, scan_string, (sizeof(ip) - 1));

                    for(i = 0; i < strlen(ip)-1; i++)
                    {
                        if(!isdigit(ip[i]) && ip[i] != '.')
                        {
                            printf("invalid ip,i=%d,len=%d\n", i, strlen(ip));
                            break;
                        }
                    }
                    printf("\nPlease input server port>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    port = atoi(scan_string);

                    printf("\nPlease input send data(Maximum 1024)>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    int nret = simcom_handle_input_string(scan_string, strlen(scan_string), send_string, 1024, 0);
                    if(nret == 0){
                        app_udp_send_data(ip, port, (uint8_t *)send_string, strlen(send_string));
                    }else if(nret ==1){
                        printf("intput data is too long.\n");
                    }
                    else{
                        printf("intput error!\n");
                    }                     
                }
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

}

static void simcom_uim_test()
{
    SimCard_Status_type sim_status;
    const char *options_list[] = {
        "1. get simcard Info",
        "2. get iccid",
        "3. get imsi",
        "99. back",		
    };

    int ret;
    int opt = 0;
    int i,j;
    char scan_string[100] = {0};

    if(uim_init() != 0)
    {
        return;
    }

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
    	print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                {
                /* 获取SIM 卡 信息*/

                    memset(&sim_status, 0, sizeof(SimCard_Status_type));
                    ret = getSimCardStatus(&sim_status);

                    printf("card_status:%d   (0:not exist 1: Exist: 2: UNKNOW)\n", sim_status.card_status);
                    printf("app_type:   %d   (0:UNKNOWN 1: SIM 2: USIM 3: RUIM 4: CSIM 5: ISIM)\n",sim_status.app_type);

                    printf("\napp_state define: (0:unknown  1: detected      2: pin block          3: puk block)\n");
                    printf("app_state define: (4: person check  5:Permanently blocked 6: illgeal 7: Ready)\n");
                    printf("app_state:  %d   \n\n", sim_status.app_state);

                    printf("\npin_state define: (0: unknown  1: Enabled and not verified 2: Enabled and verified)\n");
                    printf("pin_state define: (3: Disabled 4: Blocked                  5:Permanently blocked)\n");
                    printf("pin_state:  %d   \n\n", sim_status.pin.pin_state);

                    printf("pin_retries:%d\n", sim_status.pin.pin_retries);
                    printf("puk_retries:%d\n", sim_status.pin.puk_retries);

                }
                break;
            case 2:
                {
                    char iccid[21] = {0};
                    ret = get_iccid(iccid);
                    if(ret != 0)
                    {
                    printf("get iccid fail!\n");
                    break;
                    }
                    printf("iccid:%s\n", iccid);
                }
                break;
            case 3:
                {
                    char imsi[33] = {0};
                    ret = get_imsi(imsi);
                    if(ret != 0)
                    {
                        printf("get imsi fail!\n");
                        break;
                    }
                    printf("imsi:%s\n", imsi);
                }
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

    uim_deinit();
}

static void simcom_dms_test()
{
    const char *options_list[] = {
        "1. get imei",
        "2. get meid",
        "3. get rev_id",
        "4. set operate mode",
        "5. get operate mode",
        "99. back",
    };

    int ret;
    int opt = 0;
    int i,j;
    char scan_string[100] = {0};

    if(dms_init() != 0)
    {
        return;
    }

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                {
                    /*IMEI*/
                    char imei[33] = {0};
                    ret = get_imei(imei);
                    if(ret != 0)
                    {
                        printf("get imei fail!\n");
                        break;
                    }
                    printf("IMEI:%s\n", imei); 
                }
                break;
            case 2:
                {
                    /* MEID*/
                    char meid[33] = {0};
                    ret = get_meid(meid);
                    if(ret != 0)
                    {
                        printf("get meid fail!\n");
                        break;
                    }
                    printf("MEID:%s\n", meid);
                }
                break;

            case 3:
                {
                    /* rev_id*/
                    char revid[150] = {0};
                    ret = get_rev_id(revid);
                    if(ret != 0)
                    {
                        printf("get rev_id fail!\n");
                        break;
                    }
                    printf("rev_id:%s\n", revid);
                }
                break;

            case 4:
                {
                    /* operating_mode*/
                    char mode = 0;
                    printf("\nPlease input mode value\n");
                    printf(" 0 Online      (AT+CFUN=1) \n");
                    printf(" 1 Low power   (AT+CFUN=0) \n");
                    printf(" 2 Factory Test mode\n 3 Offline\n 4 Resetting\n 5 Shutting down\n 6 Persistent low power\n 7 Mode-only low power\n");
                    memset(scan_string, 0, sizeof(scan_string));
                    if(fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;

                    /* Convert the option to an integer, and switch on the option entered. */
                    mode = atoi(scan_string);


                    ret = dms_set_operating_mode(mode);
                    if(ret != 0)
                    {
                        printf("set operate mode fail!\n");
                        break;
                    }

                }
                break;
            case 5:
                {
                    /* operating_mode*/
                    char mode = 0;

                    ret = dms_get_operating_mode(&mode);
                    if(ret != 0)
                    {
                        printf("get operate mode fail!\n");
                        break;
                    }
                    printf("operate mode: %d\n", mode);

                }
                break;

            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

    dms_deinit();
}


static void simcom_get_version()
{
    const char *options_list[] = {
        "1. get sdk lib version",
        "2. get demo version",
        "99. back",		
    };

    int ret;
    int opt = 0;
    int i,j;
    char scan_string[100] = {0};


    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
            {
                char *sdk_version;
                sdk_version = (char *)get_simcom_sdk_version();

                printf("sdk lib version: %s\n", sdk_version);
                }
                break;
            case 2:
                {
                    char *demo_version;
                    demo_version = (char *)get_simcom_demo_version();

                    printf("demo version: %s\n", demo_version);
                }
                break;
            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

}

static void simcom_eth_test()
{
    const char *options_list[] = {
        "1. install drivers",
        "2. uninstall drivers",
        "3. set mac address",
        "4. set ip address",
        "99. back",
    };

    int ret;
    int opt = 0;
    int i,j;
    char scan_string[100] = {0};
    char command[128] = {0};
    ethernet_type_info type = ETH_TYPE_UNKNOWN;
    int tmp_val;

    while(1)
    {

        if(type == ETH_TYPE_UNKNOWN)
        {
            printf("\nPlease select eth type (1.  TYPE_BCM898XX  2. TYPE_AT803X)\n");
            memset(scan_string, 0, sizeof(scan_string));
            if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                continue;        
            tmp_val = atoi(scan_string);
            if(tmp_val != ETH_TYPE_BCM898XX && tmp_val != ETH_TYPE_AT803X)
            {
                continue;
            }
            else
            {
                type = (ethernet_type_info)tmp_val;
                eth_init(type);
            }
        }
        
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                {
                    Install_driver();
                    printf("eth install driver end.\n");
                }
                break;
            case 2:
                {
                    Uninstall_driver();
                    printf("eth install driver end.\n");
                }
                break;
            case 3:
                {

                    int ret;
                    char mac_address[16] = {0};
                    uint8_t mac_addr_digit[6] = {0};
                    int get_mac_from_nv_success = 0;
                    
                    if(dms_init() != 0)
                    {
                        printf("dms init fail!\n");
                    }
                    else
                    {
                        ret = get_mac_address_from_nv(DMS_DEVICE_MAC_LAN_V01, mac_addr_digit);
                        if(ret == 0)
                        {
                            sprintf(mac_address, "%02X:%02X:%02X:%02X:%02X:%02X", 
                                                mac_addr_digit[0],
                                                mac_addr_digit[1],
                                                mac_addr_digit[2],
                                                mac_addr_digit[3],
                                                mac_addr_digit[4],
                                                mac_addr_digit[5]);

                            get_mac_from_nv_success = 1;
                        }
                        dms_deinit();
                    }
                    if(!get_mac_from_nv_success)
                    {
                        //先写入一个随机值做测试
                        strcpy(mac_address,"00:24:1D:AF:3D:F2");
                    }

                    set_eth_mac_address(mac_address);
                    printf("eth set mac address end!  addr=%s\n", mac_address);
                }
                break;
            case 4:
                {
                    char *ip_addr = "192.168.1.1";
                    set_eth_ip_address(ip_addr);
                    printf("eth set ip address end!  addr=%s\n", ip_addr);
                }
                break;

            default:
            break;
        }

        if(opt == 99)
        {
            break;
        }
    }

}


static char ftp_dl_save_path[128];
static int  ftp_dl_save_handle;
static long long recv_len = 0;
void process_ftp_dl_recv_data(void *data, int data_len, int index, long long total_size)
{
    int write_len;

    if(index == 1 && strlen(ftp_dl_save_path) > 0)
    {
        ftp_dl_save_handle = open(ftp_dl_save_path,  O_WRONLY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
        recv_len = data_len;
    }
    if(ftp_dl_save_handle < 0)
    {
        return;
    }

    if(index >= 1)
    {
        recv_len+=data_len;
        printf("\r\033[k"); 
        printf("pack index: %d,  recv = %lld/%lld  %%%d", index, recv_len, total_size, recv_len * 100 /total_size);
    }

    if(data_len > 0)
    {
        write_len = write(ftp_dl_save_handle, data, data_len);
    }
    
    if(index == 0 || data_len == 0)  // recv end
    {
        close(ftp_dl_save_handle);
        printf("\nclose write file handle.\n");
    }    
}

static void simcom_ftp_test()
{
    const char *options_list[] = {
        "1. Init",
        "2. Login",
        "3. Logout",
        "4. Print work directory",
        "5. List",
        "6. Change work directory",
        "7. Mkdir",
        "8. Rename or move",
        "9. Delete file",
        "10. Delete folder",
        "11  Get file size",
        "12. download file",
        "13. upload file",
        "99. back",
    };

    int i;
    int ret;
    int ctrl_sock = -1;
    int bLoginSuccess = 0;
    int opt = 0;
    char ip[20] = {0};
    char scan_string[128] = {0};
    char command[128] = {0};
    int tmp_val;

    char host[32]={0};
    char user[32]={0};
    char password[32]={0};
    char* list_data[20480] = {0};
    while(1)
    {
     
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        
        switch(opt)
        {
            case 1:
                {
                    printf("please input IP address\n");
                    printf("For example:xx.xx.xx.xx\n");
                    printf("please input>");
                    fgets(host, sizeof(host), stdin);
    
                    printf("please input user name\n");
                    printf("For example:xxx\n");
                    printf("please input>");
                    fgets(user, sizeof(user), stdin);
     
                    printf("please input password\n");
                    printf("For example:**********\n");
                    printf("please input>");
                    fgets(password, sizeof(password), stdin);
                    
                    ftp_param_init(host,21,user,password);
                }
                break;
            case 2:
                {
                    ftp_connect();
                    ret = ftp_login();
                    if(ret == 0)
                    {
                        printf("Login Success!\n");
                    }
                    else
                    {
                        printf("Login Fail\n");
                    }
                }
                break;                
            case 3:                     
                {
                    if(get_connect_status())
                    {      
                        ftp_logout();
                      printf("ftp logout\n");
                    }    
                }
                break;
            case 4:      //Print work directory
                {
                    char  path[256] = {0};
                    if(!get_connect_status())
                    {
                        ftp_connect();
                        ftp_login();                       
                    }
                    
                    ret = ftp_pwd(path);
                    if(ret != 0)
                    {
                        printf("ftp_pwd fail!\n");
                        break;
                    }
                    printf("current path:%s\n", path);
                }
                break;  
            case 5:     //List
                {
                    
                    char  path[1024] = {0};
                    unsigned long long data_len;
                    
                    if(!get_connect_status())
                    {
                        ftp_connect();
                        ftp_login();                       
                    }

                    ret = ftp_pwd(path);
                    if(ret != 0)
                    {
                        printf("ftp_pwd fail!\n");
                        break;
                    }
                    memset(list_data, 0, sizeof(list_data));
                    ret = ftp_list(path, (void *)list_data, sizeof(list_data), &data_len);
                    if(ret != 0)
                    {
                        printf("ftp list fail!, ret = %d\n", ret);
                    }
                    else
                    {
                        printf("\n%s\n", list_data);
                    }                    
                }
                break;
            case 6:       //change work dir
                {
                    char  path[1024] = {0};
                    unsigned long long data_len;
                    printf("\nPlease input directory\n");
                    printf("  /  ------> prefix means absolute path, or else relative path\n");  
                    printf("For example:  /test    test    ../test \n");
                    printf("please input>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;                  
                    memcpy(path, scan_string, strlen(scan_string) - 1);
                    if(!get_connect_status())
                    {
                        ftp_connect();
                        ftp_login();                       
                    }                    
                    ret = ftp_cwd(path);
                    if(ret != 0)
                    {
                        printf("change work directory fail!\n");
                    }
                    else
                    {
                        printf("change work directory success!\n");
                    }
                }
                break;
            case 7:         //mkdir
                {
                    char  path[1024] = {0};
                    unsigned long long data_len;
                    printf("\nPlease input directory\n");
                    printf("  /  ------> prefix means absolute path, or else relative path\n");  
                    printf("For example:  /test    test    ../test \n");
                    printf("please input>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;                  
                    memcpy(path, scan_string, strlen(scan_string) - 1);
                    if(!get_connect_status())
                    {
                        ftp_connect();
                        ftp_login();                       
                    }                    
                    ret = ftp_mkdir(path);
                    if(ret != 0)
                    {
                        printf("create fail!\n");
                    }
                    else
                    {
                        printf("create success!\n");
                    }
                }
                break;

            case 8:     //rename move
                {
                    char  s_path[1024] = {0};
                    char  d_path[1024] = {0};
                    unsigned long long data_len;
                    printf("\nPlease input source directory or filename\n");
                    printf("  /  ------> prefix means absolute path, or else relative path\n");  
                    printf("For example:  /test    test   test/test.ini  ../test \n");
                    printf("please input>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;                  
                    memcpy(s_path, scan_string, strlen(scan_string) - 1);

                    printf("\nPlease input destination directory or filename\n");
                    printf("please input>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;                  
                    memcpy(d_path, scan_string, strlen(scan_string) - 1);
                    if(!get_connect_status())
                    {
                        ftp_connect();
                        ftp_login();                       
                    }                    
                    ret = ftp_mv(s_path, d_path);
                    if(ret != 0)
                    {
                        printf("rename or move fail!\n");
                    }
                    else
                    {
                        printf("rename or move success!\n");
                    }
                }
                break;                
            case 9:        //delete file
                {
                    char  path[1024] = {0};
                    unsigned long long data_len;
                    printf("\nPlease input file\n");
                    printf("  /  ------> prefix means absolute path, or else relative path\n");  
                    printf("For example:  /test/test.ini   test.ini  ../test/test.ini \n");
                    printf("please input>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;                  
                    memcpy(path, scan_string, strlen(scan_string) - 1);
                    if(!get_connect_status())
                    {
                        ftp_connect();
                        ftp_login();                       
                    }                    
                    ret = ftp_deltefile(path);
                    if(ret != 0)
                    {
                        printf("change work directory fail!\n");
                    }
                    else
                    {
                        printf("change work directory success!\n");
                    }
                }
                break;            
            case 10:   //ftp_delete folder
                {
                    char  path[1024] = {0};
                    unsigned long long data_len;
                    printf("\nPlease input file\n");
                    printf("  /  ------> prefix means absolute path, or else relative path\n");  
                    printf("For example:  /test  test  ../test/test.ini \n");
                    printf("please input>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;                  
                    memcpy(path, scan_string, strlen(scan_string) - 1);
                    if(!get_connect_status())
                    {
                        ftp_connect();
                        ftp_login();                       
                    }                    
                    ret = ftp_deletefolder(path);
                    if(ret != 0)
                    {
                        printf("change work directory fail!\n");
                    }
                    else
                    {
                        printf("change work directory success!\n");
                    }
                }
                break; 
                
            case 11:     // get file size
                {
                    char path[1024] = {0};
                    long long size;
                    printf("\nPlease input file path\n");
                    printf("  /  ------> prefix means absolute path, or else relative path\n");         
                    printf("For example:  /test/test.ini    test.ini\n");
                    printf("please input>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;                  
                    memcpy(path, scan_string, strlen(scan_string) - 1);
                    if(!get_connect_status())
                    {
                        ftp_connect();
                        ftp_login();                       
                    }
                    ret = ftp_size(path, &size);
                    if(ret != 0)
                    {
                        printf("get size fail!\n");
                    }
                    else
                    {
                        printf("size: %lld!\n", size);
                    }
                }
                break;
            case 12:
                {
                    unsigned long long  stor_size;
                    char *p;
                    char path[1024] = {0};
                    printf("\nPlease input file path\n");
                    printf("  /  ------> prefix means absolute path, or else relative path\n");         
                    printf("For example:  /test/test.ini    test.ini\n");
                    printf("please input>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;                  
                    memcpy(path, scan_string, strlen(scan_string) - 1);

                    p = strrchr(path,'/');
                    
                    if(p == NULL)
                    {
                        sprintf(ftp_dl_save_path, "/data/%s", path);
                    }
                    else
                    {
                        if(strlen(p+1) > 0)
                        {
                            sprintf(ftp_dl_save_path, "/data/%s", p+1);
                        }
                        else
                        {
                            sprintf(ftp_dl_save_path, "/data/%s", "unkonw_name");
                        }
                    }
                    if(!get_connect_status())
                    {
                        ftp_connect();
                        ftp_login();                       
                    }                    
                    ret = ftp_download(path,process_ftp_dl_recv_data);
                    if(ret != 0)
                    {
                        printf("download file fail!\n");
                    }
                    else
                    {
                        printf("download file success!\n");
                    }
                }
                break;
            case 13:
                {
                    unsigned long long  stor_size;
                    char *p;
                    char path[256] = {0};
                    char local_path[256] = {0};
                    printf("please input local file path(For example: /data/test.ini)>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;                  
                    memcpy(local_path, scan_string, strlen(scan_string) - 1);

                    
                    printf("\nPlease input file path\n");
                    printf("  /  ------> prefix means absolute path, or else relative path\n");         
                    printf("For example:  /test/test.ini    test.ini\n");
                    printf("please input>");
                    memset(scan_string, 0, sizeof(scan_string));
                    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                        continue;                  
                    memcpy(path, scan_string, strlen(scan_string) - 1);

                    p = strrchr(path,'/');
                    if(!get_connect_status())
                    {
                        ftp_connect();
                        ftp_login();                       
                    }                    
                    ret = ftp_upload(local_path, path);
                    
                    if(ret != 0)
                    {
                        printf("upload file fail!\n");
                    }
                    else
                    {
                        printf("upload file success!\n");
                    }
                }
                break;   
            
            case 99:
               {
                    if(get_connect_status())
                   { 
                       ftp_logout();
                    }
               }     
                break;

            default:
                break;
        }

        if(opt == 99)
        {
            break;
        }
    }

}

#ifdef FEATURE_NETWORK_SDK
static void simcom_ssl_test()
{
    const char *options_list[] = {
        "1. Start SSL Client oneway",
        "2. Start SSL Client twoway",
        "3. Start SSL Server oneway",
        "4. Start SSL Server twoway",
        "99. back",
    };
    int opt = 0;
    char scan_string[100] = {0};

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
				  ssl_client_oneway();
                break;
            case 2:
				  ssl_client_twoway();
                break;                
            case 3:
				  ssl_server_oneway();
                break;
            case 4:
				  ssl_server_twoway();
                break;  
            default:
                break;
        }
        if(opt == 99)
        {
            break;
        }
    }
}

static void simcom_http_s_test()
{
    const char *options_list[] = {
        "1. HTTP",
        "2. HTTPS",
        "99. back",
    };
    int opt = 0;
    char scan_string[100] = {0};
	 char url[100] = {0};
	 char rename[20] = {0};
	 char capath[50] = {0};
	 char cainfo[50] = {0};
	 int ca_flag = 0;

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
		         printf("\nPlease Input the url:\n>");
		         memset(url, 0, sizeof(url));
		         if (fgets(url, sizeof(url), stdin) == NULL)
		             continue;
				  url[strlen(url)-1] = 0;
					
		         printf("\nPlease Input the rename:\n>");
		         memset(rename, 0, sizeof(rename));
		         if (fgets(rename, sizeof(rename), stdin) == NULL)
		             continue;
				  rename[strlen(rename)-1] = 0;
				  http_get(url, rename);
                break;
            case 2:
		         printf("\nPlease Input the url:\n>");
		         memset(url, 0, sizeof(url));
		         if (fgets(url, sizeof(url), stdin) == NULL)
		             continue;
				  url[strlen(url)-1] = 0;
					
		         printf("\nPlease Input the rename:\n>");
		         memset(rename, 0, sizeof(rename));
		         if (fgets(rename, sizeof(rename), stdin) == NULL)
		             continue;
				  rename[strlen(rename)-1] = 0;

		         printf("\nPlease Input the ca_flag(1,enable, 0,disable):\n>");
		         memset(scan_string, 0, sizeof(scan_string));
		         if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
		             continue;
				   ca_flag = atoi(scan_string);

				  if (ca_flag)
				  {
			         printf("\nPlease Input the ca path:\n>");
			         memset(capath, 0, sizeof(capath));
			         if (fgets(capath, sizeof(capath), stdin) == NULL)
			             continue;
					  capath[strlen(capath)-1] = 0;
						
			         printf("\nPlease Input the ca name:\n>");
			         memset(cainfo, 0, sizeof(cainfo));
			         if (fgets(cainfo, sizeof(cainfo), stdin) == NULL)
			             continue;
					  cainfo[strlen(cainfo)-1] = 0;
				  }
				  https_get(url, rename, ca_flag, capath, cainfo);
                break;                
            default:
                break;
        }
        if(opt == 99)
        {
            break;
        }
    }
}

static void simcom_ftp_s_test()
{
    const char *options_list[] = {
        "1. FTP",
        "2. FTPS",
        "99. back",
    };
    int opt = 0;
    char scan_string[100] = {0};
	 char url[100] = {0};
	 char rename[20] = {0};
	 char usr[50] = {0};
	 char pwd[20] = {0};

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
		         printf("\nPlease Input the url:\n>");
		         memset(url, 0, sizeof(url));
		         if (fgets(url, sizeof(url), stdin) == NULL)
		             continue;
				  url[strlen(url)-1] = 0;
					
		         printf("\nPlease Input the rename:\n>");
		         memset(rename, 0, sizeof(rename));
		         if (fgets(rename, sizeof(rename), stdin) == NULL)
		             continue;
				  rename[strlen(rename)-1] = 0;

		         printf("\nPlease Input the usr:\n>");
		         memset(usr, 0, sizeof(usr));
		         if (fgets(usr, sizeof(usr), stdin) == NULL)
		             continue;
				  usr[strlen(usr)-1] = 0;
					
		         printf("\nPlease Input the pwd:\n>");
		         memset(pwd, 0, sizeof(pwd));
		         if (fgets(pwd, sizeof(pwd), stdin) == NULL)
		             continue;
				  pwd[strlen(pwd)-1] = 0;
				  ftp_get(url, rename, usr, pwd);
                break;
            case 2:
		         printf("\nPlease Input the url:\n>");
		         memset(url, 0, sizeof(url));
		         if (fgets(url, sizeof(url), stdin) == NULL)
		             continue;
				  url[strlen(url)-1] = 0;
					
		         printf("\nPlease Input the rename:\n>");
		         memset(rename, 0, sizeof(rename));
		         if (fgets(rename, sizeof(rename), stdin) == NULL)
		             continue;
				  rename[strlen(rename)-1] = 0;

		         printf("\nPlease Input the usr:\n>");
		         memset(usr, 0, sizeof(usr));
		         if (fgets(usr, sizeof(usr), stdin) == NULL)
		             continue;
				  usr[strlen(usr)-1] = 0;
					
		         printf("\nPlease Input the pwd:\n>");
		         memset(pwd, 0, sizeof(pwd));
		         if (fgets(pwd, sizeof(pwd), stdin) == NULL)
		             continue;
				  pwd[strlen(pwd)-1] = 0;
				  ftps_get(url, rename, usr, pwd);
                break;                
            default:
                break;
        }
        if(opt == 99)
        {
            break;
        }
    }
}

static void simcom_mqtt_s_test()
{
    const char *options_list[] = {
        "1. MQTT_SUB",
        "2. MQTT_PUB",
        "3. MQTTS_SUB",
        "4. MQTTS_PUB",
        "99. back",
    };
    int opt = 0;
    char scan_string[100] = {0};

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        /* Convert the option to an integer, and switch on the option entered. */
        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
				  mqtt_sub();
                break;
            case 2:
				  mqtt_pub();
                break;        
            case 3:
				  mqtts_sub();
                break;                
            case 4:
				  mqtts_pub();
                break;                
            default:
                break;
        }
        if(opt == 99)
        {
            break;
        }
    }
}
#endif

#ifndef UPCASE
#define  UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )
#endif

static void simcom_audio_test()
{
    const char *options_list[] = {
        "1. play_start",
        "2. play_stop",
		"3. get_play_state",
		"4. start record",
		"5. start record",
		"6. get record status",
        "99. back"
    };
    int ret;
    int opt = 0;
    char scan_string[128] = {0};
    char filename[128]={0};
    int  repeat_value = 0;
    int  path_value = 0;
    int  record_type = 0;
    bool enable_remote=true;

    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");

        memset(scan_string, 0, sizeof(scan_string));
        if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
            continue;

        opt = atoi(scan_string);
        switch(opt)
        {
            case 1:
                #if 1
                enable_remote=true;
                printf("\nPlease input filename(for example: e:/test2s.wav)>");
                memset(scan_string, 0, sizeof(scan_string));
                memset(filename, 0, sizeof(filename));
                if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    continue; 
                if(strlen(scan_string) < 9 || strlen(scan_string) > 101 )
                {
                     printf("\nYou are wrong,number of character is (8-100),not include \\n \n");
                    continue;
                }
                memcpy(filename, scan_string, strlen(scan_string) - 1);
                {
                    char file_format[4] = {0};
                    int  i;
                    if(strlen(filename) <= 4)
                    {
                        printf("The filename or filetype failed!");
                        continue;
                    }

                    for(i = 0; i < 4; i++)
                    {
                        file_format[i] = UPCASE( filename[strlen(filename) - 4 + i]);
                    }

                    if((!memcmp(file_format, ".MP3", 4)) || (!memcmp(file_format, ".AAC", 4)))
                    {
                        enable_remote=false;
                    }
                }

                printf("\nPlease input repeat number>");
                memset(scan_string, 0, sizeof(scan_string));
                if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    continue; 
                
                repeat_value = simcom_handle_input_number(scan_string, strlen(scan_string));

                if(repeat_value < 0 || repeat_value > 255){
                    printf("Input repeat number error!\n");
                    break;
                }

                if(enable_remote==false)
                    printf("\nPlease input path value(0: local)>");
                else
                    printf("\nPlease input path value(0: local  1: remote)>");
                memset(scan_string, 0, sizeof(scan_string));
                if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    continue; 

                path_value = simcom_handle_input_number(scan_string, strlen(scan_string)); 
                if(((path_value != 0) && (path_value != 1))
                        ||((enable_remote==false)&&(path_value!=0)))
                {
                    printf("Input path value error!\n");
                    break;
                }
                #else
                memcpy(filename, "e:/test2.wav", strlen("e:/test2.wav"));
                repeat_value = 2;
                path_value = 0;
                #endif
                ret = audio_play_start(filename, repeat_value, path_value);
                if(ret < 0)
                {
                    printf("play audio error!\n");

                }
                else
                {
                    printf("play started!\n");
                }
                break;
            case 2:
                printf("stop >>>>>\n");
                ret = audio_play_stop();
                break;
			case 3:
				ret = audio_get_state();
				if(ret == 1)
					printf("audio is playing!\n");
				else if(ret == 0)
					printf("audio is stop!\n");
				else
					printf("something is error\n");
				break;
			case 4: //start audio record
                printf("\nPlease input filename(for example: /data/media/record.wav)>");
                memset(scan_string, 0, sizeof(scan_string));
                memset(filename, 0, sizeof(filename));
                if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    continue;                 
                memcpy(filename, scan_string, strlen(scan_string) - 1);

                printf("\nPlease input record type(1: local 2: remote 3: local and remote)>");
                memset(scan_string, 0, sizeof(scan_string));
                if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
                    continue; 
                record_type = simcom_handle_input_number(scan_string, strlen(scan_string)); 
                
				ret = simcom_audio_start_record(record_type, filename);
				if(ret == EXIT_SUCCESS)
					printf("start audio recording\n");
				else
					printf("start audio record error\n");
				break;
			case 5: //stop audio record
				ret = simcom_audio_stop_record();
				if(ret == EXIT_SUCCESS)
					printf("audio record stop success\n");
				else
					printf("audio record stop failure\n");
				break;
			case 6: //get audio record status
				ret = simcom_audio_get_record_state();
				if(ret == AUDIO_RECORD_STATE_BUSY)
					printf("audio record is recording!\n");
				else if(ret == AUDIO_RECORD_STATE_IDLE)
					printf("audio record was stop!\n");
				else
					printf("audio record is error\n");
				break;
					
            case 99:
                ret = audio_play_stop();
                break;
            default:
                continue;
        }
        if(opt == 99)
        {
            break;
        }

    }


}
#ifdef FEATURE_JSON_SDK
static void simcom_json_test()
{
    json_test1();
}
#endif
#endif
