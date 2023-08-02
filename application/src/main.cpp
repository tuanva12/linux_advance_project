#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <netinet/ether.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <pthread.h>

#include "romconfig.h"
#include "app_mqtt.h"

ROMCONFIG romConfig;
APPMQTT mymqtt;


///////////////////////////////////////////////////////////////////////////////
void *exe_thread_connection(void *arg)
{
    mymqtt.app_mqtt_connect();

    // Just block till user tells us to quit.

    while (std::tolower(std::cin.get()) != 'q');

    // Disconnect

    mymqtt.app_mqtt_disconnect();

    pthread_exit(NULL);
}

///////////////////////////////////////////////////////////////////////////////

int main()
{

    pthread_t myThreadMqtt;

    pthread_create(&myThreadMqtt, NULL, exe_thread_connection, (void *)"thread 1");

    pthread_join(myThreadMqtt, NULL);

    printf("Vo Anh Tuan\n");
    return 0;
}