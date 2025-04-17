#include "Arduino.h"
#include "Uart.h"
#include "cyhal_gpio.h"

Uart::Uart(pin_size_t tx, pin_size_t rx, pin_size_t cts, pin_size_t rts) : tx_pin(tx), rx_pin(rx), cts_pin(cts), rts_pin(rts) {

}

void Uart::begin(unsigned long baud) {
    begin(baud, SERIAL_8N1);
}

void Uart::begin(unsigned long baud, uint16_t config) {
    switch (config)
    {
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


    /* If the user does not define them (or pass NC), they are not connected. No Arduino GPIO mapping */
    cyhal_gpio_t cy_cts_pin = (cts_pin == NC) ? NC : mapping_gpio_pin[cts_pin];
    cyhal_gpio_t cy_rts_pin = (rts_pin == NC) ? NC : mapping_gpio_pin[rts_pin];

    /**
     * TODO: Error handling of cyhal return is not implemented.
     */
    cyhal_uart_init(&uart_obj, mapping_gpio_pin[tx_pin], mapping_gpio_pin[rx_pin], cy_cts_pin, cy_rts_pin, NULL, &uart_config);
    cyhal_uart_set_baud(&uart_obj, baud, &actualbaud);

    cyhal_uart_register_callback(&uart_obj, Uart::uart_event_handler, this);
    cyhal_uart_enable_event(&uart_obj, CYHAL_UART_IRQ_RX_NOT_EMPTY, 7, true);
}

int Uart::available(void) {
    return rx_buffer.available();
}

int Uart::availableForWrite() {
    return cyhal_uart_writable(&uart_obj);
}

void Uart::end() {
    cyhal_uart_clear(&uart_obj);
    cyhal_uart_free(&uart_obj);
    rx_buffer.clear();
}

void Uart::flush() {
    unsigned long time_start_ms = millis();
    unsigned long timeout_ms = 1000;
    while (cyhal_uart_is_tx_active(&uart_obj)) {
        if (millis() - time_start_ms >= timeout_ms) {
            // Timeout occurred
            break;
        }
    }
}

int Uart::peek(void) {
    if (rx_buffer.available() == 0) {
        return -1;       // Buffer is empty
    }
    return rx_buffer.peek();
}

int Uart::read(void) {
    if (rx_buffer.available() == 0) {
        return -1;       // Buffer is empty
    }
    return rx_buffer.read_char();
}

size_t Uart::write(uint8_t c) {
    return write((const uint8_t *)&c, 1);
}

size_t Uart::write(const uint8_t *buffer, size_t size) {
    size_t left_to_write = size;
    unsigned long time_start_ms = millis();
    uint32_t constexpr timeout_ms = 500;
    /* Flow control is implemented by checking the available capacity of the
    UART for writing. Iterate in case of the amount of requested bytes exceed the
    available. To avoid and infinite loop a timeout of is implemented.
    For very longer transaction the application needs to to implement a similar
     */
    do {
        uint32_t num_bytes_writable = cyhal_uart_writable(&uart_obj);
        size_t bytes_to_write = left_to_write > num_bytes_writable ? num_bytes_writable : left_to_write;
        /* Trying to write 0 size will throw an exception. */
        if (bytes_to_write > 0) {
            cy_rslt_t result = cyhal_uart_write(&uart_obj, (void *)buffer, &bytes_to_write);
            if (result != CY_RSLT_SUCCESS) {
                break;
            }
            left_to_write -= bytes_to_write;
            buffer += bytes_to_write;
        }
    } while (left_to_write > 0 && (millis() - time_start_ms) < timeout_ms);

    return size - left_to_write;
}

void Uart::uart_event_handler(void *handler_arg, cyhal_uart_event_t event) {
    Uart *uart = static_cast < Uart * > (handler_arg);
    uart->IrqHandler();
}

void Uart::IrqHandler() {
    uint8_t c;
    size_t size = 1;
    while (cyhal_uart_readable(&uart_obj) > 0) {
        cyhal_uart_read(&uart_obj, &c, &size);
        if (rx_buffer.availableForStore() > 0) {
            rx_buffer.store_char(c);
        }
    }
}

#if SERIAL_HOWMANY > 0
Uart _UART1_(UART1_TX_PIN, UART1_RX_PIN, UART1_CTS_PIN, UART1_RTS_PIN);
#endif

#if SERIAL_HOWMANY > 1
Uart _UART2_(UART2_TX_PIN, UART2_RX_PIN, UART2_CTS_PIN, UART2_RTS_PIN);
#endif
