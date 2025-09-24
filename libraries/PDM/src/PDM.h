#ifndef PDM_H
#define PDM_H

#include "Arduino.h"
#include "cyhal_pdmpcm.h"
#include "cyhal.h"

#define SAMPLE_RATE_HZ                      8000u
#define DECIMATION_RATE                     64u
#define DEFAULT_LEFT_GAIN                   0
#define DEFAULT_RIGHT_GAIN                  0
#define FRAME_SIZE                          (1024)

#define BUFFER_SIZE                         128u
#define PDM_FRAME_SIZE                      1024                 // Block size per transfer
#define AUDIO_BUFFER_SIZE                   (PDM_FRAME_SIZE * 8)  // Circular buffer size
#define AUDIO_SYS_CLOCK_HZ                  24576000u

class PDMClass {
public:
    PDMClass(pin_size_t sda, pin_size_t scl);
    PDMClass(); // Constructor

    int32_t active_rx_buffer[BUFFER_SIZE];
    int32_t full_rx_buffer[BUFFER_SIZE];
    int32_t *active_rx_buffer_p = nullptr;
    int32_t *full_rx_buffer_p = nullptr;

    // PCM samples pending in full_rx_buffer for user to read
    volatile uint32_t full_length = 0;
    volatile uint32_t read_index = 0;  // Index within full_rx_buffer
    volatile bool full_ready = false;
    bool pdm_inited = false;

    bool begin(int channels, int sample_rate);
    void end();
    uint32_t available();
    uint32_t read(uint16_t *out_buffer, uint32_t samples_to_read);
    static void pdm_pcm_isr_handler(void *arg, cyhal_pdm_pcm_event_t event);
    void clock_init(void);

private:
    pin_size_t sda_pin;
    pin_size_t scl_pin;
    cy_rslt_t pdm_status;
    cyhal_clock_t audio_clock;
    cyhal_pdm_pcm_t pdm_pcm;
    cyhal_clock_t pll_clock;
};

#if (I2C_HOWMANY > 0)
extern PDMClass PDM;
#endif


#endif
