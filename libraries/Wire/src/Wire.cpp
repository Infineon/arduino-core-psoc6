extern "C" {
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
}

      #include "Wire.h"

      #define Wire_assert(cy_ret) if (cy_ret != CY_RSLT_SUCCESS) { \
                  return; \
}


TwoWire::TwoWire(cyhal_gpio_t sda, cyhal_gpio_t scl) : sda_pin(sda), scl_pin(scl) {
}

void TwoWire::_begin() {

    if (is_master) {
        i2c_config = {
            .is_slave = CYHAL_I2C_MODE_MASTER,
            .address = 0,
            .frequencyhal_hz = I2C_DEFAULT_FREQ
        };
    } else {
        i2c_config = {
            .is_slave = CYHAL_I2C_MODE_SLAVE,
            .address = slave_address,
            .frequencyhal_hz = I2C_DEFAULT_FREQ
        };
    }

    w_status = cyhal_i2c_init(&i2c_obj, sda_pin, scl_pin, NULL);
    Wire_assert(w_status);
    w_status = cyhal_i2c_configure(&i2c_obj, &i2c_config);
    Wire_assert(w_status);

    if (!is_master) {
        // Configure the read and write buffers for the I2C slave
        w_status = cyhal_i2c_slave_config_read_buffer(&i2c_obj,  temp_tx_buff, BUFFER_LENGTH);
        Wire_assert(w_status);
        w_status = cyhal_i2c_slave_config_write_buffer(&i2c_obj, temp_rx_buff, BUFFER_LENGTH);
        Wire_assert(w_status);
        cyhal_i2c_register_callback(&i2c_obj, i2c_event_handler, this);
    }
}

void TwoWire::begin() {
    is_master = true;
    _begin();
}

void TwoWire::begin(uint8_t address) {
    is_master = false;
    slave_address = address;
    _begin();
}

void TwoWire::end() {
    cyhal_i2c_free(&i2c_obj);
}

void TwoWire::setClock(uint32_t freq) {
    if (is_master) {
        i2c_config = {
            .is_slave = false,
            .address = 0,
            .frequencyhal_hz = freq
        };
    } else {
        i2c_config = {
            .is_slave = true,
            .address = slave_address,
            .frequencyhal_hz = freq
        };
    }
    cyhal_i2c_configure(&i2c_obj, &i2c_config);
}

void TwoWire::beginTransmission(uint8_t address) {
    slave_address = address;
    txBuffer.clear();
}

uint8_t TwoWire::endTransmission(bool sendStop) {
    cy_rslt_t result;
    uint16_t bytes_rcvd_request = txBuffer.available();

    for (uint16_t i = 0; i < bytes_rcvd_request; i++) {
        temp_tx_buff[i] = txBuffer.read_char();
    }

    result = cyhal_i2c_master_write(&i2c_obj, slave_address, temp_tx_buff, bytes_rcvd_request, timeout, sendStop);
    // Handle specific error codes
    switch (result) {
        case I2C_SUCCESS:
            return 0;           // Success
        case I2C_NO_DEVICE_ATTACHED_PULL_UP:
            return 2;           // NACK on transmit of address . Error: No device attached to SDA/SCL, but they are pulled-up
        case I2C_NO_DEVICE_ATTACHED_NO_PULL_UP:
            return 2;           // NACK on transmit of address. Error: No device attached to SDA/SCL, and they are not pulled-up
        case I2C_TIMEOUT:
            return 5;           // Timeout
        default:
            return 4;           // Other error
    }
}

uint8_t TwoWire::endTransmission(void) {
    return endTransmission(true);
}

size_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit) {
    rxBuffer.clear();
    if (quantity > BUFFER_LENGTH) {
        quantity = BUFFER_LENGTH;
    }

    cy_rslt_t result = cyhal_i2c_master_read(&i2c_obj, address, temp_rx_buff, quantity, timeout, stopBit);
    if (result == CY_RSLT_SUCCESS) {
        for (uint32_t i = 0; i < quantity; i++) {
            rxBuffer.store_char(temp_rx_buff[i]);
        }
        return quantity;       // Return number of bytes read
    } else {
        return 0;       // Return 0 on failure of read operation
    }
}

size_t TwoWire::requestFrom(uint8_t address, size_t len) {
    return requestFrom(address, len, true);
}

size_t TwoWire::write(uint8_t data) {
    if (txBuffer.isFull()) {
        return 0;       // Buffer is full
    }

    txBuffer.store_char(data);
    return 1;
}

int TwoWire::available(void) {
    return rxBuffer.available();
}

int TwoWire::read(void) {
    if (rxBuffer.available() == 0) {
        return -1;       // Buffer is empty
    }
    return rxBuffer.read_char();
}

int TwoWire::peek(void) {
    if (rxBuffer.available() == 0) {
        return -1;       // Buffer is empty
    }
    return rxBuffer.peek();
}

void TwoWire::onReceive(void (*function)(int)) {
    user_onReceive = function;
    cyhal_i2c_enable_event(&i2c_obj, CYHAL_I2C_SLAVE_WR_CMPLT_EVENT, 7, true);
}

void TwoWire::onRequest(void (*function)(void)) {
    user_onRequest = function;
    cyhal_i2c_enable_event(&i2c_obj, CYHAL_I2C_SLAVE_READ_EVENT, 7, true);
}

void TwoWire::i2c_event_handler(void *callback_arg, cyhal_i2c_event_t event) {
    // Call the non-static member function
    TwoWire *wire = static_cast < TwoWire * > (callback_arg);
    wire->i2c_event_handler_member(event);
}

void TwoWire::i2c_event_handler_member(cyhal_i2c_event_t event) {
    if (event == CYHAL_I2C_SLAVE_WR_CMPLT_EVENT) {        // master wants to write data
        onReceiveService();
    } else if (event == CYHAL_I2C_SLAVE_READ_EVENT) {       // master wants to read data
        onRequestService();
    } else {
        return;
    }
}

// This function is called when the master wants to write data to the slave
void TwoWire::onReceiveService(void) {

    uint16_t numBytes = cyhal_i2c_slave_readable(&i2c_obj);

    // Ensure numBytes does not exceed BUFFER_LENGTH
    numBytes = (numBytes > BUFFER_LENGTH) ? BUFFER_LENGTH : numBytes;

    // Manually update rxBuffer to reflect the number of bytes received
    for (uint8_t i = 0; i < numBytes; i++) {
        if (i < rxBuffer.availableForStore()) {
            rxBuffer.store_char(temp_rx_buff[i]);
        } else {
            break;
        }
    }
    // Alert user program if registered by user
    if (user_onReceive) {
        user_onReceive(numBytes);
    }

    // Reconfigure the write buffer after the transaction
    w_status = cyhal_i2c_slave_config_write_buffer(&i2c_obj, temp_rx_buff, BUFFER_LENGTH);
    Wire_assert(w_status);
}

// This function is called when the master wants to read data from the slave
void TwoWire::onRequestService(void) {

    // Alert user program if registered
    if (user_onRequest) {
        user_onRequest();
    }

    uint16_t bytesToSend = cyhal_i2c_slave_writable(&i2c_obj);

    // Check if the buffer is empty
    uint16_t bytesToSend_available = txBuffer.available();
    bytesToSend = (bytesToSend > bytesToSend_available) ? bytesToSend_available : bytesToSend;

    // Copy the data from the txBuffer to the temp_tx_buff array
    for (uint16_t i = 0; i < bytesToSend; i++) {
        temp_tx_buff[i] = txBuffer.read_char();
    }

    // Reconfigure the read buffer after the transaction
    w_status = cyhal_i2c_slave_config_read_buffer(&i2c_obj, temp_tx_buff, BUFFER_LENGTH);
    Wire_assert(w_status);
}

      #if I2C_HOWMANY > 0
TwoWire Wire(I2C1_SDA_PIN, I2C1_SCL_PIN);
      #endif

      #if I2C_HOWMANY > 1
TwoWire Wire1(I2C2_SDA_PIN, I2C2_SCL_PIN);
      #endif
