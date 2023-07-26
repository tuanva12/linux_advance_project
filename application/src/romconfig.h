/*
Cấu hình eeprom cho hệ thống.
Sử dụng ic at24c32 để lưu thông tin cấu hình hệ thống.

##. Cấu hình device tree.
&i2c2{
	myat24c32: myat24c32@50
	{
		compatible = "atmel,24c32";
		reg = <0x50>;
		status = "okay";
	};
};

Sau khi load file .dtb bào board , kết nối kít at24c32 vào board với cổng i2c 2.
Việc giao tiếp với module eeprom sẽ sử dụng đọc ghi file /sys/bus/i2c/devices/0-0050/eeprom.

*/

#ifndef _ROMCONFIG_H__
#define _ROMCONFIG_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <asm/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

class ROMCONFIG
{
public:
    bool romConfigInit(void);
    uint32_t writedata(uint16_t regadd, uint8_t *pData, uint32_t size);
    uint32_t readdata(uint16_t regadd, uint8_t *rData, uint32_t count);
private:
    int epromFileFd = 0;
};

#endif // _ROMCONFIG_H__