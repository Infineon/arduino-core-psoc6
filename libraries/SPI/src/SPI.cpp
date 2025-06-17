#include "SPI.h"

#define spi_assert(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
            return; \
}

SPIClassPSOC::SPIClassPSOC(pin_size_t mosi, pin_size_t miso, pin_size_t sck, pin_size_t ssel, bool is_slave)
    : _mosi_pin(mosi), _miso_pin(miso), _sck_pin(sck), _ssel_pin(ssel), _is_slave(is_slave), _is_initialized(false) {

}

SPIClassPSOC::~SPIClassPSOC() {
    end();
}

void SPIClassPSOC::begin() {
    if (_is_initialized) {
        return;
    }
    /* If the user does not define them (or pass NC), they are not connected. No Arduino GPIO mapping */
    cyhal_gpio_t cy_ssel_pin = (_ssel_pin == NC) ? NC : mapping_gpio_pin[_ssel_pin];
    /* If master, the SS pin driven external. Thus, always NC */
    cy_ssel_pin = _is_slave ? cy_ssel_pin : NC;

    status = cyhal_spi_init(&_spi_obj, mapping_gpio_pin[_mosi_pin], mapping_gpio_pin[_miso_pin], mapping_gpio_pin[_sck_pin], cy_ssel_pin, NULL, 8, getSpiMode(), _is_slave);

    spi_assert(status);
    status = cyhal_spi_set_frequency(&_spi_obj, _settings.getClockFreq());
    spi_assert(status);
    _is_initialized = true;
}

void SPIClassPSOC::end() {
    if (!_is_initialized) {
        return;
    }
    status = cyhal_spi_clear(&_spi_obj);
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
    uint8_t tranceive_data = data;
    transfer(&tranceive_data, 1);
    return tranceive_data;
}

uint16_t SPIClassPSOC::transfer16(uint16_t data) {
// Union to split 16-bit data into two 8-bit data
    union { uint16_t val;
            struct { uint8_t lsb = 0;
                     uint8_t msb = 0;
            };
    } data_in_out;

    data_in_out.val = data;

    if (_settings.getBitOrder() == LSBFIRST) {
        transfer(&data_in_out.lsb, 1);
        transfer(&data_in_out.msb, 1);
    } else {
        transfer(&data_in_out.msb, 1);
        transfer(&data_in_out.lsb, 1);
    }

    return data_in_out.val;
}

void SPIClassPSOC::transfer(void *buf, size_t count) {
    uint8_t *buffer = reinterpret_cast < uint8_t * > (buf);
    const uint8_t *tx_buf;
    uint8_t tx_temp_buf[count];

    memcpy(tx_temp_buf, buffer, count);
    tx_buf = tx_temp_buf;

    if (count > 0) {
        status = cyhal_spi_transfer(&_spi_obj, tx_buf, count, buffer, count, 0xFF);
        spi_assert(status);
    }
}

void SPIClassPSOC::usingInterrupt(int interruptNumber) {
    // The SPI transfer functions are interrupt-driven, so this operation is not supported.
}

void SPIClassPSOC::notUsingInterrupt(int interruptNumber) {
    // The SPI transfer functions are interrupt-driven, so this operation is not supported.
}

void SPIClassPSOC::attachInterrupt() {
    // Since SPI transfer functions rely on interrupts, attaching a separate interrupt is not applicable.
}

void SPIClassPSOC::detachInterrupt() {
    // Detaching interrupts is not applicable as SPI transfer functions are already interrupt-driven.
}

void SPIClassPSOC::setDataMode(uint8_t dataMode) {
    // This functionality is deprecated in the Arduino core API,
    // retained only for backward compatibility and no longer recommended for use.
}

void SPIClassPSOC::setBitOrder(uint8_t bitOrder) {
    // This functionality is deprecated in the Arduino core API,
    // retained only for backward compatibility and no longer recommended for use.
}

void SPIClassPSOC::setClockDivider(uint8_t div) {
    // This functionality is deprecated in the Arduino core API,
    // retained only for backward compatibility and no longer recommended for use.
}

void SPIClassPSOC::beginTransaction(arduino::SPISettings settings) {
    if (!_is_initialized) {
        _settings = settings;
        begin();
    }
    if (_settings != settings) {
        end();
        _settings = settings;
        begin();
    }
}

void SPIClassPSOC::endTransaction() {
    // do nothing
}

#if SPI_HOWMANY > 0
SPIClassPSOC SPI = SPIClassPSOC(PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_SCK);
#endif
