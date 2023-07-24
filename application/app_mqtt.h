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

class APPMQTT
{
public:
    std::string SERVER_ADDRESS = "tcp://localhost:1883";
    std::string CLIENT_ID = "paho_cpp_async_subcribe";

    void app_mqtt_connect(void);

private:
    int QOS = 1;
    int N_RETRY_ATTEMPTS = 5;
    std::string TOPIC_EVENT = "hello";
    std::string TOPIC_CMD = "hello";
};
#endif // _APP_MQTT_H__