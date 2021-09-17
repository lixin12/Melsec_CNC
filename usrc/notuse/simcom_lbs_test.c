#define SIMCOM_LOG_TAG    "SIMCOM_LBS_TEST"

#include "simcom_common.h"
#include "simcom_lbs.h"

/*****************************************************************************
 * FUNCTION
 *  simcom_lbs_disInfo
 *
 * DESCRIPTION
 *  simcom_lbs_disInfo
 *  
 * PARAMETERS
 *  pLbsRevInfo: pointer of simcom_lbs_receive_info_t
 *  mode: @SIMCOM_LBS_OP
 *
 * RETURNS
 *  @SIMCOM_LBS_CODE_TYPE  
 *
 * NOTE
 *  
 *****************************************************************************/
int simcom_lbs_disInfo(simcom_lbs_receive_info_t* pLbsRevInfo, unsigned char mode) 
{
    unsigned int i;    
    extern char  g_ServerAddr[SIMCOM_LBS_DETAILADDR_LEN];
    extern unsigned int g_ServerPort;

    if((pLbsRevInfo->u8ErrorCode != SIMCOM_LBS_CODE_DEFUALT_VALUE)
        &&(pLbsRevInfo->u8ErrorCode != SIMCOM_LBS_CODE_SUCC_ERROR))
    {
        printf("LBS ERROR:%d\n", pLbsRevInfo->u8ErrorCode);   
        return -1;
    }

    switch(mode)
    {
        case SIMCOM_LBS_OP_LON_LAT:
        {
            printf("LNG:%f\n", (float)(pLbsRevInfo->u32Lng)/1000000);            
            printf("LAT:%f\n", (float)(pLbsRevInfo->u32Lat)/1000000);
            printf("ACC:%u\n", pLbsRevInfo->u16Acc);
        }
        break;
        
        case SIMCOM_LBS_OP_DETAIL_ADDR:
        {        
            printf("ADDR:");
            for(i=0; i<pLbsRevInfo->u32AddrLen; i++)
            {
                printf("%02x", pLbsRevInfo->u8LocAddress[i]);
            }
            printf("\n");
        }
        break;
        
        case SIMCOM_LBS_OP_ACCESS_TIMES:
        {
            printf("UTC:%s\n", pLbsRevInfo->u8DateAndTime);
        }
        break;

        case SIMCOM_LBS_OP_LON_LAT_DATA_TIME:
        {
            printf("LNG:%f\n", (float)(pLbsRevInfo->u32Lng)/1000000);            
            printf("LAT:%f\n", (float)(pLbsRevInfo->u32Lat)/1000000);
            printf("ACC:%u\n", pLbsRevInfo->u16Acc);        
            printf("UTC:%s\n", pLbsRevInfo->u8DateAndTime);
        }
        break;

        case SIMCOM_LBS_OP_GET_SERVER_CONFIG:
        {
            printf("Get config info OK:\n");
            printf("%s:%d\n", g_ServerAddr, g_ServerPort);           
        }
        break;

        case SIMCOM_LBS_OP_SET_SERVER_CONFIG:
        {
            printf("Set config info OK:\n");        
            printf("%s:%d\n", g_ServerAddr, g_ServerPort);           
        }
        break;

        default:
        {
            //printf("Invalid input...\n");           
        }
        break;        
    }

    return EXIT_SUCCESS;
}


void simcom_lbs_test()
{

    const char *options_list[] = {
        "1. Get longitude and latitude",
        "2. Get detail address",
        "3. get access times",
        "4. get lon,lat and time",
        "5. get Server's address",
        "6. set Server's address",
        "99. Back",
    };
    int option = 0;
    char scan_string[100] = {0};
    simcom_lbs_receive_info_t LbsReceiveinfo;  
    uint8_t serverAddr[150];
    uint32_t serverPort=0;    
    int ret = 0;

    if(simcom_lbs_init() != EXIT_SUCCESS)
    {
        simcom_log_e("simcom sms init failed");
        return;
    }

    /* SMS main menu*/
    while(1)
    {
        printf("\nPlease select an option to test from the items listed below.\n");
        print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
        printf("Option > ");
         
        simcom_fgets(scan_string, sizeof(scan_string));
        
        option = atoi(scan_string);
        switch(option)
        {
            case 1:             /*get longitude and latitude*/
            case 2:         /*get longitude and latitude and detailaddress*/
            case 3:
            case 4:
            {
                ret = simcom_lbs_get_location_info(option, &LbsReceiveinfo);
            }
            break;
            
            case 5:
            {
                ret = simcom_lbs_get_config_info((char *)serverAddr, &serverPort);                    
                LbsReceiveinfo.u8ErrorCode = ret;
            }
            break;
        
            case 6:
            {
                printf("Enter Server Address:");
                memset(scan_string, 0, sizeof(scan_string));
                simcom_fgets(scan_string, sizeof(scan_string));
                scan_string[strlen(scan_string)] = '\n';
                snprintf((char *)serverAddr, 100-1, scan_string);               

                printf("Enter Server Port:");
                memset(scan_string, 0, sizeof(scan_string));
                simcom_fgets(scan_string, sizeof(scan_string));                
                scan_string[strlen(scan_string)] = '\n';
                serverPort = atoi(scan_string);

                ret = simcom_lbs_set_config_info((char *)serverAddr, serverPort);  
                LbsReceiveinfo.u8ErrorCode = ret;
            }
            break; 

            default:
            break;            
        }

        if(option == 99)
        {
            break;
        }

        printf("******************************\n\n");
        simcom_lbs_disInfo(&LbsReceiveinfo, option);
        printf("\n******************************\n");
    }
    simcom_lbs_deinit();
}


