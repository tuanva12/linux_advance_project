#include <sys/ioctl.h>
#include <linux/if.h>
#include <netinet/ether.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <iostream>

#include "romconfig.h"
#include "app_mqtt.h"

ROMCONFIG romConfig;
APPMQTT mymqtt;


static std::string appGetMacAdd(void)
{
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    struct ifreq ifr{};
    strcpy(ifr.ifr_name, "eth0");
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    close(fd);

    char mac[18];
    strcpy(mac, ether_ntoa((ether_addr *) ifr.ifr_hwaddr.sa_data));

    // std::cout << mac << std::endl;

    return mac;
}


///////////////////////////////////////////////////////////////////////////////

int main() {

    mymqtt.CLIENT_ID = appGetMacAdd();

    mymqtt.app_mqtt_connect();

    printf("Vo Anh Tuan\n");
    return 0;
}