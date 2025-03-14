extern "C" {
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
}

#include "Wire.h"

#define Wire_assert(cy_ret)          \
    if (cy_ret != CY_RSLT_SUCCESS) { \
        return;                      \
    }

TwoWire *TwoWire::instances[I2C_HOWMANY] = {nullptr};
uint8_t TwoWire::bytesSent = 0;

TwoWire::TwoWire(cyhal_gpio_t sda, cyhal_gpio_t scl, uint8_t instance)
    : sda_pin(sda),
      scl_pin(scl),
      instance(instance) {
    if (instance < I2C_HOWMANY) {
        instances[instance] = this;
    }
}

void TwoWire::_begin() {
    rxBufferIndex = 0;
    rxBufferLength = 0;

    txBufferIndex = 0;
    txBufferLength = 0;

    if (is_master) {
        i2c_config = {
            .is_slave = CYHAL_I2C_MODE_MASTER, .address = 0, .frequencyhal_hz = I2C_DEFAULT_FREQ};
    } else {
        i2c_config = {.is_slave = CYHAL_I2C_MODE_SLAVE,
                      .address = slave_address,
                      .frequencyhal_hz = I2C_DEFAULT_FREQ};
    }

    w_status = cyhal_i2c_init(&i2c_obj, sda_pin, scl_pin, NULL);
    Wire_assert(w_status);
    w_status = cyhal_i2c_configure(&i2c_obj, &i2c_config);
    Wire_assert(w_status);

    if (!is_master) {
        // Configure the read and write buffers for the I2C slave
        w_status = cyhal_i2c_slave_config_read_buffer(&i2c_obj, txBuffer, BUFFER_LENGTH);
        Wire_assert(w_status);
        w_status = cyhal_i2c_slave_config_write_buffer(&i2c_obj, rxBuffer, BUFFER_LENGTH);
        Wire_assert(w_status);
    }

    cyhal_i2c_register_callback(&i2c_obj, i2c_event_handler, this);
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
    instances[instance] = nullptr; // Clear the instance
}

void TwoWire::setClock(uint32_t freq) {
    if (is_master) {
        i2c_config = {.is_slave = false, .address = 0, .frequencyhal_hz = freq};
    } else {
        i2c_config = {.is_slave = true, .address = slave_address, .frequencyhal_hz = freq};
    }
    cyhal_i2c_configure(&i2c_obj, &i2c_config);
}

void TwoWire::beginTransmission(uint8_t address) {
    txAddress = address;
    txBufferIndex = 0;
    txBufferLength = 0;
}

uint8_t TwoWire::endTransmission(bool sendStop) {
    cy_rslt_t result;
    bytesSent = txBufferLength;
    if (txBufferLength == 0) {
        result = cyhal_i2c_master_write(&i2c_obj, txAddress, txBuffer, 0, timeout, sendStop);
    } else {
        result = cyhal_i2c_master_write(&i2c_obj, txAddress, txBuffer, txBufferLength, timeout,
                                        sendStop);
        txBufferIndex = 0;
        txBufferLength = 0;
    }
    // Handle specific error codes
    switch (result) {
        case I2C_SUCCESS:
            return 0; // Success
        case I2C_NO_DEVICE_ATTACHED_PULL_UP:
            return 2; // NACK on transmit of address . Error: No device attached to SDA/SCL, but
                      // they are pulled-up
        case I2C_NO_DEVICE_ATTACHED_NO_PULL_UP:
            return 2; // NACK on transmit of address. Error: No device attached to SDA/SCL, and they
                      // are not pulled-up
        case I2C_TIMEOUT:
            return 5; // Timeout
        default:
            return 4; // Other error
    }
}

uint8_t TwoWire::endTransmission(void) {
    return endTransmission(true);
}

size_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit) {
    if (quantity > BUFFER_LENGTH) {
        quantity = BUFFER_LENGTH;
    }
    memset(rxBuffer, 0, BUFFER_LENGTH);
    cy_rslt_t result =
        cyhal_i2c_master_read(&i2c_obj, address, rxBuffer, quantity, timeout, stopBit);
    if (result == CY_RSLT_SUCCESS) {
        rxBufferIndex = 0;
        rxBufferLength = quantity;
        return quantity; // Return number of bytes read
    } else {
        return 0; // Return 0 on failure of read operation
    }
}

size_t TwoWire::requestFrom(uint8_t address, size_t len) {
    return requestFrom(address, len, true);
}

size_t TwoWire::write(uint8_t data) {
    /* Check if buffer is full */
    if (txBufferLength >= BUFFER_LENGTH) {
        return 0;
    }

    /* Put byte in txBuffer */
    txBuffer[txBufferIndex] = data;
    txBufferIndex++;

    /* Update buffer length */
    txBufferLength = txBufferIndex;

    return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity) {
    for (size_t i = 0; i < quantity; i++) {
        if (write(data[i]) == 0) {
            return i; // Return the number of bytes successfully written
        }
    }
    return quantity; // All bytes were successfully written
}

// New overloaded write function to accept a string
size_t TwoWire::write(const char *str) {
    if (str == nullptr) {
        return 0;
    }
    return write((const uint8_t *)str, strlen(str));
}

int TwoWire::available(void) {
    return rxBufferLength - rxBufferIndex;
}

int TwoWire::read(void) {
    if (rxBufferIndex < rxBufferLength) {
        int data = rxBuffer[rxBufferIndex++];
        if (rxBufferIndex == rxBufferLength) {
            // Clear the buffer after reading
            memset(rxBuffer, 0, BUFFER_LENGTH);
            rxBufferIndex = 0;
            rxBufferLength = 0;
        }
        return data;
    }
    return -1;
}

int TwoWire::peek(void) {
    if (rxBufferIndex < rxBufferLength) {
        return rxBuffer[rxBufferIndex];
    }
    return -1;
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
    static_cast<TwoWire *>(callback_arg)->i2c_event_handler_member(event);
}

void TwoWire::i2c_event_handler_member(cyhal_i2c_event_t event) {
    if (event == CYHAL_I2C_SLAVE_WR_CMPLT_EVENT) { // master wants to write data
        int availableBytes = bytesSent;
        if (availableBytes > 0) {
            onReceiveService(availableBytes);
        } else {
            return;
        }
    } else if (event == CYHAL_I2C_SLAVE_READ_EVENT) { // master wants to read data
        onRequestService();
    } else {
        return;
    }
}

// This function is called when the master wants to write data to the slave
void TwoWire::onReceiveService(int numBytes) {
    // Don't bother if user hasn't registered a callback
    if (!user_onReceive) {
        return;
    }

    // Don't bother if rx buffer is in use by a master requestFrom() op
    if (rxBufferIndex < rxBufferLength) {
        return;
    }

    // Manually update rxBufferLength to reflect the number of bytes received
    rxBufferLength = numBytes;
    rxBufferIndex = 0;

    // Alert user program
    user_onReceive(rxBufferLength);

    // Reconfigure the write buffer after the transaction
    w_status = cyhal_i2c_slave_config_write_buffer(&i2c_obj, rxBuffer, BUFFER_LENGTH);
    Wire_assert(w_status);
}

// This function is called when the master wants to read data from the slave
void TwoWire::onRequestService(void) {
    // Don't bother if user hasn't registered a callback
    if (!user_onRequest) {
        return;
    }
    // // Reset slave write buffer iterator var
    txBufferIndex = 0;
    txBufferLength = 0;

    // Alert user program
    user_onRequest();

    // Reconfigure the read buffer after the transaction
    w_status = cyhal_i2c_slave_config_read_buffer(&i2c_obj, txBuffer, BUFFER_LENGTH);
    Wire_assert(w_status);
}

#if I2C_HOWMANY > 0
TwoWire Wire = TwoWire(I2C1_SDA_PIN, I2C1_SCL_PIN, 0);
#endif

#if I2C_HOWMANY > 1
TwoWire Wire1 = TwoWire(I2C2_SDA_PIN, I2C2_SCL_PIN, 1);
#endif