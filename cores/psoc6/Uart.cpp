#include "Uart.h"
#include "cybsp.h"

#ifdef Serial
    #undef Serial
#endif

Uart *Uart::g_uarts[MAX_UARTS] = {nullptr};

Uart::Uart(cyhal_gpio_t tx, cyhal_gpio_t rx, cyhal_gpio_t cts, cyhal_gpio_t rts)
    : tx_pin(tx),
      rx_pin(rx),
      cts_pin(cts),
      rts_pin(rts) {
}

void Uart::begin(unsigned long baud) {
    begin(baud, SERIAL_8N1);
}

void Uart::begin(unsigned long baud, uint16_t config) {
    switch (config) {
        case SERIAL_8N1:
            uart_config.data_bits = 8;
            uart_config.stop_bits = 1;
            uart_config.parity = CYHAL_UART_PARITY_NONE;
            break;
        case SERIAL_8N2:
            uart_config.data_bits = 8;
            uart_config.stop_bits = 2;
            uart_config.parity = CYHAL_UART_PARITY_NONE;
            break;
        case SERIAL_8E1:
            uart_config.data_bits = 8;
            uart_config.stop_bits = 1;
            uart_config.parity = CYHAL_UART_PARITY_EVEN;
            break;
        case SERIAL_8E2:
            uart_config.data_bits = 8;
            uart_config.stop_bits = 2;
            uart_config.parity = CYHAL_UART_PARITY_EVEN;
            break;
        case SERIAL_8O1:
            uart_config.data_bits = 8;
            uart_config.stop_bits = 1;
            uart_config.parity = CYHAL_UART_PARITY_ODD;
            break;
        case SERIAL_8O2:
            uart_config.data_bits = 8;
            uart_config.stop_bits = 2;
            uart_config.parity = CYHAL_UART_PARITY_ODD;
            break;
        default:
            uart_config.data_bits = 8;
            uart_config.stop_bits = 1;
            uart_config.parity = CYHAL_UART_PARITY_NONE;
            break;
    }

    // Initialize the UART Block
    cyhal_uart_init(&uart_obj, tx_pin, rx_pin, cts_pin, rts_pin, NULL, &uart_config);
    // set the baud rate
    cyhal_uart_set_baud(&uart_obj, baud, &actualbaud);

    cyhal_uart_register_callback(&uart_obj, Uart::uart_event_handler, this);
    cyhal_uart_enable_event(&uart_obj, CYHAL_UART_IRQ_RX_NOT_EMPTY, 7, true);
}

int Uart::available() {
    return (bufferHead - bufferTail + bufferSize) % bufferSize;
}

int Uart::availableForWrite() {
    uint32_t ret = cyhal_uart_writable(&uart_obj);
    return ret;
}

void Uart::end() {
    // Clear the UART buffer
    cyhal_uart_clear(&uart_obj);
    cyhal_uart_free(&uart_obj);
    bufferHead = bufferTail;
}

void Uart::flush() {
    unsigned long startMillis = millis();
    unsigned long timeout = 1000; // Timeout period in milliseconds
    while (cyhal_uart_is_tx_active(&uart_obj)) {
        if (millis() - startMillis >= timeout) {
            // Timeout occurred
            break;
        }
    }
}

int Uart::peek() {
    if (bufferHead == bufferTail) {
        return -1; // Buffer is empty
    }
    return buffer[bufferTail]; // Return the next byte without removing it from the buffer
}

int Uart::read() {
    noInterrupts();
    if (bufferHead == bufferTail) {
        interrupts();
        return -1; // Buffer is empty
    }
    uint8_t c = buffer[bufferTail];
    bufferTail = (bufferTail + 1) % bufferSize;
    interrupts();
    return c;
}

size_t Uart::write(uint8_t c) {
    cy_rslt_t result = cyhal_uart_putc(&uart_obj, c);
    if (result != CY_RSLT_SUCCESS) {
        return 0;
    }
    return 1;
}

size_t Uart::write(const uint8_t *buffer, size_t size) {
    cy_rslt_t result = cyhal_uart_write(&uart_obj, (void *)buffer, &size);
    if (result != CY_RSLT_SUCCESS) {
        return 0;
    }
    return size;
}

void Uart::uart_event_handler(void *handler_arg, cyhal_uart_event_t event) {
    Uart *uart = static_cast<Uart *>(handler_arg);
    uart->IrqHandler();
}

void Uart::IrqHandler() {
    uint8_t c = 0;
    size_t size = 1;
    while (cyhal_uart_readable(&uart_obj) > 0) {
        cyhal_uart_read(&uart_obj, &c, &size);
        int nextHead = (bufferHead + 1) % bufferSize;
        if (nextHead != bufferTail) {
            buffer[bufferHead] = c;
            bufferHead = nextHead;
        } else {
            // Buffer overflow, discard the byte
        }
    }
}

// #endif
