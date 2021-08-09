#include <Arduino.h>

#include "MyI2CDevice.h"

#define REG_CONFIG      (0x10)
#define REG_VALUE_X     (0x11) // 1 byte unsigned
#define REG_VALUE_Y     (0x12) // 3 bytes signed

#define REG_CONFIG_FOO_MASK     (0b00000011)
#define REG_CONFIG_BAR_MASK     (0b00001100)

#define REG_CONFIG_FOO_OPTION_A     (0b00000001)
#define REG_CONFIG_FOO_OPTION_B     (0b00000010)

#define REG_CONFIG_BAR_OPTION_A     (0b00000100)
#define REG_CONFIG_BAR_OPTION_B     (0b00001000)


void MyI2CDevice::begin(uint8_t i2c_address) {
    i2c_device_address = i2c_address;

    // To configure an I2C device, sometimes you need to read an I2C register, modify
    // the read value and then write it back again. To keep your code tidy, these
    // registers, masks and settings should be stored in #defines

    // Read the config reg (1 byte)
    uint8_t config = (uint8_t)readReg(REG_CONFIG, 1);

    // Set the FOO field to option A and the BAR field to option B
    SET_BITS(config, REG_CONFIG_FOO_MASK, REG_CONFIG_FOO_OPTION_A);
    SET_BITS(config, REG_CONFIG_BAR_MASK, REG_CONFIG_BAR_OPTION_B);

    // Now write this back to the device
    sendCommand(REG_CONFIG, config);
}

uint8_t MyI2CDevice::readValueX() {
    return (uint8_t)readReg(REG_VALUE_X, 1);
}

int32_t MyI2CDevice::readValueY() {
    return readRegSigned(REG_VALUE_Y, 3);
}
