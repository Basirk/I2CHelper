#include <Wire.h>


#define SET_BITS(n, m, v) (((n) & ~(m)) | (v))


class I2CHelper {

private:
    void readResponseBytes(int num_bytes, uint8_t *p_buffer) const;
    uint32_t readResponse(int num_bytes);
    int32_t readResponseSigned(int num_bytes);

protected:
    uint8_t i2c_device_address{};

    void sendCommand(uint8_t reg, uint8_t cmd) const;
    uint32_t readReg(uint8_t reg, int num_bytes);
    int32_t readRegSigned(uint8_t reg, int num_bytes);

public:
    /**
     * I2C_ClearBus
     * (http://www.forward.com.au/pfod/ArduinoProgramming/I2C_ClearBus/index.html)
     * (c)2014 Forward Computing and Control Pty. Ltd.
     */
    static int clearBus(int scl=SCL, int sda=SDA);
};