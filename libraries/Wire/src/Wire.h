#ifndef WIRE_H
#define WIRE_H

#include <Arduino.h>
#include "cyhal_i2c.h"
#include "api/RingBuffer.h"
#include "api/HardwareI2C.h"
#include <map>


class TwoWire: public arduino::HardwareI2C {
public:

    static const size_t BUFFER_LENGTH = 256;
    static const uint32_t I2C_DEFAULT_FREQ = 100000;

    TwoWire(pin_size_t sda, pin_size_t scl);

    enum I2C_ErrorCodes {
        I2C_SUCCESS = CY_RSLT_SUCCESS,
        I2C_NO_DEVICE_ATTACHED_PULL_UP = 0xAA2004,
        I2C_NO_DEVICE_ATTACHED_NO_PULL_UP = 0xAA2003,
        I2C_TIMEOUT = CYHAL_I2C_RSLT_WARN_TIMEOUT
    };

    void begin();
    void begin(uint8_t address);
    void end();
    void setClock(uint32_t freq);
    void beginTransmission(uint8_t address);
    uint8_t endTransmission(void);
    uint8_t endTransmission(bool sendStop);
    size_t requestFrom(uint8_t address, size_t quantity, bool stopBit);
    size_t requestFrom(uint8_t address, size_t len);
    size_t write(uint8_t data);
    using Print::write;   // pull in write(str) and write(buf, size) from Print

    int available(void);
    int read(void);
    int peek(void);
    void onReceive(void (*function)(int));
    void onRequest(void (*function)(void));

private:
    void _begin();
    pin_size_t sda_pin = NC;
    pin_size_t scl_pin = NC;
    bool is_master = true;
    uint16_t slave_address = 0;
    arduino::RingBufferN < BUFFER_LENGTH > rxBuffer = {};
    arduino::RingBufferN < BUFFER_LENGTH > txBuffer = {};
    uint8_t temp_rx_buff[BUFFER_LENGTH] = {0};
    uint8_t temp_tx_buff[BUFFER_LENGTH] = {0};
    cyhal_i2c_cfg_t i2c_config = {};
    cyhal_i2c_t i2c_obj = {};
    cy_rslt_t w_status = {};
    uint32_t timeout = 0; // Timeout in milliseconds
    void (*user_onRequest)(void) = nullptr;
    void (*user_onReceive)(int) = nullptr;
    void onRequestService(void);
    void onReceiveService(void);
    static void i2c_event_handler(void *callback_arg, cyhal_i2c_event_t event);
    void i2c_event_handler_member(cyhal_i2c_event_t event);

};

#if (I2C_HOWMANY > 0)
extern TwoWire Wire;
#endif

#if (I2C_HOWMANY > 1)
extern TwoWire Wire1;
#endif

#endif // WIRE_H
