/*
 ******************************************************************************
 * @file      : romconfig.cpp
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
#include "romconfig.h"
#include "define.h"
#include <nlohmann/json.hpp>

#include <fstream>
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/* External variables --------------------------------------------------------*/
extern app_config_db_t AppDataBase; /*  */

/******************************************************************************/
/*                              FUNCTION                                      */
/******************************************************************************/

bool ROMCONFIG::romConfigInit(void)
{
    epromFileFd = open("/sys/bus/i2c/devices/0-0050/eeprom", O_RDWR);
    if (epromFileFd < 0)
    {
        perror("can not open eeprom file.\n");
        return false;
    }
    return true;
}

uint32_t ROMCONFIG::writedata(uint16_t regadd, uint8_t *pData, uint32_t size)
{
    lseek(epromFileFd, regadd, SEEK_SET);
    return write((int)epromFileFd, (void*)pData, size);
}

uint32_t ROMCONFIG::readdata(uint16_t regadd, uint8_t *rData, uint32_t count)
{
    lseek(epromFileFd, regadd, SEEK_SET);
    return read((int)epromFileFd, (void*)rData, count);
}


/// @brief Read data config stored in json file
/// @param
void DatabaseRead(void)
{
    using json = nlohmann::json;

    std::ifstream f("setting.json");
    json dataJson = json::parse(f);

    AppDataBase.mqttIp = dataJson["mqttIp"];
    AppDataBase.mqttPort = dataJson["mqttPort"];
    AppDataBase.ftpServerIP = dataJson["ftpServerIP"];
    AppDataBase.ftpUsr = dataJson["ftpUsr"];
    AppDataBase.ftpPwd = dataJson["ftpPwd"];
}

/************************ (C) VO ANH TUAN *********************END OF FILE****/