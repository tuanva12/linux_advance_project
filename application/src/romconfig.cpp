//
//
//
//
//

#include "romconfig.h"

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
