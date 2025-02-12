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

// ****************************************************************************
// @Project Includes
// ****************************************************************************
#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

static cyhal_gpio_callback_data_t gpio_callback_data;

void attachInterrupt(pin_size_t interruptNumber, voidFuncPtr callback, PinStatus mode) {
    if ((interruptNumber < GPIO_PIN_COUNT) || (digitalPinToInterrupt(interruptNumber) >= 0)) {

        cyhal_gpio_event_t event;
        cyhal_gpio_t pin = mapping_gpio_pin[interruptNumber];

        switch (mode) {
        case CHANGE:
            event = CYHAL_GPIO_IRQ_BOTH;
            break;

        case RISING:
            event = CYHAL_GPIO_IRQ_RISE;
            break;

        case FALLING:
            event = CYHAL_GPIO_IRQ_FALL;
            break;

        default:
            event = CYHAL_GPIO_IRQ_NONE;
            break;
        }

        gpio_callback_data.callback = (void (*)(void *, cyhal_gpio_event_t))callback;
        cyhal_gpio_register_callback(pin, &gpio_callback_data);
        cyhal_gpio_enable_event(pin, event, GPIO_INTERRUPT_PRIORITY, true);
    }
}

void detachInterrupt(pin_size_t interruptNumber) {

    if ((interruptNumber < GPIO_PIN_COUNT) || (digitalPinToInterrupt(interruptNumber) >= 0)) {

        cyhal_gpio_t pin = mapping_gpio_pin[interruptNumber];
        cyhal_gpio_enable_event(pin, CYHAL_GPIO_IRQ_NONE, 0, false);
        cyhal_gpio_register_callback(pin, NULL);
    }
}

#ifdef __cplusplus
}
#endif
