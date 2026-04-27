#ifndef PDM_H
#define PDM_H

#include "Arduino.h"
#include "cyhal_pdmpcm.h"
#include "cyhal_clock.h"

#define SIZEOF_DMA_BUFFER               (128)
#define AUDIO_SYS_CLOCK_HZ              24576000u               // Audio system clock frequency in Hz

/**
 * @class PDMClassPSOC
 * @brief Arduino-compatible PDM microphone driver for PSoC6
 *
 * Manages PDM/PCM hardware initialization, asynchronous DMA-based audio
 * capture, and a ring buffer for streaming audio samples to the application.
 */
class PDMClassPSOC {
public:
    /**
     * @brief Construct a new PDMClassPSOC object
     * @param pdmClockPin GPIO pin connected to the PDM clock line
     * @param pdmDataPin GPIO pin connected to the PDM data line
     */
    PDMClassPSOC(pin_size_t pdmClockPin, pin_size_t pdmDataPin);

    /**
     * @brief Destroy the PDMClassPSOC object and release all resources
     */
    ~PDMClassPSOC();

    /**
     * @brief Initialize the PDM interface
     * @param channels Number of channels (1 for mono, 2 for stereo)
     * @param sampleRate Sample rate in Hz (e.g., 16000, 44100)
     * @return 1 on success, 0 on already initialized, -1 on failure (e.g., buffer not set)
     */
    int begin(int channels, int sampleRate);

    /**
     * @brief Deinitialize the PDM interface
     */
    void end();

    /**
     * @brief Check the number of samples available for reading
     * @return Number of samples available
     */
    int available();

    /**
     * @brief Read audio samples from the PDM buffer
     * @param buffer Pointer to the buffer to store the samples
     * @param size Number of samples to read
     * @return Number of samples actually read
     */
    uint32_t read(void *buffer, size_t size);

    /**
     * @brief Set the callback function for when new data is available
     * @param callback Function to call when data is ready
     */
    void onReceive(void (*callback)(void));

    /**
     * @brief Set the input gain
     * @param gain Gain value (implementation specific range)
     */
    uint32_t setGain(int16_t gain);

    /**
     * @brief Set the ring buffer size (must be called before begin())
     * @param bufferSize Size of the ring buffer in samples (int32_t units)
     *                   Should be at least 2*SIZEOF_DMA_BUFFER for reliable operation
     */
    void setBufferSize(uint16_t bufferSize);

private:
    pin_size_t _pdmClockPin;
    pin_size_t _pdmDataPin;

    cyhal_pdm_pcm_t _pdm_pcm;
    cyhal_pdm_pcm_cfg_t _pdm_pcm_cfg;

    cyhal_clock_t _audioClock;
    cyhal_clock_t _pllClock;

    // DMA buffer for async reads (fixed size)
    int32_t _dmaBuffer[SIZEOF_DMA_BUFFER];

    // Dynamic ring buffer for accumulating audio data
    int32_t *_ringBuffer;
    volatile size_t _ringHead;          // Write position (ISR writes here)
    volatile size_t _ringTail;          // Read position (user reads from here)
    size_t _ringCapacity;               // Total capacity in samples

    /**
     * @brief Get the number of samples available for reading in the ring buffer
     * @return Number of samples available
     */
    size_t _ringAvailable() const;

    /**
     * @brief Get the number of free sample slots in the ring buffer
     * @return Number of free slots
     */
    size_t _ringFree() const;

    /**
     * @brief Write samples into the ring buffer
     * @param data Pointer to the source sample data
     * @param count Number of samples to write
     * @return Number of samples actually written
     */
    size_t _ringWrite(const int32_t *data, size_t count);

    /**
     * @brief Read samples from the ring buffer
     * @param data Pointer to the destination buffer
     * @param count Number of samples to read
     * @return Number of samples actually read
     */
    size_t _ringRead(int32_t *data, size_t count);

    /**
     * @brief Initialize the PLL and audio clocks for PDM operation
     * @return 0 on success, -1 on failure
     */
    int _clockInit(void);

    /**
     * @brief Static ISR callback for PDM/PCM async events
     * @param arg Pointer to the PDMClassPSOC instance (passed as void*)
     * @param event The PDM/PCM event that triggered the interrupt
     */
    static void _pdm_pcm_isr_handler(void *arg, cyhal_pdm_pcm_event_t event);

    /**
     * @brief Instance-level handler for PDM/PCM events
     * @param event The PDM/PCM event to handle
     */
    void _pdm_pcm_event_handler_member(cyhal_pdm_pcm_event_t event);

    void (*_user_onReceive)(void);     /**< User-registered data-ready callback */

    /**
     * @brief Process completed DMA transfer: expand samples, fill ring buffer, and restart async read
     */
    void onReceiveService(void);

    bool _initialized;
    bool _bufferInitialized;
};

#if (PDM_HOWMANY > 0)
extern PDMClassPSOC PDM;
#endif

#endif // PDM_H
