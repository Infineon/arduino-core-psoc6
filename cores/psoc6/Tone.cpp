#include "Arduino.h"
#include "cyhal_pwm.h"
#include "cyhal_timer.h"

void timerISR(void *callback_arg, cyhal_timer_event_t event);
constexpr float PWM_DUTY_CYCLE_50_PERCENTAGE = 50.0f;

class PwmTone {
public:
    static PwmTone& getInstance() {
        static PwmTone instance;
        return instance;
    }

    void handleTimerEvent(void *callback_arg, cyhal_timer_event_t event) {
        (void)callback_arg;
        (void)event;

        cyhal_timer_stop(&timerObj);
        cyhal_timer_free(&timerObj);
        timerInitialized = false;

        stop();
    }

    void play(uint8_t targetPin, unsigned int frequency, unsigned long duration = 0) {
        // Tone is already playing on a different pin
        if (pwmInitialized && targetPin != pin) {
            return;
        }

        // First call to play on a specified pin
        if (!pwmInitialized) {
            initializePwm(targetPin);
            startPwm();
        }

        // Update frequency for first call or Tone already playing on the same pin
        setDutyCycleAndFrequency(frequency);

        if (duration > 0) {
            startTimer(duration);
        }
    }

    void stop() {
        if (pwmInitialized) {
            stopPwm();
            freePwm();
            pwmInitialized = false;
        }
    }

private:
    cyhal_pwm_t pwmObj = {};
    cyhal_timer_t timerObj = {};

    bool pwmInitialized = false;
    uint8_t pin = 0;
    bool timerInitialized = false;

    PwmTone() : pwmInitialized(false), pin(0), timerInitialized(false) {
    }
    ~PwmTone() {
        stop();
    }

    /* Delete copy constructor and assignment operator  */
    PwmTone(const PwmTone&) = delete;
    PwmTone& operator = (const PwmTone&) = delete;

    void initializePwm(uint8_t targetPin) {
        cy_rslt_t result = cyhal_pwm_init(&pwmObj, mapping_gpio_pin[targetPin], nullptr);
        assertResult(result);
        pin = targetPin;
        pwmInitialized = true;
    }

    void setDutyCycleAndFrequency(unsigned int frequency) {
        cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwmObj, PWM_DUTY_CYCLE_50_PERCENTAGE, frequency);
        assertResult(result);
    }

    void startPwm() {
        cy_rslt_t result = cyhal_pwm_start(&pwmObj);
        assertResult(result);
    }

    void stopPwm() {
        cy_rslt_t result = cyhal_pwm_stop(&pwmObj);
        assertResult(result);
    }

    void freePwm() {
        cyhal_pwm_free(&pwmObj);
    }

    void startTimer(unsigned long duration) {

        cy_rslt_t result = CY_RSLT_TYPE_ERROR;

        if (!timerInitialized) {
            result = cyhal_timer_init(&timerObj, NC, nullptr);
            assertResult(result);
            timerInitialized = true;
        }

        float period = (float)duration / 1000.0f;  // ms to s conversion
        uint32_t period_hal;        // Period/count input for the PSOC6 HAL timer configuration
        uint32_t fz_hal = 1000000;  // Frequency for the PSOC timer clock is fixed as 1 MHz
        period_hal = (uint32_t)(period * fz_hal) - 1; // Overflow Period = (Period + 1)/ frequency ;period = (overflow period * frequency)-1

        // Adjust the frequency & recalculate the period if period/count is  greater than the maximum overflow value for a 32 bit timer ie; 2^32
        while (period_hal > 4294967296) {
            fz_hal = fz_hal / 10;  // Reduce the fz_hal value by 10%
            period_hal = (uint32_t)(period * fz_hal) - 1;  // Recalculate Period input for the PSOC6 HAL timer configuration
        }

        const cyhal_timer_cfg_t timer_cfg =
        {
            .is_continuous = false,             /* Run the timer */
            .direction = CYHAL_TIMER_DIR_UP,    /* Timer counts up */
            .is_compare = false,                /* Don't use compare mode */
            .period = period_hal,               /* Defines the timer period */
            .compare_value = 0,                 /* Timer compare value, not used */
            .value = 0                          /* Initial value of counter */
        };

        cyhal_timer_stop(&timerObj);

        result = cyhal_timer_configure(&timerObj, &timer_cfg);
        assertResult(result);

        result = cyhal_timer_set_frequency(&timerObj, fz_hal);
        assertResult(result);

        cyhal_timer_register_callback(&timerObj, timerISR, nullptr);

        cyhal_timer_enable_event(&timerObj, CYHAL_TIMER_IRQ_TERMINAL_COUNT, 3, true);

        result = cyhal_timer_start(&timerObj);
        assertResult(result);

    }

    static void assertResult(cy_rslt_t result) {
        if (result != CY_RSLT_SUCCESS) {
            exit(EXIT_FAILURE);
        }
    }
};

void timerISR(void *callback_arg, cyhal_timer_event_t event) {
    PwmTone::getInstance().handleTimerEvent(callback_arg, event);
}

void tone(uint8_t _pin, unsigned int frequency, unsigned long duration) {
    if (_pin > GPIO_PIN_COUNT) {
        return; // Invalid pin number
    }
    PwmTone::getInstance().play(_pin, frequency, duration);
}

void noTone(uint8_t _pin) {
    if (_pin > GPIO_PIN_COUNT) {
        return; // Invalid pin number
    }
    PwmTone::getInstance().stop();
}
