#include <Arduino.h>
#include <Wire.h>

#include "MyI2CDevice.h"

MyI2CDevice my_i2c_device = MyI2CDevice();

void setup() {
    // Ensure I2C is correctly reset before starting (use default SDA and SCL pins)
    my_i2c_device.clearBus();

    // Start Wire (use default SDA and SCL pins)
    Wire.begin();

    // Start the I2C controlled device (use default I2C device address)
    my_i2c_device.begin();
}

void loop() {
    Serial.println("X : " + String(my_i2c_device.readValueX()));
    Serial.println("Y : " + String(my_i2c_device.readValueY()));
    delay(1000);
}
