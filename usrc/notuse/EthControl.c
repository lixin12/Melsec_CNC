#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include "common.h"
#include "EthControl.h"
#include "ds_util.h"


static int is_driver_installed = FALSE;
static ethernet_type_info  eth_type;


int eth_init(ethernet_type_info type)
{
    eth_type = type;
    return 0;
}

int Install_driver()
{
    char command[256];

    if(eth_type != ETH_TYPE_BCM898XX && eth_type != ETH_TYPE_AT803X)
    {
        return -1;
    }

    memset(command, 0, sizeof(command));
    sprintf(command, "%s", "insmod /lib/modules/3.18.20/kernel/drivers/net/phy/libphy.ko"); 
    ds_system_call(command, strlen(command));

    memset(command, 0, sizeof(command));
    sprintf(command, "%s", "insmod /lib/modules/3.18.20/kernel/drivers/of/of_mdio.ko"); 
    ds_system_call(command, strlen(command));

    if(eth_type == ETH_TYPE_BCM898XX)
    {
        memset(command, 0, sizeof(command));
        sprintf(command, "%s", "insmod /lib/modules/3.18.20/kernel/drivers/net/phy/bcm898xx.ko"); 
        ds_system_call(command, strlen(command));
    }
    else if(eth_type == ETH_TYPE_AT803X)
    {
        memset(command, 0, sizeof(command));
        sprintf(command, "%s", "insmod /lib/modules/3.18.20/kernel/drivers/net/phy/at803x.ko"); 
        ds_system_call(command, strlen(command));
    }

    memset(command, 0, sizeof(command));
    sprintf(command, "%s", "insmod /lib/modules/3.18.20/kernel/drivers/net/phy/qca8337.ko"); 
    ds_system_call(command, strlen(command));

    memset(command, 0, sizeof(command));
    sprintf(command, "%s", "insmod /lib/modules/3.18.20/kernel/drivers/net/ethernet/qualcomm/emac/qcom_emac.ko"); 
    ds_system_call(command, strlen(command)); 

    is_driver_installed = TRUE;
}

int Uninstall_driver()
{
    char command[256];
    if(eth_type != ETH_TYPE_BCM898XX && eth_type != ETH_TYPE_AT803X)
    {
        return -1;
    }
    memset(command, 0, sizeof(command));
    sprintf(command, "%s", "rmmod qcom_emac.ko"); 
    ds_system_call(command, strlen(command));

    memset(command, 0, sizeof(command));
    sprintf(command, "%s", "rmmod qca8337.ko"); 
    ds_system_call(command, strlen(command));

    if(eth_type == ETH_TYPE_BCM898XX)
    {
        memset(command, 0, sizeof(command));
        sprintf(command, "%s", "rmmod bcm898xx.ko"); 
        ds_system_call(command, strlen(command));
    }
    else
    {
        memset(command, 0, sizeof(command));
        sprintf(command, "%s", "rmmod at803x.ko"); 
        ds_system_call(command, strlen(command));    
    }
    memset(command, 0, sizeof(command));
    sprintf(command, "%s", "rmmod of_mdio.ko"); 
    ds_system_call(command, strlen(command));

    memset(command, 0, sizeof(command));
    sprintf(command, "%s", "rmmod libphy.ko"); 
    ds_system_call(command, strlen(command));

    is_driver_installed = FALSE;
}

int set_eth_mac_address(char *mac_address)
{
    char command[256];
    memset(command, 0, sizeof(command));
    sprintf(command, "%s", "ifconfig eth0 down"); 
    ds_system_call(command, strlen(command));

    memset(command, 0, sizeof(command));
    sprintf(command, "ifconfig eth0 hw ether %s", mac_address); 
    ds_system_call(command, strlen(command));

    memset(command, 0, sizeof(command));
    sprintf(command, "%s", "ifconfig eth0 up"); 
    ds_system_call(command, strlen(command));
}

int set_eth_ip_address(char *ip_address)
{
    char command[256];
    memset(command, 0, sizeof(command));
    sprintf(command, "ifconfig eth0 %s netmask 255.255.255.0", ip_address); 
    ds_system_call(command, strlen(command));
}


