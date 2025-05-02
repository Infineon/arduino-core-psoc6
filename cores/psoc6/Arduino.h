/*
  Arduino.h - Main include file for the Arduino SDK
  Copyright (c) 2014 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef Arduino_h
#define Arduino_h

#include "api/ArduinoAPI.h"
#include "cyhal_gpio.h"

#define RAMSTART (HMCRAMC0_ADDR)
#define RAMSIZE  (HMCRAMC0_SIZE)
#define RAMEND   (RAMSTART + RAMSIZE - 1)

#ifdef __cplusplus

using namespace arduino;

extern "C" {
#endif // __cplusplus

// ****************************************************************************
// @Imported Global Variables
// ****************************************************************************
extern const cyhal_gpio_t mapping_gpio_pin[];
extern const uint8_t GPIO_PIN_COUNT;

#define GPIO_INTERRUPT_PRIORITY 3 // GPIO interrupt priority
#define digitalPinToInterrupt(p) ((p) < GPIO_PIN_COUNT ? (p) : -1)
#define PWM_FREQUENCY_HZ    1000  // 1 kHz
void analogWriteResolution(int res);

#undef LITTLE_ENDIAN

#define clockCyclesPerMicrosecond() (SystemCoreClock / 1000000L)
#define clockCyclesToMicroseconds(a) (((a) * 1000L) / (SystemCoreClock / 1000L))
#define microsecondsToClockCycles(a) ((a) * (SystemCoreClock / 1000000L))

typedef enum {
    DEFAULT
} analog_reference_t;

#ifdef __cplusplus
} // extern "C"
#endif


// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif // abs

#define abs(x) ((x) > 0?(x):-(x))

// Globally enable or disable interrupts
#define interrupts() __enable_irq()
#define noInterrupts() __disable_irq()

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif


// ARM toolchain doesn't provide itoa etc, provide them
#include "api/itoa.h"

#include "pins_arduino.h"

#ifdef __cplusplus
#include "Uart.h"
#endif

#define Serial _UART1_
#define Serial1 _UART2_

#endif // Arduino_h
