#ifndef TwoWire_h
#define TwoWire_h

#include <Arduino.h>
#include <api/HardwareI2C.h>
#include <cybsp.h>

#define BUFFER_LENGTH 32

class TwoWire : public arduino::HardwareI2C
{
  public:
    TwoWire(cyhal_gpio_t sda, cyhal_gpio_t scl); // Constructor declaration

    void begin();
    void _begin();
    void begin(uint16_t);
    void begin(uint8_t);
    void begin(int);
    void end();

    void setClock(uint32_t);

    void beginTransmission(uint8_t);
    uint8_t endTransmission(bool);
    uint8_t endTransmission(void);

    size_t requestFrom(uint8_t, size_t, bool);
    size_t requestFrom(uint8_t, size_t);

    virtual size_t write(uint8_t);
    // virtual size_t write(std::string);
    virtual size_t write(const uint8_t *, size_t);

    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);

    void onReceive(void(*)(int));
    void onRequest(void(*)(void));

    private:

    bool is_master;
    cyhal_gpio_t sda_pin;
    cyhal_gpio_t scl_pin;
    cyhal_i2c_t i2c_obj;
    cyhal_i2c_cfg_t i2c_config;
    uint16_t slave_address;
    uint32_t actualfreq;
    uint32_t timeout=0;
    bool send_stop;
    size_t len;
    uint8_t txAddress;
    uint8_t txBuffer[BUFFER_LENGTH];
    size_t txBufferLength;
    size_t txBufferIndex;
    uint8_t rxBuffer[BUFFER_LENGTH];
    size_t rxBufferLength;
    size_t rxBufferIndex;
    void (*user_onReceive)(int); // Add this declaration
    void (*user_onRequest)(void);
    static void i2c_event_handler(void *callback_arg, cyhal_i2c_event_t event); // Add this declaration
    
};
extern TwoWire Wire;
#endif