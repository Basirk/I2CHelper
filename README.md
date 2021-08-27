I2CHelper
=========

A wrapper for Wire to help simplify I2C communication with new devices.

> **PLEASE NOTE**  
> A request has been made to change the name of this library from "I2C Helper"
> to "I2CHelper". This is to bring it in line with the Arduino library
> recommendations (Lint rule LP015). The pull request can be found here:  
> https://github.com/arduino/library-registry/pull/347  
> If you choose to use this library then you will need to switch to the new one
> once the pull request has been accepted. The code and filenames will remain
> unchanged.

## Summary

So, you've just bought this cool device that can be controlled using I2C. Now what?  
The datasheet describes how you should read and write from certain registers,
and you know that you probably need the Wire library to do that, but Wire is
quite low-level.

This is where I2CHelper comes in. If you create a class for controlling your
new device and inherit I2CHelper, you will then have access to some primitive
methods that allow reading and writing to those I2C registers.

## What do I get?

Just a few methods and a macro to help take some of the pain away.  
`sendCommand()` - send a command (write a byte) to a register.  
`readReg()` - read an unsigned value from up to 4 bytes from a starting register.  
`readRegSigned()` - read a signed value from up to 4 bytes from a starting register.  

`SET_BITS()` - a macro that helps with setting certain bits of a masked byte.

To read values from I2C, you usually send the starting register address
containing the value you want to read, and then request the appropriate number
of bytes to be read. This must all be done within a "transaction". Keeping the
sign bit correct for signed values can be quite awkward. Fortunately, I2CHelper
handles all this for you.

## Initialisation

Your subclass needs to set the device address of the I2C device you want to control:

`uint8_t i2c_device_address`

Optionally, you can also run the `clearBus()` method. This was written by
Matthew Ford, and details of it may be found here:  
http://www.forward.com.au/pfod/ArduinoProgramming/I2C_ClearBus/index.html   
The method is static so may also be run from your main setup() function.

Note that `Wire.begin()` must be called *after* `clearBus()` is used.


## Quick Start

This code is available in the `/examples` directory.

Here is a starting point for your class definition, which inherits from I2CHelper:

```c++
#include <I2CHelper.h>

#define MYI2CDEVICE_DEFAULT_ADDRESS  (0x42)

class MyI2CDevice: public I2CHelper {
    public:
        void begin(uint8_t i2c_address=MYI2CDEVICE_DEFAULT_ADDRESS);
        uint8_t readValueX();
        int32_t readValueY();
};
```

This device has two registers that can be read, X and Y.  
X is a single unsigned byte, and Y is a signed three-byte value.

Here is the implementation:

```c++
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
```

Finally, here's how you would use the class in your sketch:
```c++
#include <Arduino.h>
#include <Wire.h>

#include "MyI2CDevice.h"

MyI2CDevice my_i2c_device = MyI2CDevice();

void setup() {
    // Optionally ensure I2C is correctly reset before starting (use default SDA and SCL pins)
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
```

## Debuging

Define `DEBUG_I2C` to enable serial debugging from the library.


## Disclaimer

This was written to help support the development of a control class for an I2C
device with no existing library. The device I used may use I2C in a different
way to other devices, so it is possible that this library may be entirely
useless to anyone else. Hopefully that's not the case though...


## Contributions

As I program mainly in C and Python, the C++ in this library may not be "optimal".  
I am happy and willing to accept contributions to make it better.
