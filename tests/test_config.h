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

// Test Pin Definitions
#define         TEST_DIGITALIO_OUTPUT   7       // IO_4
#define         TEST_DIGITALIO_INPUT    6       // IO_3

#define         TEST_SYNC_INPUT_OUTPUT  4       // IO_1

#define         TEST_SPI_SLAVE_SELECT   3       // IO_0

#define         TEST_ANALOG_IO_VREF     A0      // Pin connected to VREF
#define         TEST_ANALOG_IO_DIVIDER  A1      // Pin connected to voltage divider

#define         VDDA                    3.3f    // Assume VDDA is 3.3V
#define         RESOLUTION              2048    // 11-bit resolution

#endif // TEST_CONFIG_H