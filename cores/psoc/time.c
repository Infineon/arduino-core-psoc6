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
#include "cy_systick.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines */
#define CLOCK_FREQUENCY 8000000
#define MILLISECONDS_PER_SECOND 1000
#define SYSTICK_RELOAD_VAL                                                     \
  (CLOCK_FREQUENCY / MILLISECONDS_PER_SECOND - 1) // For 1 ms tick

volatile uint32_t systick_count = 0; // Counter to keep track of milliseconds

void time_increment_millisecond_counter(void) { systick_count++; }

void time_init() {
  /* Initialize the systick, set the 8MHz IMO as clock source */
  Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_IMO, SYSTICK_RELOAD_VAL);

  /* Set Systick interrupt callback */
  Cy_SysTick_SetCallback(0, time_increment_millisecond_counter);

  /* Enable Systick and the Systick interrupt */
  Cy_SysTick_Enable();
}

unsigned long millis() {
  // Return the number of milliseconds since the program started
  return systick_count;
}

unsigned long micros() {
  uint32_t ticks = Cy_SysTick_GetValue();
  // Calculate the number of microseconds since the program started
  return (systick_count * MILLISECONDS_PER_SECOND) +
         ((SYSTICK_RELOAD_VAL - ticks) * MILLISECONDS_PER_SECOND /
          (SYSTICK_RELOAD_VAL + 1));
}

void delay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    // Yield to allow other processes to run
    yield();
  }
}

void delayMicroseconds(unsigned int us) {
  unsigned long start = micros();
  while (micros() - start < us) {
    // Yield to allow other processes to run
    yield();
  }
}

#ifdef __cplusplus
}
#endif
