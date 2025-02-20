#ifndef WIRE_H
#define WIRE_H

#include <Arduino.h>
#include "cyhal_i2c.h"

class TwoWire {
public:
    
    static const size_t BUFFER_LENGTH = 32;
    static const uint32_t I2C_DEFAULT_FREQ = 100000;

    TwoWire(cyhal_gpio_t sda, cyhal_gpio_t scl, uint8_t instance = 0);

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
    size_t write(const uint8_t *data, size_t quantity);
    size_t write(const char *str);
    int available(void);
    int read(void);
    int peek(void);
    void onReceive(void (*function)(int));
    void onRequest(void (*function)(void));

private:
    static TwoWire *instances[I2C_HOWMANY];
    static cyhal_i2c_t i2c_objs[I2C_HOWMANY];
    void _begin();
    cyhal_gpio_t sda_pin;
    cyhal_gpio_t scl_pin;
    uint8_t instance;
    bool is_master;
    uint16_t slave_address;
    static uint8_t rxBuffer[BUFFER_LENGTH];
    static uint8_t txBuffer[BUFFER_LENGTH];
    static size_t rxBufferIndex;
    static size_t rxBufferLength;
    static size_t txBufferIndex;
    static size_t txBufferLength;
    static uint8_t txAddress;
    cyhal_i2c_cfg_t i2c_config;
    cyhal_i2c_t i2c_obj;
    uint32_t timeout = 0; // Timeout in milliseconds
    static void (*user_onRequest)(void);
    static void (*user_onReceive)(int);
    uint8_t onRequestService(void);
    void onReceiveService(int);
    static void i2c_event_handler(void *callback_arg, cyhal_i2c_event_t event);

};

#if (I2C_HOWMANY > 0)
extern TwoWire Wire;
#endif

#if (I2C_HOWMANY > 1)
extern TwoWire Wire1;
#endif

#endif // WIRE_H