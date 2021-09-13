#include "GPSControl.h"
#include "simcom_common.h"
#include "simcom_lbs.h"
#include "type.h"
#include "log.h"
#include "usr_cfg.h"
int IOT_GNSSInit(void)
{
    if(gps_init((gps_ind_cb_fcn)process_simcom_ind_message) != TRUE)
    {
        zlg_debug("gps_init failed\r\n");
        return -1;
    }
    gps_xtra_enable();
    if(gps_coldstart() == FALSE) 
    {
        zlg_debug("Location ColdStart Failed!\r\n");
        return -1;
    }
    /*if(gps_hotstart() == FALSE) 
    {
        zlg_debug("Location HoldStart Failed\r\n");
    }*/
    return 0;
}
GpsInfo GpsInfoData = {0};
void process_simcom_ind_message(simcom_event_e event,void *cb_usr_data)
{
    GpsInfo* GpsInfoDataTmp = (GpsInfo*)cb_usr_data;
    switch(event)
    {
        case SIMCOM_EVENT_LOC_IND:
            memcpy(&GpsInfoData,GpsInfoDataTmp,sizeof(GpsInfo));
            zlg_debug("latitude : %.3lf",GpsInfoDataTmp->latitude);
            zlg_debug("longitude : %.3lf",GpsInfoDataTmp->longitude);
        break;
        case SIMCOM_EVENT_NMEA_IND:
            zlg_debug("GPS_NMEA:%s",(char*)cb_usr_data);
        break;
        default:
        break;
    }
    return;
}

GpsInfo* IOT_GetGNSSInfo(void)
{
    return &GpsInfoData;
}


simcom_lbs_receive_info_t LbsReceiveinfo; 
void IOT_AGPSInit(void)
{
    if(simcom_lbs_init() != EXIT_SUCCESS)
    {
        zlg_debug("simcom sms init failed");
        return;
    }
    simcom_lbs_get_location_info(4, &LbsReceiveinfo);

    printf("LNG:%f\n", (float)(LbsReceiveinfo.u32Lng)/1000000);            
    printf("LAT:%f\n", (float)(LbsReceiveinfo.u32Lat)/1000000);
    printf("ACC:%u\n", LbsReceiveinfo.u16Acc);        
    printf("UTC:%s\n", LbsReceiveinfo.u8DateAndTime);
}

simcom_lbs_receive_info_t* IOT_GetAGPSInfo(void)
{
    simcom_lbs_get_location_info(4, &LbsReceiveinfo);
    zlg_debug("LNG:%f\n", (float)(LbsReceiveinfo.u32Lng)/1000000);            
    zlg_debug("LAT:%f\n", (float)(LbsReceiveinfo.u32Lat)/1000000);
    zlg_debug("ACC:%u\n", LbsReceiveinfo.u16Acc);        
    zlg_debug("UTC:%s\n", LbsReceiveinfo.u8DateAndTime);
    return &LbsReceiveinfo;
}

simcom_lbs_receive_info_t* IOT_GetAGPSInfoVal(void)
{
    return &LbsReceiveinfo;
}

_IOT_GPS IOT_GPS = 
{
    IOT_GNSSInit,
    IOT_GetGNSSInfo,
    IOT_AGPSInit,
    IOT_GetAGPSInfo,
    IOT_GetAGPSInfoVal
};

