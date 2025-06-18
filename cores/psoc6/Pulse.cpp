#include "Arduino.h"
#include "cyhal_timer.h"

#define ASSERT_RESULT(ret, cleanupFunc) \
    if ((ret) != CY_RSLT_SUCCESS) {     \
        cleanupFunc();                  \
        return 0;                       \
    }

static cyhal_timer_t pulseTimer;

void cleanupTimer() {
    cyhal_timer_stop(&pulseTimer);
    cyhal_timer_free(&pulseTimer);
}

uint8_t initTimer() {
    cy_rslt_t result = CY_RSLT_TYPE_ERROR;

    result = cyhal_timer_init(&pulseTimer, NC, NULL);
    ASSERT_RESULT(result, cleanupTimer);

    // Configure the timer with desired settings
    const cyhal_timer_cfg_t timerConfig = {
        .is_continuous = true,           // Timer runs continuously
        .direction = CYHAL_TIMER_DIR_UP, // Timer counts up
        .is_compare = false,             // No compare mode
        .period = 0xFFFFFFFF,            // Maximum timer period
        .compare_value = 0,              // No compare value
        .value = 0                       // Initial value
    };

    result = cyhal_timer_configure(&pulseTimer, &timerConfig);
    ASSERT_RESULT(result, cleanupTimer);

    result = cyhal_timer_start(&pulseTimer);
    ASSERT_RESULT(result, cleanupTimer);

    return 1;
}

bool waitForPinState(uint8_t pin,
                     uint8_t desiredState,
                     unsigned long timeoutMicros,
                     unsigned long startMicros) {
    uint32_t currentMicros;

    while (cyhal_gpio_read(mapping_gpio_pin[pin]) != desiredState) {
        currentMicros = cyhal_timer_read(&pulseTimer);
        if ((currentMicros - startMicros) > timeoutMicros) {
            return false;
        }
    }
    return true;
}

unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout) {
    uint32_t startMicros = 0, pulseStart = 0, pulseEnd = 0;
    unsigned long timeoutMicros = timeout;

    if (!initTimer()) {
        return 0;
    }

    startMicros = cyhal_timer_read(&pulseTimer);

    // Wait for the pulse to start
    if (!waitForPinState(pin, state, timeoutMicros, startMicros)) {
        cleanupTimer();
        return 0; // Timeout while waiting for the pulse to start
    }
    pulseStart = cyhal_timer_read(&pulseTimer);

    // Wait for the pulse to end
    if (!waitForPinState(pin, !state, timeoutMicros, startMicros)) {
        cleanupTimer();
        return 0; // Timeout while waiting for the pulse to end
    }
    pulseEnd = cyhal_timer_read(&pulseTimer);

    cleanupTimer();
    // Timer counts directly represent microseconds
    return pulseEnd - pulseStart;
}

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout) {
    return pulseInLong(pin, state, timeout);
}
