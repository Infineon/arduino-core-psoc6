#include "Uart.hpp"

#if SERIAL_HOWMANY > 1
Uart _UART2_(UART2_TX_PIN, UART2_RX_PIN, NC, NC);
#endif
