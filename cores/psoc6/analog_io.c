/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"
#include "cyhal_adc.h"

#define DEFAULT_ADC_RESOLUTION      12
#define MIN_ACQUISITION_TIME        220

#define adc_assert(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
            return cy_ret; \
}

static cy_rslt_t status = CY_RSLT_TYPE_ERROR;
static uint8_t desiredResolution = DEFAULT_ADC_RESOLUTION;
static cyhal_adc_vref_t desiredVRef = CYHAL_ADC_REF_VDDA;

int analogRead(pin_size_t pinNumber) {

    if (pinNumber != A0 && pinNumber != A1) {
        return 0; // Invalid pin number for ADC
    }

    // Initialize the ADC
    cyhal_adc_t adc_obj;
    cyhal_adc_channel_t adc_chan_obj;

    // ADC configuration
    cyhal_adc_config_t adc_config = {
        .continuous_scanning = false,           // Continuous Scanning is disabled
        .average_count = 1,                     // Average count disabled
        .vref = desiredVRef,                    // VREF for Single ended channel set to VDDA
        .vneg = CYHAL_ADC_VNEG_VSSA,            // VNEG for Single ended channel set to VSSA
        .resolution = DEFAULT_ADC_RESOLUTION,   // 12-bit resolution
        .ext_vref = NC,                         // No connection
        .bypass_pin = NC                        // No connection
    };

    // Initialize and configure the ADC
    status = cyhal_adc_init(&adc_obj, mapping_gpio_pin[pinNumber], NULL);
    adc_assert(status);
    status = cyhal_adc_configure(&adc_obj, &adc_config);
    adc_assert(status);

    // Configure the ADC channel
    cyhal_adc_channel_config_t chan_config = {
        .enable_averaging = false,
        .min_acquisition_ns = MIN_ACQUISITION_TIME,
        .enabled = true
    };

    status = cyhal_adc_channel_init_diff(&adc_chan_obj, &adc_obj, mapping_gpio_pin[pinNumber], NC, &chan_config);
    adc_assert(status);

    int rawValue = cyhal_adc_read(&adc_chan_obj);

    // Free resources after reading
    cyhal_adc_channel_free(&adc_chan_obj);
    cyhal_adc_free(&adc_obj);

    if (desiredResolution < DEFAULT_ADC_RESOLUTION) {
        return rawValue >> (DEFAULT_ADC_RESOLUTION - desiredResolution);
    } else {
        return rawValue;
    }
}

void analogReference(uint8_t mode) {
    switch (mode) {
        case DEFAULT:
        case INTERNAL_VDDA:
            desiredVRef = CYHAL_ADC_REF_VDDA;
            break;

        case INTERNAL:
        case INTERNAL_1V2:
            desiredVRef = CYHAL_ADC_REF_INTERNAL;
            break;

        case INTERNAL_VDDA_DIV_2:
            desiredVRef = CYHAL_ADC_REF_VDDA_DIV_2;
            break;

        case EXTERNAL:
            desiredVRef = CYHAL_ADC_REF_EXTERNAL;
            break;

        default:
            break;
    }
}

void analogReadResolution(uint8_t res) {
    desiredResolution = res;
}
