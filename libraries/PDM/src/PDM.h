#ifndef PDM_H
#define PDM_H

#include <cstddef>
#include <cstdint>
#include <stdint.h>

#include "Arduino.h"
#include "cyhal_pdmpcm.h"
#include "cyhal_clock.h"

// #define SIZEOF_DMA_BUFFER               (128)
// #define SIZEOF_HALF_DMA_BUFFER          (SIZEOF_DMA_BUFFER / 2)
// #define SIZEOF_DMA_BUFFER_IN_BYTES      (SIZEOF_DMA_BUFFER * sizeof((uint32_t)))
// #define SIZEOF_HALF_DMA_BUFFER_IN_BYTES (SIZEOF_DMA_BUFFER_IN_BYTES / 2)
// #define PDM_PCM_RX_FRAME_SIZE_IN_BYTES  (8)

// #define NON_BLOCKING_RATE_MULTIPLIER    (4)
// #define SIZEOF_NON_BLOCKING_COPY_IN_BYTES (SIZEOF_HALF_DMA_BUFFER * NON_BLOCKING_RATE_MULTIPLIER)

// #define DEFAULT_LEFT_GAIN               0
// #define DEFAULT_RIGHT_GAIN              0

// #define AUDIO_SYS_CLOCK_24_576_000_HZ   24576000u
// #define AUDIO_SYS_CLOCK_22_579_000_HZ   22579000u

// typedef enum {
//         BITS_16 = 16,
//         BITS_18 = 18,
//         BITS_20 = 20,
//         BITS_24 = 24
// } pdm_pcm_word_length_t;

// typedef enum {
//     MONO_LEFT   = CYHAL_PDM_PCM_MODE_LEFT,
//     MONO_RIGHT  = CYHAL_PDM_PCM_MODE_RIGHT,
//     STEREO      = CYHAL_PDM_PCM_MODE_STEREO
// } format_t;

// typedef enum {
//     BLOCKING,
//     NON_BLOCKING,
// } io_mode_t;

#define FRAME_SIZE              (1024)                  // Number of samples per frame
#define THRESHOLD_HYSTERESIS    (256)                   // Noise threshold hysteresis
#define VOLUME_RATIO            (4*FRAME_SIZE)          // Volume ratio for noise and print purposes
#define SAMPLE_RATE_HZ          8000u                   // Sample rate in Hz
#define DECIMATION_RATE         64u                     // Decimation rate for PDM to PCM conversion
#define AUDIO_SYS_CLOCK_HZ      24576000u               // Audio system clock frequency in Hz
#define PDM_DATA_PIN            P10_5                   // PDM data pin
#define PDM_CLOCK_PIN           P10_4                   // PDM clock pin


class PDMClassPSOC {
public:
        PDMClassPSOC(pin_size_t pdmDataPin, pin_size_t pdmClockPin);
        ~PDMClassPSOC();

        /**
         * @brief Initialize the PDM interface
         * @param channels Number of channels (1 for mono, 2 for stereo)
         * @param sampleRate Sample rate in Hz (e.g., 16000, 44100)
         * @return 1 on success, 0 on failure
         */
        int begin(int channels, int sampleRate);

        /**
         * @brief Deinitialize the PDM interface
         */
        void end();

        /**
         * @brief Check the number of bytes available for reading
         * @return Number of bytes available
         */
        int available();

        /**
         * @brief Read audio samples from the PDM buffer
         * @param buffer Pointer to the buffer to store the samples
         * @param size Number of bytes to read
         * @return Number of bytes actually read
         */
        int read(void* buffer, size_t size);

        /**
         * @brief Set the callback function for when new data is available
         * @param callback Function to call when data is ready
         */
        void onReceive(void (*callback)(void));

        /**
         * @brief Set the input gain
         * @param gain Gain value (implementation specific range)
         */
        void setGain(int gain);

        /**
         * @brief Set the internal buffer size
         * @param bufferSize Size of the buffer in bytes
         */
        void setBufferSize(uint16_t bufferSize);

private:
        int _channels;
        int _sampleRate;
        int16_t *_audioDataBuffer;

        void _clockInit(void);
        static void _pdm_pcm_isr_handler(void *arg, cyhal_pdm_pcm_event_t event);
        void _pdm_pcm_event_handler_member(cyhal_pdm_pcm_event_t event);
        void (*_user_onReceive)(void);
        void onReceiveService(void);

        cyhal_pdm_pcm_t _pdm_pcm;
        cyhal_pdm_pcm_cfg_t _pdm_pcm_cfg;

        cyhal_clock_t _audioClock;
        cyhal_clock_t _pllCLock;

        pin_size_t _pdmDataPin;
        pin_size_t _pdmClockPin;

        bool _initialized;
        bool _bufferInitialized;
};

#endif // PDM_H