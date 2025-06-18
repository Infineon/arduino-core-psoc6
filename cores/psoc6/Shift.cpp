#include "Arduino.h"

uint8_t shiftIn(pin_size_t ulDataPin, pin_size_t ulClockPin, BitOrder ulBitOrder) {
    uint8_t value = 0;

    cyhal_gpio_t data_pin = mapping_gpio_pin[ulDataPin];
    cyhal_gpio_t clock_pin = mapping_gpio_pin[ulClockPin];

    for (uint8_t i = 0; i < 8; ++i) {
        cyhal_gpio_write(clock_pin, HIGH);

        if (ulBitOrder == LSBFIRST) {
            value |= cyhal_gpio_read(data_pin) << i;
        } else {
            value |= cyhal_gpio_read(data_pin) << (7 - i);
        }

        cyhal_gpio_write(clock_pin, LOW);
    }

    return value;
}

void shiftOut(pin_size_t ulDataPin, pin_size_t ulClockPin, BitOrder ulBitOrder, uint8_t ulVal) {
    cyhal_gpio_t data_pin = mapping_gpio_pin[ulDataPin];
    cyhal_gpio_t clock_pin = mapping_gpio_pin[ulClockPin];

    for (uint8_t i = 0; i < 8; i++) {
        if (ulBitOrder == LSBFIRST) {
            cyhal_gpio_write(data_pin, !!(ulVal & (1 << i)));
        } else {
            cyhal_gpio_write(data_pin, !!(ulVal & (1 << (7 - i))));
        }

        cyhal_gpio_write(clock_pin, HIGH);
        cyhal_gpio_write(clock_pin, LOW);
    }
}
