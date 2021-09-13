#include "type.h"
#include "log.h"
#include "simcom_common.h"
#include "qmi_client.h"
#include "network_access_service_v01.h"
typedef enum{
  RADIO_IF_NO_SVC = 0x00, 
  RADIO_IF_CDMA_1X = 0x01,
  RADIO_IF_CDMA_1XEVDO = 0x02,
  RADIO_IF_AMPS = 0x03, 
  RADIO_IF_GSM = 0x04, 
  RADIO_IF_UMTS = 0x05,
  RADIO_IF_LTE = 0x08,
  RADIO_IF_TDSCDMA = 0x09,
  RADIO_IF_1XLTE = 0x0A
}radio_if_type_enum;

static timer_t timerid;
static long counter = 0;
static char LED_CollVal = 0;
static char LED_MqttVal = 0;
static char LED_LTEVal = 0;
extern u8 bsp_GetOnLine(void);
static int csq_val = 0;
static int csq_0_flag = 0;

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
        csqval = (int)((125.0 + rssi) * 31.0 / 50);
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
        csqval = (int)((182.26 + 100.0/62 * rssi) * 31.0 / 100 + 0.5);
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
       csqval = (int)((113.0 + 100.0/191 * rssi) * 31 / 100 + 0.5);
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

static void get_SignalStrengthWithCSQ(int* csq)
{
    nas_get_signal_strength_resp_msg_v01 signal_strength;

    int i = 0;  
    int rssi_len;
    int rssi;
    int rf;
    int csqval;
    int ret = nas_get_signal_strength(&signal_strength);
    if (ret != 0)
    {
        zlg_debug("get sig err!\n");
        return ;
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
    *csq = get_csq_val(rf, rssi);

    zlg_debug("radio_if = %d, rssi= %d, csq= %d\n", rf, (-1) * rssi, csqval);
}
extern int sendATCmd(char* atCmd, char* finalRsp, char *buff, uint32_t buffSize, long timeout_ms);
int my_send_at(char * cmd, char * buff, uint32_t buffSize)
{
    int i;
    int retval = -1;    
    printf(cmd );
    printf("\n");
    for (i = 0; i < 5; i++) {
        memset(buff, 0, buffSize);        
        if ((retval = sendATCmd(cmd, "OK", buff, buffSize, 2000)) <= 0) {
            zlg_debug("try %d, result = %d\n", i, retval);
        }
        else {
            break;
        }
    }
    zlg_debug(buff);
    return retval;
}

static void handler(int sig, siginfo_t *si, void *uc)
{
   /* Note: calling printf() from a signal handler is not
      strictly correct, since printf() is not async-signal-safe;
      see signal(7) */
   counter++;
   int res = 0;
   char buffer[200] = {0};
   if(0==(counter%600))
   {
       get_SignalStrengthWithCSQ(&csq_val);
       if(csq_val==0)
       {
           csq_0_flag++;
           if(csq_0_flag>=5)
           {
               //reset system stop feed watchdog or restart 4G Network
               //
               res = my_send_at("AT+CFUN=0\r\n",buffer,sizeof(buffer));
               if(res>0)
               {
                   res = my_send_at("AT+CFUN=1\r\n",buffer,sizeof(buffer));
               }
               if(res>0)
               {
                   zlg_debug("restart 4G Network ok!\r\n");
               }
               else
               {
                   zlg_debug("restart 4G Network failed!\r\n");
                   if(csq_0_flag>=10)
                   {
                       //feed watchdog and restart 4G Module
                       my_send_at("AT+CRESET\r\n",buffer,sizeof(buffer));
                   }
               }
           }
       }
       else
       {
           csq_0_flag = 0;
       }
   }
   if(csq_val>0)
   {
       int tmp = 0;
       tmp = (1-csq_val/31)*10;
       if(1>=tmp)
       {
           tmp = 1;
       }
       if(counter%tmp==0)
       {
           LED_LTEVal++;
           IOT_LED.gpio_set_value(IOT_LED.LED_LTE_GPIO,LED_LTEVal%2);
       }
   }
   if(1==bsp_GetOnLine())
   {
       if(counter%3==0)
       {
           LED_CollVal++;
           IOT_LED.gpio_set_value(IOT_LED.LED_LED1_GPIO,LED_CollVal%2);
       }
   }
   else
   {
       IOT_LED.gpio_set_value(IOT_LED.LED_LED1_GPIO,0);
   }
   if(1==mqtt_PubIsOnline())
   {
       if(counter%3==0)
       {
           LED_MqttVal++;
           IOT_LED.gpio_set_value(IOT_LED.LED_LED2_GPIO,LED_MqttVal%2);
       }
   }
   else
   {
       IOT_LED.gpio_set_value(IOT_LED.LED_LED2_GPIO,0);
   }
}


void IOT_LEDControl(void)
{
    nas_init();
    IOT_LED.IOT_GPIO_Init();

    IOT_TIMER.handler = handler;               //rewrite handler function
    IOT_TIMER.create_timer(100000000,&timerid);//100ms timer
    get_SignalStrengthWithCSQ(&csq_val);
}

_IOT_LEDCONTROL IOT_LEDCONTROL = 
{
    IOT_LEDControl
};
