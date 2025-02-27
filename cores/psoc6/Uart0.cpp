#include "Uart.h"

#if SERIAL_HOWMANY > 0
Uart _UART1_(UART1_TX_PIN, UART1_RX_PIN, NC, NC);
#endif
