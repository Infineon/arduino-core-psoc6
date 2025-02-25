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
#include <FreeRTOS.h>
#include <task.h>
#include "cyhal_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines */
#define MILLISECONDS_PER_SECOND 1000
#define MICROSECONDS_PER_SECOND 1000000
#define MICROS_TO_MILLISECONDS(us) ((unsigned long)((double)(us) / (double)MILLISECONDS_PER_SECOND));

cyhal_timer_t timer;

#define time_assert(ret) if (ret != CY_RSLT_SUCCESS) { \
            time_error = ret; \
            return; \
}

static cy_rslt_t time_error = CY_RSLT_SUCCESS;
/**
 * This function can be used for debugging purposes.
 * Include in app as extern cy_rslt_t get_time_error();
 */
cy_rslt_t get_time_error() {
    return time_error;
}


void time_init() {
    const cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,                     /* Unused */
        .period = 4294967295,                   /* 2^32 - 1 ticks. With 1MHz freq ~ 70 min overflow.*/
        .direction = CYHAL_TIMER_DIR_UP,
        .is_compare = false,
        .is_continuous = true,
        .value = 0
    };

    cy_rslt_t ret = cyhal_timer_init(&timer, NC, NULL);
    time_assert(ret);

    ret = cyhal_timer_configure(&timer, &timer_cfg);
    time_assert(ret);

    ret = cyhal_timer_set_frequency(&timer, MICROSECONDS_PER_SECOND);
    time_assert(ret);

    ret = cyhal_timer_start(&timer);
    time_assert(ret);
}

unsigned long millis() {
    return MICROS_TO_MILLISECONDS(cyhal_timer_read(&timer));
}

unsigned long micros() {
    return cyhal_timer_read(&timer);
}

void delay(unsigned long ms) {
    const TickType_t xDelay = ms / portTICK_PERIOD_MS;
    vTaskDelay(xDelay);
}

void delayMicroseconds(unsigned int us) {
    const TickType_t xDelay = us / (portTICK_PERIOD_MS * MILLISECONDS_PER_SECOND);
    vTaskDelay(xDelay);
}

#ifdef __cplusplus
}
#endif
