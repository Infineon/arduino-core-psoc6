#include "PDM.h"

#define PDM_assert(cy_ret) if (cy_ret != CY_RSLT_SUCCESS) { \
            return; \
}

PDMClass::PDMClass(pin_size_t sda, pin_size_t scl) : sda_pin(sda), scl_pin(scl) {
}
bool PDMClass::begin(int channels, int sample_rate) {
    Serial.println("Begin function called");

    if (channels != 1 && channels != 2) {
        Serial.println("Invalid channel. Desired channel should be 1 0r 2");
        return false;
    }
    cyhal_pdm_pcm_cfg_t pdm_pcm_cfg = {
        .sample_rate = SAMPLE_RATE_HZ,
        .decimation_rate = DECIMATION_RATE,
        .mode = (channels == 2) ? CYHAL_PDM_PCM_MODE_STEREO : CYHAL_PDM_PCM_MODE_LEFT,
        .word_length = 16,
        .left_gain = DEFAULT_LEFT_GAIN,
        .right_gain = DEFAULT_RIGHT_GAIN,
    };
    // buffer_size = BUFFER_SIZE;
    if (sample_rate == 8000 || sample_rate == 16000 || sample_rate == 32000 || sample_rate == 48000) {
        pdm_pcm_cfg.sample_rate = sample_rate;
    } else {
        Serial.println("Invalid sample rate. Desired sample rate Typical values: 8/16/32/48kHz");
        return false;
    }
    PDMClass::clock_init();
    // ringbuf_init -->to be implemented
    PDMClass::ringbuf_init(BUFFER_SIZE);
    cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_ON);  // pcm signal with gpio toggling
    pdm_status = cyhal_pdm_pcm_init(&pdm_pcm,  mapping_gpio_pin[sda_pin], mapping_gpio_pin[scl_pin], &audio_clock, &pdm_pcm_cfg);
    if (pdm_status != CY_RSLT_SUCCESS) {
        Serial.println("cyhal_pdmpcm init failed");
        return false;
    }

    cyhal_pdm_pcm_register_callback(&pdm_pcm, pdm_pcm_isr_handler, this);
    cyhal_pdm_pcm_enable_event(&pdm_pcm, CYHAL_PDM_PCM_ASYNC_COMPLETE, 1, true);
    cyhal_pdm_pcm_set_async_mode(&pdm_pcm, CYHAL_ASYNC_DMA, CYHAL_ISR_PRIORITY_DEFAULT);

    // _dma_buff_init
    active_rx_buffer_p = active_rx_buffer;
    full_rx_buffer_p = active_rx_buffer;
    full_ready = false;
    cyhal_pdm_pcm_read_async(&pdm_pcm, active_rx_buffer, BUFFER_SIZE);
    pdm_status = cyhal_pdm_pcm_start(&pdm_pcm);
    if (pdm_status != CY_RSLT_SUCCESS) {
        Serial.println("PDM start failed!");
        return false;
    }
    pdm_inited = true;
    return true;
}

void PDMClass::end() {
    pdm_status = cyhal_pdm_pcm_stop(&pdm_pcm);
    PDM_assert(pdm_status);
    cyhal_pdm_pcm_free(&pdm_pcm);
}

void PDMClass::setBufferSize(uint32_t size) {

    if (pdm_inited == false) {
        buffer_size = size;
    }

}
void PDMClass::setGain(uint8_t gain) {

    if (pdm_inited == true) {
        cyhal_pdm_pcm_set_gain(&pdm_pcm, gain, gain);
    }

}

static void PDMClass::pdm_pcm_isr_handler(void *arg, cyhal_pdm_pcm_event_t event) {
    Serial.println("pdm_pcm_isr_handler init");
    cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
    PDMClass *obj = static_cast < PDMClass * > (arg);

    if (0u != (event & CYHAL_PDM_PCM_ASYNC_COMPLETE)) {
        // Swap active and full
        int32_t *temp = obj->active_rx_buffer_p;
        obj->active_rx_buffer_p = obj->full_rx_buffer_p;
        obj->full_rx_buffer_p = temp;

        // Mark new buffer as filled, ready for reading
        obj->full_length = BUFFER_SIZE;
        obj->read_index = 0;
        obj->full_ready = true;

        // Start DMA on buffer
        cyhal_pdm_pcm_read_async(&obj->pdm_pcm, obj->active_rx_buffer_p, BUFFER_SIZE);
        // dma_copy_from_dmabuf_to_ringbuf

        for (size_t i = 0; i < BUFFER_SIZE; ++i) {
            // Push each 32-bit value as 4 bytes
            uint32_t sample = obj->active_rx_buffer_p[i];
            for (size_t b = 0; b < 4; ++b) {
                obj->ringbuf_push((sample >> (8 * b)) & 0xFF);
            }
        }
    }
}

void PDMClass::clock_init(void) {
    /* Initialize the PLL */
    cyhal_clock_reserve(&pll_clock, &CYHAL_CLOCK_PLL[0]);
    cyhal_clock_set_frequency(&pll_clock, AUDIO_SYS_CLOCK_HZ, NULL);
    cyhal_clock_set_enabled(&pll_clock, true, true);

    /* Initialize the audio subsystem clock (CLK_HF[1])
     * The CLK_HF[1] is the root clock for the I2S and PDM/PCM blocks */
    cyhal_clock_reserve(&audio_clock, &CYHAL_CLOCK_HF[1]);

    /* Source the audio subsystem clock from PLL */
    cyhal_clock_set_source(&audio_clock, &pll_clock);
    cyhal_clock_set_enabled(&audio_clock, true, true);
}

// Initialize (or re-initialize) the ring buffer
void PDMClass::ringbuf_init(size_t size) {
    // Free previous buffer if it exists
    if (ringbuf.buffer != nullptr) {
        delete[] ringbuf.buffer;
    }
    ringbuf.buffer = new uint8_t[size];
    memset(ringbuf.buffer, 0, size);
    ringbuf.size = size;
    ringbuf.head = 0;
    ringbuf.tail = 0;
}
// Ring buffer operations as before...

bool PDMClass::ringbuf_push(int32_t data) {
    size_t next_tail = (ringbuf.tail + 1) % ringbuf.size;
    if (next_tail != ringbuf.head) {
        ringbuf.buffer[ringbuf.tail] = data;
        ringbuf.tail = next_tail;
        return true;
    }
    return false;
}

int32_t PDMClass::ringbuf_pop(int32_t &data) {
    if (ringbuf.head == ringbuf.tail) {
        return false;
    }
    data = ringbuf.buffer[ringbuf.head];
    ringbuf.head = (ringbuf.head + 1) % ringbuf.size;
    return data;
}

size_t PDMClass::ringbuf_available_data() const {
    return (ringbuf.tail - ringbuf.head + ringbuf.size) % ringbuf.size;
}

size_t PDMClass::ringbuf_available_space() const {
    return ringbuf.size - ringbuf_available_data() - 1;
}

void PDMClass::ringbuf_reset() {
    ringbuf.head = 0;
    ringbuf.tail = 0;
    memset(ringbuf.buffer, 0, ringbuf.size);
}

// High-level API...

size_t PDMClass::available() {

    size_t length_of_data = ringbuf_available_data();
    Serial.println("available length_of_data");
    Serial.println(length_of_data);
    size_t words_popped = 0;
    // Each word is 4 bytes
    while (words_popped < ringbuf.size) {
        // Check if there are at least 4 bytes available
        size_t bytes_available = (ringbuf.tail - ringbuf.head + ringbuf.size) % ringbuf.size;
        if (bytes_available < 4) {
            break;
        }

        uint32_t word = 0;
        for (size_t b = 0; b < 4; ++b) {
            word |= (static_cast < uint32_t > (ringbuf.buffer[ringbuf.head]) << (8 * b));
            ringbuf.head = (ringbuf.head + 1) % ringbuf.size;
        }
        // buffer[words_popped] = word;
        ++words_popped;
    }
    // Return the number of words read
    return words_popped;
}

int32_t PDMClass::read(int32_t data, uint32_t size) {
    if (size != 0) {
        int32_t data = ringbuf_pop(data);
        Serial.println(data);
    }
    return data;
}

void PDMClass::writeToRingBuffer(const uint8_t *data, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        if (!ringbuf_push(data[i])) {
            break;
        }
    }
}

#if I2C_HOWMANY > 0
PDMClass PDM(PDM_SDA_PIN, PDM_SCL_PIN);
#endif
