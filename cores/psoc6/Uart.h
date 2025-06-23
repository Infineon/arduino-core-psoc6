#pragma once

#include "api/HardwareSerial.h"
#include "api/RingBuffer.h"
#include "cyhal_uart.h"

typedef enum {
    UART_ERROR_NONE = 0,
    UART_ERROR_INIT_FAILED = -1,
    UART_ERROR_SET_BAUD_FAILED = -2,
} uart_error_t;

class Uart: public arduino::HardwareSerial
{
public:

    Uart(pin_size_t tx, pin_size_t rx, pin_size_t cts = NC, pin_size_t rts = NC);
    ~Uart();
    void begin(unsigned long baud);
    void begin(unsigned long baud, uint16_t config);
    void end();
    int available(void);
    virtual int availableForWrite();
    int peek(void);
    int read(void);
    void flush(void);
    virtual size_t write(uint8_t c);
    virtual size_t write(const uint8_t *buffer, size_t size);

    using Print::write;
    operator bool();

    uart_error_t getLastError();

    static void uart_event_handler(void *handler_arg, cyhal_uart_event_t event);


private:

    pin_size_t tx_pin;
    pin_size_t rx_pin;
    pin_size_t cts_pin;
    pin_size_t rts_pin;
    cyhal_uart_t uart_obj = {};
    cyhal_uart_cfg_t uart_config = {};
    uint32_t actualbaud = 0;
    bool serial_ready = false;
    uart_error_t last_error = UART_ERROR_NONE;

    static constexpr size_t BUFFER_LENGTH = 512;
    arduino::RingBufferN < BUFFER_LENGTH > rx_buffer = {};

    void IrqHandler();
};

#if (SERIAL_HOWMANY > 0)
extern Uart _UART1_;
#endif

#if (SERIAL_HOWMANY > 1)
extern Uart _UART2_;
#endif
