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

#define PDM_FRAME_SIZE                      1024                 // Block size per transfer
#define AUDIO_BUFFER_SIZE                   (PDM_FRAME_SIZE * 8)  // Circular buffer size
#define AUDIO_SYS_CLOCK_HZ                  24576000u
#define BUFFER_SIZE                         512

class PDMClass {
public:
    PDMClass(pin_size_t sda, pin_size_t scl);
    PDMClass(); // Constructor

    typedef struct _ring_buf_t {
        uint8_t *buffer;
        size_t head;
        size_t tail;
        size_t size;
    } ring_buf_t;


    void ringbuf_init(size_t size);
    bool ringbuf_push(int32_t data);
    int32_t ringbuf_pop(int32_t &data);
    size_t ringbuf_available_data() const;
    size_t ringbuf_available_space() const;
    void   ringbuf_reset();

    // Primary API
    size_t available();
    void writeToRingBuffer(const uint8_t *data, size_t length);
    size_t buffer_size;
    int32_t active_rx_buffer[BUFFER_SIZE];
    int32_t full_rx_buffer[BUFFER_SIZE];
    int32_t *active_rx_buffer_p = nullptr;
    int32_t *full_rx_buffer_p = nullptr;

    // PCM samples pending in full_rx_buffer for user to read
    volatile uint32_t full_length = 0;
    volatile uint32_t read_index = 0;  // Index within full_rx_buffer
    volatile bool full_ready = false;
    bool pdm_inited = false;

    static void pdm_pcm_isr_handler(void *arg, cyhal_pdm_pcm_event_t event);
    bool begin(int channels, int sample_rate);
    void end();
    int32_t read(int32_t data, uint32_t size);
    void setBufferSize(uint32_t size);
    void setGain(uint8_t gain);
    // void onReceive(void(*)(void));

    void clock_init(void);
    // void (*_onReceive)(void);
private:
    pin_size_t sda_pin;
    pin_size_t scl_pin;
    cy_rslt_t pdm_status;
    cyhal_clock_t audio_clock;
    cyhal_pdm_pcm_t pdm_pcm;
    cyhal_clock_t pll_clock;
    ring_buf_t ringbuf;
};

#if (I2C_HOWMANY > 0)
extern PDMClass PDM;
#endif


#endif
