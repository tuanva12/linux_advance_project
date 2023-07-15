# linux_advance_project

# Kiểm tra cổng i2c hiện tại.
i2cdetect -l

# Detect các thiết bị slave trên cổng i2c 2
i2cdetect -y 2

#  writes the value 0x40 to register address 0x3030 at device address 0x50.
i2cset -y -f 2 0x50 0x00 0x00 0x61 0x61 0x61 0x61 i

