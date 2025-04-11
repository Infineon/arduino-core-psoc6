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
#define UART1_TX_PIN      CYBSP_DEBUG_UART_TX
#define UART1_RX_PIN      CYBSP_DEBUG_UART_RX
#define UART2_TX_PIN      P10_1
#define UART2_RX_PIN      P10_0

#define I2C_HOWMANY    2
#define I2C1_SDA_PIN    CYBSP_I2C_SDA
#define I2C1_SCL_PIN    CYBSP_I2C_SCL
#define I2C2_SDA_PIN    P9_1
#define I2C2_SCL_PIN    P9_0

#define SPI_HOWMANY             1
#define PIN_SPI_SS              0 // SPI-MOSI P9_0
#define PIN_SPI_MOSI            1 // SPI-MISO P9_1
#define PIN_SPI_MISO            2 // SPI-SCLK P9_2
#define PIN_SPI_SCK             3 // IO_0     P9_3

static const uint8_t SS = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI; 
static const uint8_t MISO = PIN_SPI_MISO; 
static const uint8_t SCK = PIN_SPI_SCK;

#define A0                      0 // ADC P10.1
#define A1                      1 // ADC P10.0

#define LED1                    12 // Additional LED1
#define LED_BUILTIN             LED1 // Standard Arduino LED: Uses LED1
#define LED2                    13 // Additional LED2
#define BUTTON1                 14 // Additional BUTTON1
#define USER_BUTTON             BUTTON1 // Standard Arduino USER_BUTTON: Uses BUTTON1

//****************************************************************************

#ifdef ARDUINO_GPIO

#ifdef __cplusplus
extern "C" {
#endif

// Mapping of digital pins and comments
const cyhal_gpio_t mapping_gpio_pin[] = {
    /* 0   */ P9_0, // SPI-MOSI         
    /* 1   */ P9_1, // SPI-MISO        
    /* 2   */ P9_2, // SPI-SCLK        
    /* 3   */ P9_3, // IO_0            
    /* 4   */ P9_4, // IO_1            
    /* 5   */ P9_5, // IO_2 / PWM1     
    /* 6   */ P9_6, // IO_3 / PWM2     
    /* 7   */ P9_7, // IO_4            
		   
    /* 8   */ P0_2, // I2C-SCL         
    /* 9   */ P0_3, // I2C-SDA         
    /* 10  */ P10_1,// A1 / UART_TX    
    /* 11  */ P10_0,// A0 / UART_RX    

    // on board LEDs and USER BUTTON

    /* 12  */ P5_3, // LED1            
    /* 13  */ P5_4, // LED2            
    /* 14  */ P5_2, // USER BUTTON     

    // Additional pins for expansion IO connector - J15 starting here

    /* 15  */ P13_0,// SDHC_DATA00 / SPI-MOSI / UART_RX / I2C-SCL     
    /* 16  */ P13_1,// SDHC_DATA01 / SPI-MISO / UART_TX / I2C-SDA     
    /* 17  */ P13_2,// SDHC_DATA02 / SPI-SCLK / IO / PWM             
    /* 18  */ P13_3,// SDHC_DATA03 / IO / PWM                         
    /* 19  */ P13_4,// SDHC_DATA10 / UART_RX / I2C-SCL                
    /* 20  */ P13_5,// SDHC_DATA11 / UART_TX / I2C-SDA                
    /* 21  */ P13_6,// SDHC_DATA12 / IO / PWM                         
    /* 22  */ P13_7,// SDHC_DATA13 / IO / PWM                         
    /* 23  */ P8_2,// SPI-SCLK / IO /PWM                              
    /* 24  */ P8_1,// SPI-MISO / UART_TX / I2C-SDA / IO / PWM         
    /* 25  */ P8_0,// SPI-MOSI / UART_RX / I2C-SCL / IO / PWM         
    /* 26  */ P8_3,// IO / PWM                                        
    /* 27  */ P8_4,// UART_RX / I2C-SCL / IO /PWM                     
    /* 28  */ P8_5,// UART_TX / I2C-SDA / IO /PWM                     
    /* 29  */ P12_4,// SDHC_CMD / IO /PWM                             
    /* 30  */ P12_5,// SDHC_CLK / IO /PWM                             
};

const uint8_t GPIO_PIN_COUNT = (sizeof(mapping_gpio_pin) / sizeof(mapping_gpio_pin[0])) - 1;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ARDUINO_GPIO*/

#endif /* PINS_ARDUINO_H_ */
