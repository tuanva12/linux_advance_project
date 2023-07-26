#include <sys/ioctl.h>
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
    // mymqtt.app_mqtt_connect();
    std::string data = "{\"type\":\"version\"}";

    mymqtt.app_mqtt_parsedata(data);

    pthread_exit(NULL);
}

///////////////////////////////////////////////////////////////////////////////

int main() {

    pthread_t myThreadMqtt;

    pthread_create(&myThreadMqtt, NULL, exe_thread_connection, (void*)"thread 1");

    pthread_join(myThreadMqtt, NULL);


    printf("Vo Anh Tuan\n");
    return 0;
}