#ifndef SPI_H
#define SPI_H

#include <Arduino.h>
#include "cyhal_spi.h"
#include <HardwareSPI.h>

class SPIClassPSOC: public arduino::HardwareSPI {
public:
    SPIClassPSOC(cyhal_gpio_t mosi, cyhal_gpio_t miso, cyhal_gpio_t sck, cyhal_gpio_t ssel);
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

    cy_rslt_t status;

private:
    arduino::SPISettings const DEFAULT_SPI_SETTINGS = arduino::SPISettings(1000000, MSBFIRST, arduino::SPI_MODE0);

    cyhal_spi_mode_t getSpiMode() const;

    arduino::SPISettings _settings = arduino::SPISettings();
    cyhal_spi_t _spi_obj;
    cyhal_gpio_t _mosi_pin;
    cyhal_gpio_t _miso_pin;
    cyhal_gpio_t _sck_pin;
    cyhal_gpio_t _ssel_pin;

    bool _is_slave;
    bool _is_initialized;
};

#if (SPI_HOWMANY > 0)
extern SPIClassPSOC SPI;
#endif

#endif // SPI_H
