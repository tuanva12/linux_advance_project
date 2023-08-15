/*
 ******************************************************************************
 * @file      : main.cpp
 * @Created on: Aug 1, 2023
 * @author    : Tuann
 ******************************************************************************
 * @brief
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
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
#include "app_ftpupload.h"
#include "app_system.h"
#include "capture_camera.h"
#include "define.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define XRES 1280
#define YRES 720
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/* External variables --------------------------------------------------------*/
ROMCONFIG romConfig; /*  */
flag_struct_t controlFlag;

APPMQTT mymqtt;      /*  */
FtpUpload myFtpUpload;   /*  */

app_config_db_t AppDataBase; /*  */

Webcam webcam("/dev/video0", XRES, YRES);
/******************************************************************************/
/*                              FUNCTION                                      */
/******************************************************************************/

/// @brief
/// @param arg
/// @return
void *exe_thread_connection(void *arg)
{
    /* Init for mqtt connect */
    mymqtt.appMqttInit(AppDataBase.mqttIp, AppDataBase.mqttPort);

    /* Init for FTP upload file */
    myFtpUpload.Init(AppDataBase.ftpServerIP, AppDataBase.ftpUsr, AppDataBase.ftpPwd);

    /* Connect to MQTT */
    while (mymqtt.app_mqtt_connect() != true)
    {
        myprint("Error while connect to MQTT");
        sleep(1);
    }
    myprint("Connected to MQTT");

    // Just block till user tells us to quit.

    while (std::tolower(std::cin.get()) != 'q')
        ;

    // Disconnect

    mymqtt.app_mqtt_disconnect();

    pthread_exit(NULL);
}


/// @brief
/// @param arg
/// @return
void *exe_thread_system(void *arg)
{
    MYSYSTEM sysControl(AppDataBase.dirRootStore);

    /* Main Rountine for capture, save log, setting sys time */
    sysControl.mainRoutine();

    pthread_exit(NULL);
}

/**
 * Main Function
 */
int main()
{
    /* Declare for multi thread */
    pthread_t comunicateThread;
    pthread_t systemThread;

    /* INIT Database */
    DatabaseRead();

    /* Create thread for connect MQTT, ftp file tranfer */
    pthread_create(&comunicateThread, NULL, exe_thread_connection, (void *)"comunicateThread");

    /* Create thread for capture image, save in root, stream camera, ... */
    pthread_create(&systemThread, NULL, exe_thread_system, (void *)"systemThread");

    /* Wait for end thread */
    pthread_join(comunicateThread, NULL);
    pthread_join(systemThread, NULL);

    printf("Vo Anh Tuan\n");
    return 0;
}

/************************ (C) VO ANH TUAN *********************END OF FILE****/