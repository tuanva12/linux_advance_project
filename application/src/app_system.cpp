/*
 ******************************************************************************
 * @file      : app_savedata.cpp
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
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <fstream>

#include "define.h"
#include "app_system.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/* External variables --------------------------------------------------------*/
extern flag_struct_t controlFlag;
/******************************************************************************/
/*                              FUNCTION                                      */
/******************************************************************************/

static void timeCallback100(void)
{
    while (true)
    {
        controlFlag.captureFlag = true;
        /* Sleep in one second */
        usleep(60 * 1000000);
    }
}

MYSYSTEM::MYSYSTEM(std::string rootDir)
{
    /* Ensure that last charater of rootfs folder is '/' character */
    if (rootDir.back() != '/')
    {
        rootDir = rootDir + "/";
    }

    rootFolder = rootDir;
}
    /// @brief Check and create directory
    /// @param typeData folder data: "image" for save image from camera
    ///                              "data" for store log data
std::string MYSYSTEM::CreateDirForSaveData(std::string typeData)
{
    std::string day;
    std::string month;
    std::string year;

    std::string tempName;

    /* Get datetime */
    time_t now = time(0);
    tm *ltm = localtime(&now);
    day = std::to_string(ltm->tm_mday);
    month = std::to_string(ltm->tm_mon + 1);
    year = std::to_string(ltm->tm_year + 1900);

    tempName = rootFolder + typeData;
    mkdir(tempName.c_str(), 0777);

    tempName = tempName + "/" + year;
    mkdir(tempName.c_str(), 0777);

    tempName = tempName + "/" + month;
    mkdir(tempName.c_str(), 0777);

    tempName = tempName + "/" + day;
    mkdir(tempName.c_str(), 0777);

    return tempName + "/";
}

void MYSYSTEM::mainRoutine(void)
{
    /* Init timer 100ms */
    std::thread threadTimer(timeCallback100);

    /* Loop */

    while (true)
    {
        /* Capture image in 1 minute perios */
        if (controlFlag.captureFlag)
        {
            if (controlFlag.streamFlag != true)
            {
                /* Capture Image */
                myprint("Start capture image");
                std::string fileName = CreateDirForSaveData("image");
                time_t now = time(0);
                tm *ltm = localtime(&now);
                fileName = fileName + std::to_string(ltm->tm_min) + ".txt";

                std::ofstream MyFile(fileName);
                MyFile << "Files can be tricky, but it is fun enough!";
                MyFile.close();

                controlFlag.captureFlag = false;
            }
        }
        /* Process when get stream command */
        if (controlFlag.streamFlag)
        {
            myprint("Start stream camera through RTSP");

            controlFlag.streamFlag = false;
        }

        /* Delay in 100ms */
        usleep(100000);
    }
}

/************************ (C) VO ANH TUAN *********************END OF FILE****/
