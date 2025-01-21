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

#ifndef PINS_ARDUINO_H_
#define PINS_ARDUINO_H_

//****************************************************************************
// @Project Includes
//****************************************************************************
#include "cybsp.h"

//****************************************************************************
// @Defines
//****************************************************************************

/****** UART CORE DEFINES ******/

#define SERIAL_HOWMANY		2
#define UART1_TX_PIN        CYBSP_DEBUG_UART_TX
#define UART1_RX_PIN        CYBSP_DEBUG_UART_RX
#define UART2_TX_PIN        P10_1
#define UART2_RX_PIN        P10_0

#define A0 0 // ADC P10.1
#define A1 1 // ADC P10.0

#define LED1 12               // Additional LED1
#define LED_BUILTIN LED1      // Standard Arduino LED: Uses LED1
#define LED2 13
#define BUTTON1 14            // Additional BUTTON1
#define USER_BUTTON BUTTON1   // Standard Arduino USER_BUTTON: Uses BUTTON1

//****************************************************************************

#ifdef ARDUINO_GPIO

#ifdef __cplusplus
extern "C" {
#endif

// Mapping of digital pins and comments
const cyhal_gpio_t mapping_gpio_pin[] = {
    /* 0  */ CYHAL_GET_GPIO(CYHAL_PORT_9, 0),  // SPI-MOSI                       P9.0 
    /* 1  */ CYHAL_GET_GPIO(CYHAL_PORT_9, 1),  // SPI-MISO                       P9.1
    /* 2  */ CYHAL_GET_GPIO(CYHAL_PORT_9, 2),  // SPI-SCLK                       P9.2
    /* 3  */ CYHAL_GET_GPIO(CYHAL_PORT_9, 3),  // IO_0                           P9.3
    /* 4  */ CYHAL_GET_GPIO(CYHAL_PORT_9, 4),  // IO_1                           P9.4
    /* 5  */ CYHAL_GET_GPIO(CYHAL_PORT_9, 5),  // IO_2 / PWM1                    P9.5 
    /* 6  */ CYHAL_GET_GPIO(CYHAL_PORT_9, 6),  // IO_3 / PWM2                    P9.6
    /* 7  */ CYHAL_GET_GPIO(CYHAL_PORT_9, 7),  // IO_4                           P9.7

    /* 8  */ CYHAL_GET_GPIO(CYHAL_PORT_0, 2),  // I2C-SCL                        P0.2
    /* 9  */ CYHAL_GET_GPIO(CYHAL_PORT_0, 3),  // I2C-SDA                        P0.3
    /* 10 */ CYHAL_GET_GPIO(CYHAL_PORT_10, 1), // A1 / UART_TX                   P10.1
    /* 11 */ CYHAL_GET_GPIO(CYHAL_PORT_10, 0), // A0 / UART_RX                   P10.0

    // on board LEDs and USER BUTTON

    /* 12  */ CYHAL_GET_GPIO(CYHAL_PORT_5, 3),  // LED1                          P8.2
    /* 13  */ CYHAL_GET_GPIO(CYHAL_PORT_5, 4),  // LED2                          P8.1
    /* 14  */ CYHAL_GET_GPIO(CYHAL_PORT_5, 2),  // USER BUTTON                   P8.0 

    // Additional pins for expansion IO connector - J15 starting here

    /* 15  */ CYHAL_GET_GPIO(CYHAL_PORT_13, 0), // SDHC_DATA00 / SPI-MOSI / UART_RX / I2C-SCL     P13.0
    /* 16  */ CYHAL_GET_GPIO(CYHAL_PORT_13, 1), // SDHC_DATA01 / SPI-MISO / UART_TX / I2C-SDA     P13.1
    /* 17  */ CYHAL_GET_GPIO(CYHAL_PORT_13, 2), // SDHC_DATA02 / SPI-SCLK / IO / PWM              P13.2
    /* 18  */ CYHAL_GET_GPIO(CYHAL_PORT_13, 3), // SDHC_DATA03 / IO / PWM                         P13.3
    /* 19  */ CYHAL_GET_GPIO(CYHAL_PORT_13, 4), // SDHC_DATA10 / UART_RX / I2C-SCL                P13.4
    /* 20  */ CYHAL_GET_GPIO(CYHAL_PORT_13, 5), // SDHC_DATA11 / UART_TX / I2C-SDA                P13.5
    /* 21  */ CYHAL_GET_GPIO(CYHAL_PORT_13, 6), // SDHC_DATA12 / IO / PWM                         P13.6
    /* 22  */ CYHAL_GET_GPIO(CYHAL_PORT_13, 7), // SDHC_DATA13 / IO / PWM                         P13.7
    /* 23  */ CYHAL_GET_GPIO(CYHAL_PORT_8, 2),  // SPI-SCLK / IO /PWM                             P8.2
    /* 24  */ CYHAL_GET_GPIO(CYHAL_PORT_8, 1),  // SPI-MISO / UART_TX / I2C-SDA / IO / PWM        P8.1
    /* 25  */ CYHAL_GET_GPIO(CYHAL_PORT_8, 0),  // SPI-MOSI / UART_RX / I2C-SCL / IO / PWM        P8.0   
    /* 26  */ CYHAL_GET_GPIO(CYHAL_PORT_8, 3),  // IO / PWM                                       P8.3 
    /* 27  */ CYHAL_GET_GPIO(CYHAL_PORT_8, 4),  // UART_RX / I2C-SCL / IO /PWM                    P8.4 
    /* 28  */ CYHAL_GET_GPIO(CYHAL_PORT_8, 5),  // UART_TX / I2C-SDA / IO /PWM                    P8.5 
    /* 29  */ CYHAL_GET_GPIO(CYHAL_PORT_12, 4), // SDHC_CMD / IO /PWM                             P12.4 
    /* 30  */ CYHAL_GET_GPIO(CYHAL_PORT_12, 5), // SDHC_CLK / IO /PWM                             P12.5
};

const uint8_t GPIO_PIN_COUNT = (sizeof(mapping_gpio_pin) / sizeof(mapping_gpio_pin[0])) - 1;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ARDUINO_GPIO*/

#endif /* PINS_ARDUINO_H_ */
