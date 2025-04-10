#ifndef SPI_H
#define SPI_H

#include "Arduino.h"
#include "cyhal_spi.h"
#include "api/HardwareSPI.h"

#define ARDUINO_SPI_CLOCK 1000000

class SPIClassPSOC: public arduino::HardwareSPI {
public:
    SPIClassPSOC(pin_size_t mosi, pin_size_t miso, pin_size_t sck, pin_size_t ssel = SPI1_SSEL_PIN_DEFAULT, bool is_slave = false);
    ~SPIClassPSOC();

    virtual byte transfer(uint8_t data);
    virtual uint16_t transfer16(uint16_t data);
    virtual void transfer(void *buf, size_t count);

    virtual void usingInterrupt(int interruptNumber);
    virtual void notUsingInterrupt(int interruptNumber);
    virtual void beginTransaction(SPISettings settings);
    virtual void endTransaction(void);

    virtual void attachInterrupt();
    virtual void detachInterrupt();

    virtual void begin();
    virtual void end();

    void setDataMode(uint8_t dataMode);
    void setBitOrder(uint8_t bitOrder);
    void setClockDivider(uint8_t div);

    cy_rslt_t status = CY_RSLT_TYPE_ERROR;

private:
    arduino::SPISettings const DEFAULT_SPI_SETTINGS = arduino::SPISettings(ARDUINO_SPI_CLOCK, MSBFIRST, arduino::SPI_MODE0);

    cyhal_spi_mode_t getSpiMode() const;

    arduino::SPISettings _settings = arduino::SPISettings();
    cyhal_spi_t _spi_obj;
    pin_size_t _mosi_pin;
    pin_size_t _miso_pin;
    pin_size_t _sck_pin;
    pin_size_t _ssel_pin;

    bool _is_slave;
    bool _is_initialized;
};

#if (SPI_HOWMANY > 0)
extern SPIClassPSOC SPI;
#endif

#endif // SPI_H
