#pragma once

#include "Arduino.h"
#include "HardwareSerial.h"
#include "cyhal_uart.h"
#include "cyhal_gpio.h"

enum { MAX_UARTS = 10 };

class Uart : public arduino::HardwareSerial {
public:
    static Uart *g_uarts[MAX_UARTS];
    Uart(cyhal_gpio_t tx, cyhal_gpio_t rx, cyhal_gpio_t cts, cyhal_gpio_t rts);
    void begin(unsigned long baud) override;
    void begin(unsigned long baud, uint16_t config) override;
    void end() override;
    int available() override;
    int availableForWrite() override;
    int peek() override;
    int read() override;
    void flush() override;
    size_t write(uint8_t c) override;
    virtual size_t write(const uint8_t *buffer, size_t size);

    size_t write(unsigned long n) {
        return write(static_cast<uint8_t>(n));
    }

    size_t write(long n) {
        return write(static_cast<uint8_t>(n));
    }

    size_t write(unsigned int n) {
        return write(static_cast<uint8_t>(n));
    }

    size_t write(int n) {
        return write(static_cast<uint8_t>(n));
    }

    using Print::write;

    operator bool() override {
        return true;
    }

    static void uart_event_handler(void *handler_arg, cyhal_uart_event_t event);

private:
    cyhal_gpio_t tx_pin{};
    cyhal_gpio_t rx_pin{};
    cyhal_gpio_t cts_pin{};
    cyhal_gpio_t rts_pin{};
    cyhal_uart_t uart_obj{};
    cyhal_uart_cfg_t uart_config{};
    uint32_t actualbaud{};
    cy_rslt_t result{};
    uint32_t extractStopBit(uint16_t config);
    uint32_t extractParity(uint16_t config);
    uint32_t extractDataBits(uint16_t config);

    // Software buffer
    static const int bufferSize = 128;
    uint8_t buffer[bufferSize]{};
    volatile int bufferHead{};
    volatile int bufferTail{};
    void IrqHandler();
};

extern Uart _UART1_;
extern Uart _UART2_;
