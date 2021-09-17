#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include "usr_cfg.h"
#include "type.h"
#include "log.h"

static void ping_server(void)
{
	char cmd[128] = {'\0'}; 
	snprintf(cmd,127, "ping %s -c 4","114.114.114.114");
	zlg_debug("%s\r\n",cmd);
	system(cmd); 
}
int get_ip_from_host(char *ipbuf, const char *host, int maxlen)
{
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	if (inet_aton(host, &sa.sin_addr) == 0) 
	{
		struct hostent *he;
		he = gethostbyname(host);
		if (he == NULL)
			return -1;
		memcpy(&sa.sin_addr, he->h_addr, sizeof(struct in_addr));
	}
	strncpy(ipbuf, inet_ntoa(sa.sin_addr), maxlen);
	return 0;
}
char DomainNameIP[30] = {0};
int ConvertHostNameToIP(void)
{
    int res = 0;
    int failCnt = 0;
    do
    {
        res = get_ip_from_host(DomainNameIP,HTTPHOST,sizeof(DomainNameIP));
        if(0==res)
        {
            zlg_debug("DomainNameIP : %s\r\n",DomainNameIP);
        }
        else
        {
            failCnt++;
            zlg_debug("DomainNameIP : ERROR!\r\n");
            if(failCnt>20)
            {
                zlg_debug("DomainNameIP : ERROR more than 10!\r\n");
                exit(-1);
            }
            ping_server();
            sleep(5);
        }
    }while(0!=res);
    return res;
}
char* Get_DomainNameIP(void)
{
    return DomainNameIP;
}
_IOT_DomainNameToIP IOT_DomainNameToIP = 
{
    ConvertHostNameToIP,
    Get_DomainNameIP
};
