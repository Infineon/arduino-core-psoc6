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
#define UART1_TX_PIN      32  // UART_TX P5_1
#define UART1_RX_PIN      31  // UART_RX P5_0
#define UART1_CTS_PIN     NC  // NOT CONNECTED
#define UART1_RTS_PIN     NC  // NOT CONNECTED
#define UART2_TX_PIN      10  // UART_TX P10_1
#define UART2_RX_PIN      11  // UART_RX P10_0
#define UART2_CTS_PIN     NC  // NOT CONNECTED
#define UART2_RTS_PIN     NC  // NOT CONNECTED

#define I2C_HOWMANY     2
#define I2C1_SDA_PIN    9 // I2C-SDA P0_3
#define I2C1_SCL_PIN    8 // I2C-SCL P0_2
#define I2C2_SDA_PIN    1 // I2C-SDA P9_1
#define I2C2_SCL_PIN    0 // I2C-SCL P9_0

static const uint8_t SDA = I2C1_SDA_PIN;
static const uint8_t SCL = I2C1_SCL_PIN;

#define SPI_HOWMANY             1
#define PIN_SPI_MOSI            0 // SPI-MOSI P9_0
#define PIN_SPI_MISO            1 // SPI-MISO P9_1
#define PIN_SPI_SCK             2 // SPI-SCLK P9_2
#define PIN_SPI_SS              3 // IO_0     P9_3

static const uint8_t SS = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI; 
static const uint8_t MISO = PIN_SPI_MISO; 
static const uint8_t SCK = PIN_SPI_SCK;

#define ADC_HOWMANY             2
#define ADC_RESOLUTION          12 // ADC resolution in bits, but the observed range is from 0-2^11. 
#define PIN_A0                  11 // ADC P10.0
#define PIN_A1                  10 // ADC P10.1

static const uint8_t A0  = PIN_A0;
static const uint8_t A1  = PIN_A1;

#define PWM_HOWMANY             14 // Number of output pins that can be PWM channels

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
    /* 0   */ P9_0, // SPI-MOSI / PWM         
    /* 1   */ P9_1, // SPI-MISO / PWM        
    /* 2   */ P9_2, // SPI-SCLK / PWM        
    /* 3   */ P9_3, // IO_0 / PWM            
    /* 4   */ P9_4, // IO_1 / PWM            
    /* 5   */ P9_5, // IO_2 / PWM     
    /* 6   */ P9_6, // IO_3 / PWM     
    /* 7   */ P9_7, // IO_4 / PWM            
		   
    /* 8   */ P0_2, // I2C-SCL / PWM         
    /* 9   */ P0_3, // I2C-SDA / PWM         
    /* 10  */ P10_1,// A1 / UART_TX / PWM    
    /* 11  */ P10_0,// A0 / UART_RX / PWM    

    // on board LEDs and USER BUTTON

    /* 12  */ P5_3, // LED1            
    /* 13  */ P5_4, // LED2            
    /* 14  */ P5_2, // USER BUTTON     

    // Additional pins for expansion IO connector - J15 starting here

    /* 15  */ P13_0,// SDHC_DATA00 / SPI-MOSI / UART_RX / I2C-SCL / PWM     
    /* 16  */ P13_1,// SDHC_DATA01 / SPI-MISO / UART_TX / I2C-SDA / PWM     
    /* 17  */ P13_2,// SDHC_DATA02 / SPI-SCLK / IO / PWM             
    /* 18  */ P13_3,// SDHC_DATA03 / IO / PWM                         
    /* 19  */ P13_4,// SDHC_DATA10 / UART_RX / I2C-SCL / PWM                
    /* 20  */ P13_5,// SDHC_DATA11 / UART_TX / I2C-SDA / PWM                
    /* 21  */ P13_6,// SDHC_DATA12 / IO / PWM                         
    /* 22  */ P13_7,// SDHC_DATA13 / IO / PWM                         
    /* 23  */ P8_2,// SPI-SCLK / IO / PWM                              
    /* 24  */ P8_1,// SPI-MISO / UART_TX / I2C-SDA / IO / PWM         
    /* 25  */ P8_0,// SPI-MOSI / UART_RX / I2C-SCL / IO / PWM         
    /* 26  */ P8_3,// IO / PWM                                        
    /* 27  */ P8_4,// UART_RX / I2C-SCL / IO / PWM                     
    /* 28  */ P8_5,// UART_TX / I2C-SDA / IO / PWM                     
    /* 29  */ P12_4,// SDHC_CMD / IO / PWM                             
    /* 30  */ P12_5,// SDHC_CLK / IO / PWM

    // Additional pins not available in external connector

    // Debugger Serial UART pins 
    
    /* 31  */ P5_0, // DEBUG_UART_RX 
    /* 32  */ P5_1, // DEBUG_UART_TX 

    // IMU Interrupt pins

    /* 33  */ P0_4, // IMU_INT2
    /* 34  */ P1_5, // IMU_INT1
  
    // Magnetometer interrupt pin

    /* 35  */ P1_0, // MAG_INT

    // Digital pressure sensor interrupt pin

    /* 36  */ P1_4, // PSEN_INT
  
    // PDM microphone pins
    
    /* 37  */ P10_4, // PDM_MIC_CLK
    /* 38  */ P10_5, // PDM_MIC_DATA

    // Radar sensor pins
    
    /* 39  */ P11_0, // RSPI_IRQ
    /* 40  */ P11_1, // RXRES_L
    /* 41  */ P12_0, // RSPI_MOSI
    /* 42  */ P12_1, // RSPI_MISO
    /* 43  */ P12_2, // RSPI_SCLK
    /* 44  */ P12_3, // RSPI_CS

    // QSPI flash pins

    /* 45  */ P11_2, // QSPI_SEL_L
    /* 46  */ P11_3, // QSPI_DATA3
    /* 47  */ P11_4, // QSPI_DATA2
    /* 48  */ P11_5, // QSPI_DATA1
    /* 49  */ P11_6, // QSPI_DATA0
    /* 50  */ P11_7, // QSPI_CLK
    
    // Bluetooth module power mode pins

    /* 51  */ P3_4, // BT_REG_ON
    /* 52  */ P3_5, // BT_DEV_WAKE
    /* 53  */ P4_0, // BT_HOST_WAKE
    /* 54  */ P4_1, // WL_HOST_WAKE

    // Battery charger module pins

    /* 55  */ P5_5, // LBCPP_PG
    /* 56  */ P5_6, // LBCPP_CHG
    /* 57  */ P5_7, // LBCPP_CE
    /* 58  */ P6_0, // LBCPP_EN2
    /* 59  */ P6_1, // LBCPP_EN1
    /* 60  */ P6_3, // LBCPP_ITERM

    // USB pins

    /* 61  */ P6_2, // USB_S
    /* 62  */ USBDP, // USB_DP
    /* 63  */ USBDM, // USB_DM
};

const uint8_t GPIO_PIN_COUNT = (sizeof(mapping_gpio_pin) / sizeof(mapping_gpio_pin[0])) - 1;

bool gpio_initialized[GPIO_PIN_COUNT] = {false};
bool currentPinValue[GPIO_PIN_COUNT] = {false};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ARDUINO_GPIO*/

#endif /* PINS_ARDUINO_H_ */
