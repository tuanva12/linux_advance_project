#include <sys/ioctl.h>
#include <linux/if.h>
#include <netinet/ether.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <iostream>

#include <nlohmann/json.hpp>
#include "mqtt/async_client.h"
#include "app_mqtt.h"
#include "define.h"

/////////////////////////////////////////////////////////////////////////////
extern APPMQTT mymqtt;

using myjson = nlohmann::json;

///////////////////////////////////////////////////////////////////////////////
// Read mac address and convert to string
static std::string appGetMacAdd(void)
{
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    struct ifreq ifr
    {
    };
    strcpy(ifr.ifr_name, "ens33");
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    close(fd);

    char mac[18];
    strcpy(mac, ether_ntoa((ether_addr *)ifr.ifr_hwaddr.sa_data));

    // std::cout << mac << std::endl;

    return mac;
}

/////////////////////////////////////////////////////////////////////////////

// const std::string SERVER_ADDRESS("tcp://localhost:1883");
// const std::string CLIENT_ID("paho_cpp_async_subcribe");
// const std::string TOPIC("hello");

// const int qos_ = 1;
// const int N_RETRY_ATTEMPTS = 5;

/////////////////////////////////////////////////////////////////////////////

// Callbacks for the success or failures of requested actions.
// This could be used to initiate further action, but here we just log the
// results to the console.

void action_listener::on_failure(const mqtt::token &tok)
{
    std::cout << name_ << " failure";
    if (tok.get_message_id() != 0)
        std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
    std::cout << std::endl;
}

void action_listener::on_success(const mqtt::token &tok)
{
    std::cout << name_ << " success";
    if (tok.get_message_id() != 0)
        std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
    auto top = tok.get_topics();
    if (top && !top->empty())
        std::cout << "\ttoken topic: '" << (*top)[0] << "', ..." << std::endl;
    std::cout << std::endl;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * Local callback & listener class for use with the client connection.
 * This is primarily intended to receive messages, but it will also monitor
 * the connection to the broker. If the connection is lost, it will attempt
 * to restore the connection and re-subscribe to the topic.
 */

void callback::reconnect()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    try
    {
        cli_.connect(connOpts_, nullptr, *this);
    }
    catch (const mqtt::exception &exc)
    {
        std::cerr << "Error: " << exc.what() << std::endl;
        exit(1);
    }
}

// Re-connection failure
void callback::on_failure(const mqtt::token &tok)
{
    std::cout << "Connection attempt failed" << std::endl;
    if (++nretry_ > N_RETRY_ATTEMPTS)
        exit(1);
    reconnect();
}

// (Re)connection success
// Either this or connected() can be used for callbacks.
void callback::on_success(const mqtt::token &tok) {}

// (Re)connection success
void callback::connected(const std::string &cause)
{
    std::cout << "\nConnection success" << std::endl;
    // std::cout << "\nSubscribing to topic '" << TOPIC << "'\n"
    //           << "\tfor client " << CLIENT_ID
    //           << " using QoS" << qos_ << "\n"
    //           << "\nPress Q<Enter> to quit\n"
    //           << std::endl;

    // cli_.subscribe(cli_id_ + topic_event_, qos_, nullptr, subListener_);
    // cli_.subscribe(cli_id_ + topic_cmd_, qos_, nullptr, subListener_);

    cli_.subscribe(mymqtt.CLIENT_ID + mymqtt.TOPIC_CMD, qos_, nullptr, subListener_);
    // cli_.subscribe(cli_id_ + topic_cmd_, qos_, nullptr, subListener_);
}

// Callback for when the connection is lost.
// This will initiate the attempt to manually reconnect.
void callback::connection_lost(const std::string &cause)
{
    std::cout << "\nConnection lost" << std::endl;
    if (!cause.empty())
        std::cout << "\tcause: " << cause << std::endl;

    std::cout << "Reconnecting..." << std::endl;
    nretry_ = 0;
    reconnect();
}

// Callback for when a message arrives.
void callback::message_arrived(mqtt::const_message_ptr msg)
{
    std::cout << "Message arrived" << std::endl;
    std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
    std::cout << "\tpayload: '" << msg->to_string() << "'\n"
              << std::endl;

    mymqtt.app_mqtt_parsedata(msg->to_string());
}

void callback::delivery_complete(mqtt::delivery_token_ptr token) {}

/*////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////*/

APPMQTT::APPMQTT()
{
    CLIENT_ID = appGetMacAdd();
    cli = new mqtt::async_client(SERVER_ADDRESS, CLIENT_ID);

    connOpts.set_clean_session(false);

    cb = new callback (*cli, connOpts);

    cli->set_callback(*cb);
};

void APPMQTT::app_mqtt_connect(void)
{

    try
    {
        std::cout << "Connecting to the MQTT server..." << std::flush;
        cli->connect(connOpts, nullptr, *cb);
    }
    catch (const mqtt::exception &exc)
    {
        std::cerr << "\nERROR: Unable to connect to MQTT server: '"
                  << SERVER_ADDRESS << "'" << exc << std::endl;
        return;
    }

    // Just block till user tells us to quit.

    // while (std::tolower(std::cin.get()) != 'q')
    //     ;

    // Disconnect

    // app_mqtt_disconnect();
}

/// @brief Disconnect to mqtt server
/// @param
void APPMQTT::app_mqtt_disconnect(void)
{
    try
    {
        std::cout << "\nDisconnecting from the MQTT server..." << std::flush;
        cli->disconnect()->wait();
        std::cout << "OK" << std::endl;
    }
    catch (const mqtt::exception &exc)
    {
        std::cerr << exc << std::endl;
        return;
    }
}

/// @brief Parse data received from mqtt broker
/// @param data
void APPMQTT::app_mqtt_parsedata(std::string data)
{
    std::cout << "Message arrived" << std::endl;
    myjson jData = myjson::parse(data);

    if (jData["type"] == TYPE_CMD_VERSION)
    {
        myprint("Request version.");
    }
    else if (jData["type"] == TYPE_CMD_REBOOT)
    {
        myprint("Request reboot.");
    }
    else if (jData["type"] == TYPE_CMD_DOWNLOAD)
    {
        myprint("Request dowload.");
    }
    else if (jData["type"] == TYPE_CMD_VIEWCAM)
    {
        myprint("Request view cam.");
    }
}
