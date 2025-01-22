/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "cyhal_gpio.h"

void pinMode(pin_size_t pin, PinMode mode) {
    if (pin > GPIO_PIN_COUNT) {
        return; // Invalid pin number
    }

    cyhal_gpio_direction_t direction;
    cyhal_gpio_drive_mode_t drive_mode;
    bool initPinValue = false;

    switch (mode)
    {
        case INPUT:
            direction = CYHAL_GPIO_DIR_INPUT;
            drive_mode = CYHAL_GPIO_DRIVE_NONE;
            break;

        case INPUT_PULLUP:
            direction = CYHAL_GPIO_DIR_INPUT;
            drive_mode = CYHAL_GPIO_DRIVE_PULLUP;
            initPinValue = true;
            break;

        case INPUT_PULLDOWN:
            direction = CYHAL_GPIO_DIR_INPUT;
            drive_mode = CYHAL_GPIO_DRIVE_PULLDOWN;
            break;

        case OUTPUT:
            direction = CYHAL_GPIO_DIR_OUTPUT;
            drive_mode = CYHAL_GPIO_DRIVE_STRONG;
            break;

        case OUTPUT_OPENDRAIN:
            direction = CYHAL_GPIO_DIR_OUTPUT;
            drive_mode = CYHAL_GPIO_DRIVE_OPENDRAINDRIVESLOW;
            break;

        default:
            return; // Invalid mode
    }

    // Initialize the GPIO pin with the specified direction, drive mode and set initial value
    (void)cyhal_gpio_init(mapping_gpio_pin[pin], direction, drive_mode, initPinValue);
}

uint8_t digitalRead(uint8_t pin) {
    return cyhal_gpio_read(mapping_gpio_pin[pin]) ? HIGH : LOW;
}

void digitalWrite(uint8_t pin, uint8_t value) {
    cyhal_gpio_write(mapping_gpio_pin[pin], value);
}

#ifdef __cplusplus
}
#endif
