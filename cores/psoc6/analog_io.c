#include "Arduino.h"
#include "cyhal_adc.h"
#include "cyhal_pwm.h"

// Constants and Macros

#define MIN_ACQUISITION_TIME 220
#define adc_assert(cy_ret)           \
    if (cy_ret != CY_RSLT_SUCCESS) { \
        return 0xff;                 \
    }
#define pwm_assert(cy_ret)           \
    if (cy_ret != CY_RSLT_SUCCESS) { \
        return;                      \
    }

#define PWM_RESOLUTION_8_BIT 255.0
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
    float duty_cycle_percent;
    float frequency_hz;
    bool initialized;
    bool used;
} pwm_t;

// Static Variables

static cyhal_adc_t adc_obj = {0};
static cyhal_adc_vref_t desiredVRef = CYHAL_ADC_REF_VDDA;
static bool adc_initialized = false;
static adc_channel_t adc_channel[ADC_HOWMANY] = {0};
static int desiredReadResolution = ADC_RESOLUTION - 1; // 11-bit ADC resolution
static float desiredWriteResolution = PWM_RESOLUTION_8_BIT;
static pwm_t pwm[PWM_HOWMANY] = {0};

static cy_rslt_t initialize_adc(pin_size_t pinNumber) {
    if (!adc_initialized) {
        cy_rslt_t status = cyhal_adc_init(&adc_obj, mapping_gpio_pin[pinNumber], NULL);
        adc_assert(status);
        adc_initialized = true;
    }

    // Configure ADC settings
    cyhal_adc_config_t adc_config = {.continuous_scanning = false,
                                     .resolution = ADC_RESOLUTION,
                                     .average_count = 1,
                                     .vneg = CYHAL_ADC_VNEG_VSSA,
                                     .vref = desiredVRef,
                                     .ext_vref = NC,
                                     .bypass_pin = NC};

    return cyhal_adc_configure(&adc_obj, &adc_config);
}

static cy_rslt_t initialize_adc_channel(pin_size_t pinNumber, uint8_t adc_index) {
    // Ensure ADC hardware is initialized
    cy_rslt_t status = initialize_adc(pinNumber);
    adc_assert(status);

    // Configure individual ADC channel
    cyhal_adc_channel_config_t chan_config = {
        .enabled = true, .enable_averaging = false, .min_acquisition_ns = MIN_ACQUISITION_TIME};

    status = cyhal_adc_channel_init_diff(&adc_channel[adc_index].chan_obj, &adc_obj,
                                         mapping_gpio_pin[pinNumber], CYHAL_ADC_VNEG, &chan_config);
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
    uint8_t adc_res = ADC_RESOLUTION - 1; // 11-bit ADC resolution
    if (desiredReadResolution == adc_res) {
        return adc_value; // already in desired resolution
    } else if (desiredReadResolution < adc_res) {
        return adc_value >> (adc_res - desiredReadResolution); // reduce resolution
    }
    return adc_value << (desiredReadResolution - adc_res); // increase resolution
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

    if (adc_value <= 0) {
        adc_value = 0; // Ensure non-negative value
    } else {
        adc_value = map_adc_value((uint32_t)adc_value);
    }
    return adc_value;
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

static pwm_t * pwm_alloc(pin_size_t pinNumber) {
    if (pinNumber > GPIO_PIN_COUNT) {
        return NULL; // Invalid pin number
    }

    // First, look for existing allocation for this pin
    for (uint8_t i = 0; i < PWM_HOWMANY; i++) {
        if (pwm[i].used && pwm[i].pin == pinNumber) {
            return &pwm[i];
        }
    }

    // If not found, look for unused slot
    for (uint8_t i = 0; i < PWM_HOWMANY; i++) {
        if (!pwm[i].used) {
            // Allocate this slot
            pwm[i].pin = pinNumber;
            pwm[i].used = true;
            pwm[i].frequency_hz = 0;
            pwm[i].duty_cycle_percent = 0;
            pwm[i].initialized = false;
            return &pwm[i];
        }
    }

    return NULL; // No slots available
}

void analogWrite(pin_size_t pinNumber, int value) {
    // Allocate or find existing PWM slot
    pwm_t *pwm = pwm_alloc(pinNumber);
    if (pwm == NULL) {
        return;
    }

    if (!pwm->initialized) {
        cy_rslt_t result = cyhal_pwm_init(&pwm->pwm_obj, mapping_gpio_pin[pinNumber], NULL);
        pwm_assert(result);
        pwm->initialized = true;
    }

    int pwm_value = value;
    if (pwm_value <= 0) {
        pwm_value = 0;
    }
    if (pwm_value > desiredWriteResolution) {
        pwm_value = desiredWriteResolution;
    }

    // Calculate duty cycle
    float duty_cycle_percent = ((float)pwm_value / desiredWriteResolution) * 100.0f;
    pwm->duty_cycle_percent = duty_cycle_percent;

    // Use pre-configured frequency or default
    if (pwm->frequency_hz == 0) {
        pwm->frequency_hz = PWM_FREQUENCY_HZ;
    }

    // Set duty cycle and start PWM
    cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm->pwm_obj, duty_cycle_percent, pwm->frequency_hz);
    pwm_assert(result);

    result = cyhal_pwm_start(&pwm->pwm_obj);
    pwm_assert(result);
}

void setAnalogWriteFrequency(pin_size_t pinNumber, uint32_t frequency) {
    // Allocate or find existing PWM slot
    pwm_t *pwm = pwm_alloc(pinNumber);
    if (pwm == NULL) {
        return;
    }

    // Set frequency
    pwm->frequency_hz = frequency;

    // If already initialized, update the hardware immediately
    if (pwm->initialized) {
        cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm->pwm_obj, pwm->duty_cycle_percent, frequency);
        if (result != CY_RSLT_SUCCESS) {
            pwm_assert(result);
        }
    }
}
