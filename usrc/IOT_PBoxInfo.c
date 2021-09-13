#include "usr_cfg.h"
#include "type.h"
#include "log.h"
#include "json.h"

char templateUniqueId[50] = {"5KYK37p0EJXd6Ej76Rtp"};
char deviceId[20] = {0};
char clientId[50] = {"bc8c2f1d5f6545b8902a2e26da96e55b"};
char secret[20] = {0};
char pbox_own_ip[20] = "192.168.0.2";
char equipmentIP[20] = "192.168.0.102";
char pbox_own_gateWay[20] = {0};

void get_data(struct json_object* json_object_data,const char* key,char* val)
{
    if(json_object_data!=NULL)
    {
        struct json_object* deviceIp = json_object_object_get(json_object_data,key);
        if(deviceIp!=NULL)
        {
            strcpy(val,json_object_get_string(deviceIp));
            zlg_debug("%s:%s\r\n",key,val);
        }
        else
        {
            zlg_debug("%s null\r\n",key);
        }
    }
    else
    {
        zlg_debug("json_object_data null\r\n");
    }
    
}

int GetPBoxInfo(void)
{
    char url[200] = {0};
    char revData[1000] = {0};
    sprintf(url, "http://%s%s?pBoxId=%s&iccId=%s", IOT_DomainNameToIP.Get_DomainNameIP(), PBOXINFO_URL, IOT_SIM7600CE.Get_IMEI(),IOT_SIM7600CE.Get_IMEI());
    zlg_debug("Get PBoxInfo URL:%s\r\n",url);
    if(IOT_http_s.http_get(url,revData)==0)
    {
        zlg_debug("%s\r\n",revData);
        struct json_object* json_object = json_tokener_parse(revData);
        
        struct json_object* code = json_object_object_get(json_object,"code");
        if(code!=NULL)
        {
            if(json_object_get_int(code)==200)
            {
                struct json_object* data = json_object_object_get(json_object,"data");
                if(data!=NULL)
                {
                    const char* data_data = json_object_get_string(data);
                    if(data_data!=NULL)
                    {
                        struct json_object* json_object_data = json_tokener_parse(data_data);
                        get_data(json_object_data,"deviceIp",equipmentIP);
                        get_data(json_object_data,"templateUniqueId",templateUniqueId);
                        get_data(json_object_data,"deviceId",deviceId);
                        get_data(json_object_data,"clientId",clientId);
                        get_data(json_object_data,"secret",secret);
                        get_data(json_object_data,"pbox_own_ip",pbox_own_ip);   
                    }
                    else
                    {
                        zlg_debug("PBoxInfo data null\r\n");
                        return -5;
                    }
                }
                else
                {
                    zlg_debug("PBoxInfo data null\r\n");
                    return -4;
                }               
            }
            else
            {
                zlg_debug("message code:%d  message is invalid\r\n",json_object_get_int(code));
                return -3;
            }
        }
        else
        {
            zlg_debug("PBoxInfo code analyze error\r\n");
            return -2;
        }    
    }
    else
    {
        zlg_debug("http get error!\r\n");
        return -1;
    }
    return 0;  
}

char* PBoxInfo_templateUniqueId(void)
{
    return templateUniqueId;
}
char* PBoxInfo_deviceId(void)
{
    return deviceId;
}
char* PBoxInfo_clientId(void)
{
    return clientId;
}
char* PBoxInfo_pbox_own_ip(void)
{
    int i = 0,len = 0;
    memcpy(pbox_own_ip,equipmentIP,20);
    len = strlen(pbox_own_ip);
    for(i=len-1;i>0;i--)
    {
        if(pbox_own_ip[i]=='.')
        {
            pbox_own_ip[i+1] = '3';
            pbox_own_ip[i+2] = 0;
            break;
        }
    }
    return pbox_own_ip;
}
char* PBoxInfo_equipmentIP(void) 
{
    return equipmentIP;
}
char* PBoxInfo_pbox_own_gateWay(void)
{
    int i = 0,len = 0;
    memcpy(pbox_own_gateWay,pbox_own_ip,20);
    len = strlen(pbox_own_ip);
    for(i=len-1;i>0;i--)
    {
        if(pbox_own_gateWay[i]=='.')
        {
            pbox_own_gateWay[i+1] = '1';
            pbox_own_gateWay[i+2] = 0;
            break;
        }
    }

    return pbox_own_gateWay;
}
_IOT_PBoxInfo IOT_PBoxInfo = 
{
	GetPBoxInfo,
    PBoxInfo_templateUniqueId,
    PBoxInfo_deviceId,
    PBoxInfo_clientId,
    PBoxInfo_pbox_own_ip,
    PBoxInfo_equipmentIP,
    PBoxInfo_pbox_own_gateWay
};
