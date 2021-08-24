#include <assert.h>

#include "I2CHelper.h"

//#define DEBUG_I2C

#ifdef DEBUG_I2C
#define serial_print(...)       Serial.print(__VA_ARGS__)
#define serial_println(...)     Serial.println(__VA_ARGS__)
#else
#define serial_print(...)
#define serial_println(...)
#endif


void I2CHelper::readResponseBytes(int num_bytes, uint8_t *p_buffer) const {
    assert(num_bytes < 4);
    Wire.requestFrom(i2c_device_address, (uint8_t)num_bytes);
    while(Wire.available() < num_bytes)
    serial_print(F("Read bytes :"));
    for (int byte_num = 0; byte_num < num_bytes; byte_num++) {
        // MSB read and stored first
        p_buffer[byte_num] = (uint8_t)Wire.read();
        serial_print(" ");
        serial_print(p_buffer[byte_num], HEX);
    }
    Wire.endTransmission();
    serial_println("");
}


uint32_t I2CHelper::readResponse(int num_bytes) {
    uint8_t response_buffer[4];
    uint32_t response = 0;

    assert(num_bytes <= 4);
    readResponseBytes(num_bytes, response_buffer);

    for (int byte_num = 0; byte_num < num_bytes; byte_num++) {
        response = (response << 8) | response_buffer[byte_num];
    }
    serial_print(F("UInt : "));
    serial_println(response);

    return response;
}


int32_t I2CHelper::readResponseSigned(int num_bytes) {
    uint8_t response_buffer[4];
    uint32_t response = 0;
    int32_t ret_val;
    union {
        uint32_t unsigned_val = 0;
        int32_t signed_val;
    } converter;

    assert(num_bytes <= 4);
    readResponseBytes(num_bytes, response_buffer);

    for (int byte_num = 0; byte_num < num_bytes; byte_num++) {
        response = (response << 8) | response_buffer[byte_num];
    }

    // Convert into a signed 32-bit integer
    int bit_shift = (4 - num_bytes) * 8;
    converter.unsigned_val = response << bit_shift;
    ret_val = converter.signed_val >> bit_shift;

    serial_print(F("Int24 : "));
    serial_println(ret_val);

    return ret_val;
}


void I2CHelper::sendCommand(uint8_t reg, uint8_t cmd) const  {
    serial_print(F("Sending command : ["));
    serial_print(reg, HEX);
    serial_print(F("] "));
    serial_println(cmd, HEX);

    Wire.beginTransmission(i2c_device_address);
    Wire.write(reg);
    Wire.write(cmd);
    Wire.endTransmission();
}


uint32_t I2CHelper::readReg(uint8_t reg, int num_bytes) {
    serial_print(F("Reading reg : ["));
    serial_print(reg, HEX);
    serial_print(F("] "));

    Wire.beginTransmission(i2c_device_address);
    Wire.write(reg);
    Wire.endTransmission();
    return readResponse(num_bytes);
}


int32_t I2CHelper::readRegSigned(uint8_t reg, int num_bytes) {
    serial_print(F("Reading signed reg : ["));
    serial_print(reg, HEX);
    serial_print(F("] "));

    Wire.beginTransmission(i2c_device_address);
    Wire.write(reg);
    Wire.endTransmission();
    return readResponseSigned(num_bytes);
}


// =====================================================================================================================

/**
 * I2C_ClearBus
 * (http://www.forward.com.au/pfod/ArduinoProgramming/I2C_ClearBus/index.html)
 * (c)2014 Forward Computing and Control Pty. Ltd.
 * NSW Australia, www.forward.com.au
 * This code may be freely used for both private and commercial use
 *
 * This routine turns off the I2C bus and clears it
 * on return SCA and SCL pins are tri-state inputs.
 * You need to call Wire.begin() after this to re-enable I2C
 * This routine does NOT use the Wire library at all.
 *
 * returns 0 if bus cleared
 *         1 if SCL held low.
 *         2 if SDA held low by slave clock stretch for > 2sec
 *         3 if SDA held low after 20 clocks.
 */
int I2CHelper::clearBus(int sda, int scl) {
#if defined(TWCR) && defined(TWEN)
    TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
#endif

    pinMode(sda, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
    pinMode(scl, INPUT_PULLUP);

    delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power
    // up of the DS3231 module to allow it to initialize properly,
    // but is also assists in reliable programming of FioV3 boards as it gives the
    // IDE a chance to start uploaded the program
    // before existing sketch confuses the IDE by sending Serial data.

    boolean SCL_LOW = (digitalRead(scl) == LOW); // Check is SCL is Low.
    if (SCL_LOW) { //If it is held low Arduno cannot become the I2C master.
        serial_println(F("I2C bus error. Could not clear!"));
        serial_println(F("SCL clock line held low"));
        return 1; //I2C bus error. Could not clear SCL clock line held low
    }

    boolean SDA_LOW = (digitalRead(sda) == LOW);  // vi. Check SDA input.
    int clockCount = 20; // > 2x9 clock

    while (SDA_LOW && (clockCount > 0)) { //  vii. If SDA is Low,
        clockCount--;
        // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
        pinMode(scl, INPUT); // release SCL pullup so that when made output it will be LOW
        pinMode(scl, OUTPUT); // then clock SCL Low
        delayMicroseconds(10); //  for >5uS
        pinMode(scl, INPUT); // release SCL LOW
        pinMode(scl, INPUT_PULLUP); // turn on pullup resistors again
        // do not force high as slave may be holding it low for clock stretching.
        delayMicroseconds(10); //  for >5uS
        // The >5uS is so that even the slowest I2C devices are handled.
        SCL_LOW = (digitalRead(scl) == LOW); // Check if SCL is Low.
        int counter = 20;
        while (SCL_LOW && (counter > 0)) {  //  loop waiting for SCL to become High only wait 2sec.
            counter--;
            delay(100);
            SCL_LOW = (digitalRead(scl) == LOW);
        }
        if (SCL_LOW) { // still low after 2 sec error
            serial_println(F("I2C bus error. Could not clear!"));
            serial_println(F("SCL clock line held low by slave clock stretch"));
            return 2; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
        }
        SDA_LOW = (digitalRead(sda) == LOW); //   and check SDA input again and loop
    }
    if (SDA_LOW) { // still low
        serial_println(F("I2C bus error. Could not clear!"));
        serial_println(F("SDA data line held low"));
        return 3; // I2C bus error. Could not clear. SDA data line held low
    }

    // else pull SDA line low for Start or Repeated Start
    pinMode(sda, INPUT); // remove pullup.
    pinMode(sda, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
    // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
    /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
    delayMicroseconds(10); // wait >5uS
    pinMode(sda, INPUT); // remove output low
    pinMode(sda, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
    delayMicroseconds(10); // x. wait >5uS
    pinMode(sda, INPUT); // and reset pins as tri-state inputs which is the default state on reset
    pinMode(scl, INPUT);

    serial_println(F("I2C bus error. Could not clear!"));
    return 0; // all ok
}
