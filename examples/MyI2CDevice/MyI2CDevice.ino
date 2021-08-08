#include <Arduino.h>

#include "MyI2CDevice.h"

MyI2CDevice my_i2c_device = MyI2CDevice();

void setup() {
    my_i2c_device.begin();
}

void loop() {
    Serial.println("X : " + String(my_i2c_device.readValueX()));
    Serial.println("Y : " + String(my_i2c_device.readValueY()));
    delay(1000);
}
