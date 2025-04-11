#include "Arduino.h"
#include "cyhal_adc.h"

// Constants and Macros
#define DEFAULT_ADC_RESOLUTION      12
#define MIN_ACQUISITION_TIME        220
#define adc_assert(cy_ret)          if (cy_ret != CY_RSLT_SUCCESS) { return 0xff; }

// Type Definitions
typedef struct {
    pin_size_t pin;
    cyhal_adc_channel_t chan_obj;
    bool initialized;
} adc_channel_t;

// Static Variables
static cyhal_adc_t adc_obj = {0};
static cyhal_adc_vref_t desiredVRef = CYHAL_ADC_REF_VDDA;
static bool adc_initialized = false;
static adc_channel_t adc_channel[ADC_HOWMANY] = {0};

// Helper Function: Initialize ADC hardware if not already done
static cy_rslt_t initialize_adc(pin_size_t pinNumber) {
    if (!adc_initialized) {
        cy_rslt_t status = cyhal_adc_init(&adc_obj, mapping_gpio_pin[pinNumber], NULL);
        adc_assert(status);
        adc_initialized = true;
    }

    // Configure ADC settings
    cyhal_adc_config_t adc_config = {
        .continuous_scanning = false,
        .resolution = DEFAULT_ADC_RESOLUTION,
        .average_count = 1,
        .vneg = CYHAL_ADC_VNEG_VSSA,
        .vref = desiredVRef,
        .ext_vref = NC,
        .bypass_pin = NC
    };

    return cyhal_adc_configure(&adc_obj, &adc_config);
}

static cy_rslt_t initialize_adc_channel(pin_size_t pinNumber, uint8_t adc_index) {
    // Ensure ADC hardware is initialized
    cy_rslt_t status = initialize_adc(pinNumber);
    adc_assert(status);

    // Configure individual ADC channel
    cyhal_adc_channel_config_t chan_config = {
        .enabled = true,
        .enable_averaging = false,
        .min_acquisition_ns = MIN_ACQUISITION_TIME
    };

    status = cyhal_adc_channel_init_diff(&adc_channel[adc_index].chan_obj, &adc_obj, mapping_gpio_pin[pinNumber], CYHAL_ADC_VNEG, &chan_config);
    adc_assert(status);

    adc_channel[adc_index].pin = pinNumber;
    adc_channel[adc_index].initialized = true;

    return status;
}

int analogRead(pin_size_t pinNumber) {
    int adc_value = 0;
    uint8_t adc_index = 0;

    // Find existing channel or initialize a new one
    for (adc_index = 0; adc_index < ADC_HOWMANY; adc_index++) {
        if (adc_channel[adc_index].initialized) {
            if (adc_channel[adc_index].pin == pinNumber) {
                // Found an existing channel for the pin
                break;
            }
        } else {
            // Found an uninitialized channel, initialize it
            cy_rslt_t result = initialize_adc_channel(pinNumber, adc_index);
            adc_assert(result);
            break;
        }
    }

    if (adc_index < ADC_HOWMANY) {
        adc_value = cyhal_adc_read(&adc_channel[adc_index].chan_obj);
    }
    return adc_value;
}

void analogReference(uint8_t mode) {
    desiredVRef = CYHAL_ADC_REF_VDDA;
}
