#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <net/route.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include "stdbool.h"
#if defined(__GLIBC__) && __GLIBC__ >=2 && __GLIBC_MINOR__ >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h>
#else
#include <sys/types.h>
#include <netinet/if_ether.h>
#endif
#include "type.h"
#include "log.h"

//#define TRUE true;
//#define FALSE false;

#define DEFAULT_ETH "eth0"
#define IP_LENGTH   18

static int set_addr(unsigned char ip[16], int flag);
static int get_addr(unsigned char ip[16], int flag);

int get_ip(unsigned char ip[16])
{
    return get_addr(ip, SIOCGIFADDR);
}

int get_ip_netmask(unsigned char ip[16])
{
    return get_addr(ip, SIOCGIFNETMASK);
}

int get_mac(unsigned char addr[6])
{
    return get_addr(addr, SIOCGIFHWADDR);
}

static int get_addr(unsigned char *addr, int flag)
{
    int sockfd = 0;
    struct sockaddr_in *sin;
    struct ifreq ifr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        zlg_debug("socket error!\n");
        return FALSE;
    }

    memset(&ifr, 0, sizeof(ifr));
    snprintf(ifr.ifr_name, (sizeof(ifr.ifr_name) - 1), "%s", DEFAULT_ETH);

    if(ioctl(sockfd, flag, &ifr) < 0 )
    {
        zlg_debug("ioctl error!\n");
        close(sockfd);
        return FALSE;
    }
    close(sockfd);

    if (SIOCGIFHWADDR == flag){
        memcpy((void *)addr, (const void *)&ifr.ifr_ifru.ifru_hwaddr.sa_data, 6);
        /*printf("mac address: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);*/
    }else{
        sin = (struct sockaddr_in *)&ifr.ifr_addr;
        snprintf((char *)addr, IP_LENGTH, "%s", inet_ntoa(sin->sin_addr));
    }

    return TRUE;
}

int is_valid_ip(unsigned char ipaddr[16])
{
    int ret = 0;
    struct in_addr inp;
    ret = inet_aton(ipaddr, &inp);
    if (0 == ret)
    {
        return FALSE;
    }
    else
    {
        zlg_debug("inet_aton:ip=%lu\n",ntohl(inp.s_addr));
    }

    return TRUE;
}

/*
 * 先验证是否为合法IP，然后将掩码转化成32无符号整型，取反为000...00111...1，
 * 然后再加1为00...01000...0，此时为2^n，如果满足就为合法掩码
 *
 * */
int is_valid_netmask(unsigned char netmask[16])
{
    if(is_valid_ip(netmask) > 0)
    {
        unsigned int b = 0, i, n[4];
        sscanf(netmask, "%u.%u.%u.%u", &n[3], &n[2], &n[1], &n[0]);
        for(i = 0; i < 4; ++i) //将子网掩码存入32位无符号整型
            b += n[i] << (i * 8);
        b = ~b + 1;
        if((b & (b - 1)) == 0) //判断是否为2^n
            return TRUE;
    }

    return FALSE;
}


int set_ip_netmask(unsigned char ip[16])
{
    return set_addr(ip, SIOCSIFNETMASK);
}

int set_ip(unsigned char ip[16])
{
    return set_addr(ip, SIOCSIFADDR);
}

static int set_addr(unsigned char ip[16], int flag)
{
    struct ifreq ifr;
    struct sockaddr_in sin;
    int sockfd;

    if (is_valid_ip(ip) < 0)
    {
        zlg_debug("ip was invalid!\n");
        return FALSE;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1){
        zlg_debug("Could not get socket.\n");
        //perror("eth0\n");
        return FALSE;
    }

    snprintf(ifr.ifr_name, (sizeof(ifr.ifr_name) - 1), "%s", DEFAULT_ETH);

    /* Read interface flags */
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
        zlg_debug("ifdown: shutdown \r\n");
        //perror(ifr.ifr_name);
        return FALSE;
    }

    memset(&sin, 0, sizeof(struct sockaddr));
    sin.sin_family = AF_INET;
    inet_aton(ip, &sin.sin_addr);
    memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
    if (ioctl(sockfd, flag, &ifr) < 0){
        zlg_debug("Cannot set IP address. \r\n");
        perror(ifr.ifr_name);
        return FALSE;
    }

    return TRUE;
}


int set_gateway(unsigned char ip[16])
{
    int sockFd;
    struct sockaddr_in sockaddr;
    struct rtentry rt;

    if (is_valid_ip(ip) < 0)
    {
        zlg_debug("gateway was invalid!\n");
        return FALSE;
    }

    sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockFd < 0)
    {
        zlg_debug("Socket create error.\n");
        return FALSE;
    }

    memset(&rt, 0, sizeof(struct rtentry));
    memset(&sockaddr, 0, sizeof(struct sockaddr_in));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = 0;
    if(inet_aton(ip, &sockaddr.sin_addr)<0)
    {
        zlg_debug("inet_aton error\n" );
        close(sockFd);
        return FALSE;
    }

    memcpy ( &rt.rt_gateway, &sockaddr, sizeof(struct sockaddr_in));
    ((struct sockaddr_in *)&rt.rt_dst)->sin_family=AF_INET;
    ((struct sockaddr_in *)&rt.rt_genmask)->sin_family=AF_INET;
    rt.rt_flags = RTF_GATEWAY;
    if (ioctl(sockFd, SIOCADDRT, &rt)<0)
    {
        zlg_debug("ioctl(SIOCADDRT) error in set_default_route\n");
        close(sockFd);
        return FALSE;
    }

    return TRUE;
}

int set_MAC(unsigned char *pMac) 
{ 
    int m_socket = 0; 
    struct ifreq mac; 
    int i = 0;
    #if 0 
    m_socket = socket(AF_INET, SOCK_DGRAM, 0); 
    if(m_socket > 0) 
    {    
        strcpy(mac.ifr_name, DEFAULT_ETH);  // "eth0"     
        mac.ifr_hwaddr.sa_family = ARPHRD_ETHER;   
        for(i = 0; i < 6; i++)   
        {    
            mac.ifr_hwaddr.sa_data[i] = ((pMac[i*2]-'0')<<4) | (pMac[i*2+1]-'0');  
        }
        if(ioctl(m_socket, SIOCSIFHWADDR, &mac) == 0)    
        {     
            zlg_debug("config mac address successful!\n");    
            close(m_socket);    
            return 0;    
        }    
        zlg_debug("config mac address failed!\n");    
        close(m_socket); } 
        return -1;
        #else 
        char cmd[128] = {'\0'}; 
        snprintf(cmd,127, "ifconfig %s down",DEFAULT_ETH);
        printf("%s\r\n",cmd);
        system(cmd); 
        sleep(5);
        snprintf(cmd, 127, "ifconfig eth0 hw ether %02x:%02x:%02x:%02x:%02x:%02x",    
        ((((pMac[0]-'0')<<4) | (pMac[1]-'0'))&0xFE)|0x02, ((pMac[2]-'0')<<4) | (pMac[3]-'0'), ((pMac[4]-'0')<<4)  | (pMac[5]-'0'), 
        ((pMac[6]-'0')<<4) | (pMac[7]-'0'), ((pMac[8]-'0')<<4) | (pMac[9]-'0'), ((pMac[10]-'0')<<4) | (pMac[11]-'0')); 
        printf("%s\r\n",cmd);
        system(cmd); 
        sleep(5);
        snprintf(cmd,127, "ifconfig %s up",DEFAULT_ETH);
        printf("%s\r\n",cmd);
        system(cmd); 
        return 0;
        #endif 
    }

_IOT_IPSet IOT_IPSet = 
{
    set_ip,
    set_ip_netmask,
    set_gateway,
    set_MAC
};
