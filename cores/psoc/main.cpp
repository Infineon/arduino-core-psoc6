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

#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

#include <FreeRTOS.h>
#include <task.h>

#include "time.h"

#define ARDUINO_MAIN_TASK_STACK_SIZE    (4096u)
#define ARDUINO_MAIN_TASK_PRIORITY      (2u)

TaskHandle_t arduino_main_task_handle;

void arduino_main_task(void *arg);

// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() {
}

/*
 * \brief Main entry point of Arduino application
 */
int main(void) {
    cy_rslt_t result;

    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS) {
        CY_ASSERT(0);
    }

    time_init();

    xTaskCreate(arduino_main_task, "arduino-main-task", ARDUINO_MAIN_TASK_STACK_SIZE, NULL, ARDUINO_MAIN_TASK_PRIORITY, &arduino_main_task_handle);
    vTaskStartScheduler();

    /* Should never get here */
    CY_ASSERT(0);
    return 0;
}

void arduino_main_task(void *arg) {

    __enable_irq();
    initVariant();

    setup();

    for (;;)
    {
        loop();
        if (arduino::serialEventRun) {
            arduino::serialEventRun();
        }
    }
}
