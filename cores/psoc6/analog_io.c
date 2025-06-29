#include "Arduino.h"
#include "cyhal_adc.h"
#include "cyhal_pwm.h"

// Constants and Macros

#define MIN_ACQUISITION_TIME        220
#define adc_assert(cy_ret)          if (cy_ret != CY_RSLT_SUCCESS) { return 0xff; }
#define pwm_assert(cy_ret)          if (cy_ret != CY_RSLT_SUCCESS) { return; }

#define PWM_RESOLUTION_8_BIT  255.0
#define PWM_RESOLUTION_10_BIT 1023.0
#define PWM_RESOLUTION_12_BIT 4095.0
#define PWM_RESOLUTION_16_BIT 65535.0

// Type Definitions

typedef struct {
    pin_size_t pin;
    cyhal_adc_channel_t chan_obj;
    bool initialized;
} adc_channel_t;

typedef struct {
    cyhal_pwm_t pwm_obj;
    pin_size_t pin;
    bool initialized;
} pwm_t;

// Static Variables

static cyhal_adc_t adc_obj = {0};
static cyhal_adc_vref_t desiredVRef = CYHAL_ADC_REF_VDDA;
static bool adc_initialized = false;
static adc_channel_t adc_channel[ADC_HOWMANY] = {0};
static int desiredReadResolution = ADC_RESOLUTION;
static float desiredWriteResolution = PWM_RESOLUTION_8_BIT;
static pwm_t pwm[PWM_HOWMANY] = {0};


static cy_rslt_t initialize_adc(pin_size_t pinNumber) {
    if (!adc_initialized) {
        cy_rslt_t status = cyhal_adc_init(&adc_obj, mapping_gpio_pin[pinNumber], NULL);
        adc_assert(status);
        adc_initialized = true;
    }

    // Configure ADC settings
    cyhal_adc_config_t adc_config = {
        .continuous_scanning = false,
        .resolution = ADC_RESOLUTION,
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

void analogReadResolution(int res) {
    if (res < 1) {
        desiredReadResolution = 1; // Minimum resolution
    } else {
        desiredReadResolution = res;
    }
}

static inline uint32_t  map_adc_value(uint32_t adc_value) {
    if (desiredReadResolution == ADC_RESOLUTION) {
        return adc_value; // already in desired resolution
    } else if (desiredReadResolution < ADC_RESOLUTION) {
        return adc_value >> (ADC_RESOLUTION - desiredReadResolution); // reduce resolution
    }
    return adc_value << (desiredReadResolution - ADC_RESOLUTION); // increase resolution
}

// Helper to find or initialize ADC channel
static int get_or_init_adc_channel(pin_size_t pinNumber) {
    for (uint8_t i = 0; i < ADC_HOWMANY; i++) {
        if (adc_channel[i].initialized && adc_channel[i].pin == pinNumber) {
            return i;
        } else if (!adc_channel[i].initialized) {
            cy_rslt_t result = initialize_adc_channel(pinNumber, i);
            adc_assert(result);
            return i;
        }
    }
    return -1; // No available channel
}

int analogRead(pin_size_t pinNumber) {
    int adc_value = 0;
    int adc_index = get_or_init_adc_channel(pinNumber);
    if (adc_index >= 0) {
        adc_value = cyhal_adc_read(&adc_channel[adc_index].chan_obj);
    }
    if (adc_value < 0) {
        adc_value = 0;
    }
    return map_adc_value((uint32_t)adc_value);
}

void analogReference(uint8_t mode) {
    desiredVRef = CYHAL_ADC_REF_VDDA;
}

void analogWriteResolution(int res) {
    if (res > 12) {
        desiredWriteResolution = PWM_RESOLUTION_16_BIT;
    } else if (res > 10) {
        desiredWriteResolution = PWM_RESOLUTION_12_BIT;
    } else if (res > 8) {
        desiredWriteResolution = PWM_RESOLUTION_10_BIT;
    } else {
        desiredWriteResolution = PWM_RESOLUTION_8_BIT;
    }
}

// Helper to find or initialize PWM channel
static int get_or_init_pwm_channel(pin_size_t pinNumber) {
    for (uint8_t i = 0; i < PWM_HOWMANY; i++) {
        if (pwm[i].initialized && pwm[i].pin == pinNumber) {
            return i;
        } else if (!pwm[i].initialized) {
            cy_rslt_t result = cyhal_pwm_init(&pwm[i].pwm_obj, mapping_gpio_pin[pinNumber], NULL);
            if (result != CY_RSLT_SUCCESS) {
                return -1; // Initialization failed
            }
            pwm[i].pin = pinNumber;
            pwm[i].initialized = true;
            return i;
        }
    }
    return -1; // No available channel
}

// Clamp helper
static inline uint32_t clamp_uint32(uint32_t val, uint32_t min, uint32_t max) {
    return (val < min) ? min : (val > max) ? max : val;
}

void analogWrite(pin_size_t pinNumber, int value) {
    if (pinNumber > GPIO_PIN_COUNT) {
        return; // Invalid pin number
    }
    int pwm_index = get_or_init_pwm_channel(pinNumber);
    if (pwm_index < 0) {
        return;
    }
    uint32_t pwm_value = clamp_uint32((uint32_t)value, 0, (uint32_t)desiredWriteResolution);
    float duty_cycle_percentage = (pwm_value / desiredWriteResolution) * 100.0f;
    cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm[pwm_index].pwm_obj, duty_cycle_percentage, PWM_FREQUENCY_HZ);
    pwm_assert(result);
    result = cyhal_pwm_start(&pwm[pwm_index].pwm_obj);
    pwm_assert(result);
}
