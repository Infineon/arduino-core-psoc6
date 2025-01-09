#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "cybsp.h"
/****** UART CORE DEFINES ******/

#define SERIAL_HOWMANY		2
#define UART1_TX_PIN        CYBSP_DEBUG_UART_TX
#define UART1_RX_PIN        CYBSP_DEBUG_UART_RX
#define UART2_TX_PIN        P10_1
#define UART2_RX_PIN        P10_0

#endif