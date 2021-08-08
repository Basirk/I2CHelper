#include "I2CHelper.h"

#define MYI2CDEVICE_DEFAULT_ADDRESS  (0x42)

class MyI2CDevice: public I2CHelper {
public:
    void begin(uint8_t sensor_i2c_address=MYI2CDEVICE_DEFAULT_ADDRESS);
    uint8_t readValueX();
    int32_t readValueY();
};
