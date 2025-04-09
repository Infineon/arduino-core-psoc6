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

// test defines
const uint8_t TEST_DIGITALIO_OUTPUT = 7;  // IO_4
const uint8_t TEST_DIGITALIO_INPUT = 6;   // IO_3


const uint8_t TEST_SYNC_INPUT_OUTPUT = 4;   // IO_1

const uint8_t TEST_SPI_SLAVE_SELECT = 3;   // IO_0

#endif // TEST_CONFIG_H