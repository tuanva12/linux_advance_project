#include "romconfig.h"
#include "app_mqtt.h"

ROMCONFIG romConfig;
APPMQTT mymqtt;

int main() {

    mymqtt.app_mqtt_connect();
    printf("Vo Anh Tuan\n");
    return 0;
}