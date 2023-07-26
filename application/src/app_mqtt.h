/**
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
#ifndef _APP_MQTT_H__
#define _APP_MQTT_H__

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cctype>
#include <thread>
#include <chrono>

#include "mqtt/async_client.h"




class APPMQTT
{
protected:


public:
    std::string SERVER_ADDRESS = "tcp://localhost:1883";
    std::string CLIENT_ID = "MAC_ADD";
    std::string TOPIC_EVENT = "/event";
    std::string TOPIC_CMD = "/command";

    APPMQTT();

    void app_mqtt_connect(void);
    void app_mqtt_disconnect(void);
    void app_mqtt_parsedata(std::string data);

private:

    mqtt::async_client* cli;
};
#endif // _APP_MQTT_H__