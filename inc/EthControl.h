#ifndef _ETH_CONTROL_H_
#define _ETH_CONTROL_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <sys/select.h>
#include "common.h"


typedef enum{
    ETH_TYPE_UNKNOWN = 0,
    ETH_TYPE_BCM898XX,
    ETH_TYPE_AT803X
}ethernet_type_info;


#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

    int eth_init(ethernet_type_info type);
    int Install_driver();
    int Uninstall_driver();
    int set_eth_mac_address(char *mac_address);
    int set_eth_ip_address(char *mac_address);


#endif

