/**
 * @file test_config.h
 * @brief Configuration file for board-specific test pin definitions.
 *
 * This header file contains the definitions of the pins used for testing
 * purposes on the specific board. These pins are configured as output and 
 * input pins for various test scenarios.
 *
 */

#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#include <stdint.h>
#include <SPI.h>

// Test Pin Definitions

#define         TEST_PIN_DIGITAL_IO_OUTPUT      7       // IO_4
#define         TEST_PIN_DIGITAL_IO_INPUT       6       // IO_3

#define         TEST_PIN_PULSE                  5       // IO_2

#define         TEST_PIN_SYNC_IO                4       // IO_1

#define         TEST_PIN_SPI_SSEL               3       // IO_0

#define         TEST_PIN_ANALOG_IO_VREF         A0      // Pin connected to VREF
#define         TEST_PIN_ANALOG_IO_DIVIDER      A1      // Pin connected to voltage divider

#define         TEST_ADC_RESOLUTION             11      
#define         TEST_ADC_MAX_VALUE              2048    // 11-bit resolution

// Forward declarations for SPI instances
extern SPIClassPSOC SPI1;

#endif // TEST_CONFIG_H