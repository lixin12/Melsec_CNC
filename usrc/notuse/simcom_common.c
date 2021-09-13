#include "simcom_common.h"

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
int simcom_handle_input_string(char* scan_string, int nlen_scan_string, char* out_str, int nMaxLen, int nMinLen)
{
    int ret = 0;
    int nLenScanStr = 0;
    int i = 0;
    
    if((scan_string == NULL) || (out_str == NULL)){
        return -1;
    }

    if((scan_string[nlen_scan_string-1]  != 0x0A) && (scan_string[nlen_scan_string  - 1] != 0x0D)){           
         return 2;
    }
    
    for (i = 0; i <= nlen_scan_string - 1; i++){
        if((scan_string[i] == 0x0A) || (scan_string[i] == 0x0D)){
            scan_string[i] = '\0';
            break;
        }
    }
    
    nLenScanStr = strlen(scan_string);
    if(nLenScanStr > nMaxLen){
        return 1;
    }
    
    if(nLenScanStr < nMinLen){
        return -1;
    }

    strncpy(out_str, scan_string, nLenScanStr);
    
    return ret;
}

/*************************************************************
 * simcom_handle_input_number remove the 0x0a\0x0d, and call atoi()
 *  input 
 *          --scan_string                     :input string
 *          --nlen_scan_string            :input string length  
 * ret
 * 0xFFFF error
 * number  success
 *************************************************************/
int simcom_handle_input_number(char* scan_string, int nlen_scan_string)
{
    int ret = 0xFFFF;
    int nLenScanStr = 0;
    int i = 0;
    
    if(scan_string == NULL || (nlen_scan_string < 1)){
        return ret;
    }
    
    if((scan_string[nlen_scan_string-1]  != 0x0A) && (scan_string[nlen_scan_string  - 1] != 0x0D)){   
        return ret;
    }
    
    for (i = 0; i <= nlen_scan_string - 1; i++){
        if((scan_string[i] == 0x0A) || (scan_string[i] == 0x0D)){
            scan_string[i] = '\0';
            break;
        }
        if((scan_string[i] == '+') || (scan_string[i] == '-')){
            continue;  
         }

        if( !isdigit(scan_string[i]) ){            
            printf("!isdigit()\n");
            return ret;
        }            
    }

    return atoi(scan_string);    
}

void simcom_fgets(char *scan_string, uint32_t scan_len)
{
    memset(scan_string, 0, scan_len);
    while (fgets(scan_string, scan_len, stdin) != NULL)
    {
        if (strlen(scan_string) == 1) continue;
        scan_string[strlen(scan_string)-1] = 0;
        break;
    }
}