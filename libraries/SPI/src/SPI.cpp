#include "SPI.h"

#define spi_assert(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
            return; \
}


SPIClassPSOC::SPIClassPSOC(cyhal_gpio_t mosi, cyhal_gpio_t miso, cyhal_gpio_t sck, cyhal_gpio_t ssel)
    : _mosi_pin(mosi), _miso_pin(miso), _sck_pin(sck), _ssel_pin(ssel), _is_initialized(false), _is_slave(false) {
}

SPIClassPSOC::~SPIClassPSOC() {
    end();
}

void SPIClassPSOC::begin() {
    cy_rslt_t status;
    if (_is_initialized) {
        return;
    }

    status = cyhal_spi_init(&_spi_obj, _mosi_pin, _miso_pin, _sck_pin, _ssel_pin, NULL, 8, getSpiMode(), _is_slave);
    spi_assert(status);
    status = cyhal_spi_set_frequency(&_spi_obj, _settings.getClockFreq());
    spi_assert(status);
    _is_initialized = true;
}

void SPIClassPSOC::end() {
    if (!_is_initialized) {
        return;
    }

    cyhal_spi_free(&_spi_obj);
    _is_initialized = false;
}

cyhal_spi_mode_t SPIClassPSOC::getSpiMode() const {
    cyhal_spi_mode_t mode = CYHAL_SPI_MODE_00_MSB;

    switch (_settings.getDataMode()) {
        case SPI_MODE0:
            mode = (_settings.getBitOrder() == MSBFIRST) ? CYHAL_SPI_MODE_00_MSB : CYHAL_SPI_MODE_00_LSB;
            break;
        case SPI_MODE1:
            mode = (_settings.getBitOrder() == MSBFIRST) ? CYHAL_SPI_MODE_01_MSB : CYHAL_SPI_MODE_01_LSB;
            break;
        case SPI_MODE2:
            mode = (_settings.getBitOrder() == MSBFIRST) ? CYHAL_SPI_MODE_10_MSB : CYHAL_SPI_MODE_10_LSB;
            break;
        case SPI_MODE3:
            mode = (_settings.getBitOrder() == MSBFIRST) ? CYHAL_SPI_MODE_11_MSB : CYHAL_SPI_MODE_11_LSB;
            break;
        default:
            break;
    }

    return mode;
}

byte SPIClassPSOC::transfer(uint8_t data) {
    uint8_t receive_data = 0;
    status = cyhal_spi_transfer(&_spi_obj, &data, 1, &receive_data, 1, 0xFF);
    spi_assert(status);

    return receive_data;
}

uint16_t SPIClassPSOC::transfer16(uint16_t data) {
// Union to split 16-bit data into two 8-bit data
    union { uint16_t val;
            struct { uint8_t lsb;
                     uint8_t msb;
            };
    } data_in_out;

    data_in_out.val = data;

    if (_settings.getBitOrder() == LSBFIRST) {
        data_in_out.lsb = transfer(data_in_out.lsb);
        data_in_out.msb = transfer(data_in_out.msb);
    } else {
        data_in_out.msb = transfer(data_in_out.msb);
        data_in_out.lsb = transfer(data_in_out.lsb);
    }

    return data_in_out.val;
}

void SPIClassPSOC::transfer(void *buf, size_t count) {
    uint8_t *buffer = reinterpret_cast < uint8_t * > (buf);
    for (size_t i = 0; i < count; i++) {
        *buffer = transfer(*buffer);
        buffer++;
    }
}

void SPIClassPSOC::usingInterrupt(int interruptNumber) {
// TODO: Implement
}

void SPIClassPSOC::notUsingInterrupt(int interruptNumber) {
// TODO: Implement
}

void SPIClassPSOC::attachInterrupt() {
// TODO: Implement
}

void SPIClassPSOC::detachInterrupt() {
// TODO: Implement
}

void SPIClassPSOC::beginTransaction(arduino::SPISettings settings) {
    if (!_is_initialized) {
        _settings = settings;
        begin();
    }
}
void SPIClassPSOC::endTransaction() {
    end();
}

#if SPI_HOWMANY > 0
SPIClassPSOC SPI = SPIClassPSOC(SPI1_MOSI_PIN, SPI1_MISO_PIN, SPI1_SCK_PIN, SPI1_SSEL_PIN);
#endif
