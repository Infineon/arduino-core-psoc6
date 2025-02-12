#pragma once

#include "Arduino.h"
#include "api/HardwareSerial.h"
#include "cyhal_gpio.h"
#include "cyhal_uart.h"

#define MAX_UARTS 10

class Uart : public arduino::HardwareSerial {
public:
  static Uart *g_uarts[MAX_UARTS];
  Uart(cyhal_gpio_t tx, cyhal_gpio_t rx, cyhal_gpio_t cts, cyhal_gpio_t rts);
  void begin(unsigned long baud);
  void begin(unsigned long baud, uint16_t config);
  void end();
  int available(void);
  int availableForWrite();
  int peek(void);
  int read(void);
  void flush(void);
  virtual size_t write(uint8_t c);
  virtual size_t write(const uint8_t *buffer, size_t size);
  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }
  using Print::write; // pull in write(str) and write(buf, size) from Print
  operator bool() { return true; }
  static void uart_event_handler(void *handler_arg, cyhal_uart_event_t event);

private:
  cyhal_gpio_t tx_pin;
  cyhal_gpio_t rx_pin;
  cyhal_gpio_t cts_pin;
  cyhal_gpio_t rts_pin;
  cyhal_uart_t uart_obj;
  cyhal_uart_cfg_t uart_config;
  uint32_t actualbaud;
  cy_rslt_t result;
  uint32_t extractStopBit(uint16_t config);
  uint32_t extractParity(uint16_t config);
  uint32_t extractDataBits(uint16_t config);

  // Software buffer
  static const int bufferSize = 128;
  uint8_t buffer[bufferSize];
  volatile int bufferHead;
  volatile int bufferTail;
  void IrqHandler();
};

extern Uart _UART1_;
extern Uart _UART2_;
