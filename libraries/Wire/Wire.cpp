extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}

#include "Wire.h"

#define I2C_DEFAULT_FREQ 100000

TwoWire::TwoWire(cyhal_gpio_t sda, cyhal_gpio_t scl) : sda_pin(sda), scl_pin(scl) {
}

void TwoWire::_begin() {

    rxBufferIndex = 0;
    rxBufferLength = 0;

    txBufferIndex = 0;
    txBufferLength = 0;
    
    if (is_master) {
        i2c_config= {
            .is_slave = is_master ? false : true,
            0,
            .frequencyhal_hz = I2C_DEFAULT_FREQ
        };
    }
    else {
        i2c_config = {
            .is_slave = is_master ? false : true,
            slave_address,
            .frequencyhal_hz = I2C_DEFAULT_FREQ
        };
    }
        cyhal_i2c_init(&i2c_obj, sda_pin, scl_pin, NULL);
        cyhal_i2c_configure(&i2c_obj, &i2c_config);
        cyhal_i2c_register_callback(&i2c_obj, i2c_event_handler, this);
}

void TwoWire::begin() {
    end();
    is_master = true;
    _begin();
}

void TwoWire::begin(uint16_t address) {
    end();
    is_master = false;
    slave_address = address;
    _begin();
}

void TwoWire::begin(int address) {
    begin((uint16_t)address);
}

void TwoWire::begin(uint8_t address) {
    begin((uint16_t)address);
}

void TwoWire::end() {
    cyhal_i2c_free(&i2c_obj);
}

void TwoWire::setClock(uint32_t freq) {
    if(is_master) {
        i2c_config = {
            .is_slave = false,
            0,
            .frequencyhal_hz = freq
        };
    }
    else {
        i2c_config = {
            .is_slave = true,
            slave_address,
            .frequencyhal_hz = freq
        };
    }
    cyhal_i2c_configure(&i2c_obj, &i2c_config);
}

void TwoWire::   beginTransmission(uint8_t address) {
    txAddress = address;
    txBufferIndex = 0;
    txBufferLength = 0;
}

uint8_t TwoWire::endTransmission(bool sendStop) {
    cy_rslt_t result = cyhal_i2c_master_write(&i2c_obj, slave_address, txBuffer, txBufferLength, timeout, sendStop);
    
    txBufferIndex = 0;
    txBufferLength = 0;

    return (result == CY_RSLT_SUCCESS) ? 0 : 1; // Return 0 on success, 1 on failure
}

uint8_t TwoWire::endTransmission(void) {
    return endTransmission(true);
}

size_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit) {
    if(quantity > BUFFER_LENGTH){
        quantity = BUFFER_LENGTH;
    }
    cy_rslt_t result = cyhal_i2c_master_read(&i2c_obj, address, rxBuffer, quantity, timeout, stopBit);   
    if(result == CY_RSLT_SUCCESS){
        rxBufferIndex = 0;
        rxBufferLength = quantity;
        return quantity;
    }
    else{
        return 0; // Return 0 on failure of read operation
    }
}

size_t TwoWire::requestFrom(uint8_t address, size_t len) {
    return requestFrom(address, len, true);
}

size_t TwoWire::write(uint8_t data) {
    	/* Check if buffer is full */
	if(txBufferLength >= BUFFER_LENGTH){
	  return 0;
	}

	/* Put byte in txBuffer */
	txBuffer[txBufferIndex] = data;
	txBufferIndex++;

	/* Update buffer length */
	txBufferLength = txBufferIndex;
	 
	return 1;
}

size_t TwoWire::write(const uint8_t * data, size_t quantity) {
  	for(size_t i = 0; i < quantity; i++){
		write(*(data + i));
	}

	return quantity;
}

int TwoWire::available(void) {

    return rxBufferLength - rxBufferIndex;
}

int TwoWire::read(void) {
   if(rxBufferIndex < rxBufferLength)
    {
        return rxBuffer[rxBufferIndex++];
    }
    return -1;
}

int TwoWire::peek(void) {
   if(rxBufferIndex < rxBufferLength)
    {
        return rxBuffer[rxBufferIndex];
    }
    return -1;
}

void TwoWire::onReceive(void (*function)(int)) {
   user_onReceive = function;
   cyhal_i2c_enable_event(&i2c_obj, CYHAL_I2C_SLAVE_READ_EVENT, 7, true);
}

void TwoWire::onRequest(void (*function)(void)) {
    user_onRequest = function;
    cyhal_i2c_enable_event(&i2c_obj, CYHAL_I2C_SLAVE_WRITE_EVENT, 7, true);
}

void TwoWire::i2c_event_handler(void *callback_arg, cyhal_i2c_event_t event) {
    TwoWire *instance = static_cast<TwoWire*>(callback_arg);
    if (event == CYHAL_I2C_SLAVE_READ_EVENT && instance->user_onReceive) {
        instance->user_onReceive(instance->available());
    } else if (event == CYHAL_I2C_SLAVE_WRITE_EVENT && instance->user_onRequest) {
        instance->user_onRequest();
    }
}

#if I2C_HOWNMANY > 0
TwoWire Wire = TwoWire(I2C1_SDA_PIN, I2C1_SCL_PIN);
#endif

#if I2C_HOWNMANY > 1
TwoWire Wire1 = TwoWire(I2C2_SDA_PIN, I2C2_SCL_PIN);
#endif

