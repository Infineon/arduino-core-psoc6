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

void pinMode(pin_size_t pinNumber, PinMode pinMode) {
    if (pinNumber > GPIO_PIN_COUNT) {
        return; // Invalid pin number
    }

    cyhal_gpio_direction_t direction;
    cyhal_gpio_drive_mode_t drive_mode;
    bool initPinValue = false;

    switch (pinMode) {
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
    (void)cyhal_gpio_init(mapping_gpio_pin[pinNumber], direction, drive_mode, initPinValue);
}

PinStatus digitalRead(pin_size_t pinNumber) {
    return cyhal_gpio_read(mapping_gpio_pin[pinNumber]) ? HIGH : LOW;
}

void digitalWrite(pin_size_t pinNumber, PinStatus status) {
    cyhal_gpio_write(mapping_gpio_pin[pinNumber], status);
}

#ifdef __cplusplus
}
#endif
