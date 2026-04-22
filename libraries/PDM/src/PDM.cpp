#include "PDM.h"
#include <cstdint>
#include <cstdlib>


#define pdm_assert(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
            return; \
}

PDMClassPSOC::PDMClassPSOC(pin_size_t pdmDataPin, pin_size_t pdmClockPin) : _pdmDataPin(pdmDataPin), _pdmClockPin(pdmClockPin), _initialized(false), _bufferInitialized(false) {

}

PDMClassPSOC::~PDMClassPSOC() {
    end();
}

int PDMClassPSOC::begin(int channels, int sampleRate) {
    if (!_bufferInitialized){
        return -1; // Buffer size not set
    }
    if (_initialized) {
        return 0; // Already initialized
    }

    _pdm_pcm_cfg = {
        .sample_rate = (uint32_t)sampleRate,
        .decimation_rate = 64, // Example decimation rate, adjust as needed
        .mode = channels == 1 ? CYHAL_PDM_PCM_MODE_LEFT : CYHAL_PDM_PCM_MODE_STEREO,
        .word_length = 16, // Example word length, adjust as needed
        .left_gain = 0, // Default gain, adjust as needed
        .right_gain = 0 // Default gain, adjust as needed
    };

    _clockInit();

    cyhal_pdm_pcm_init(&_pdm_pcm, mapping_gpio_pin[_pdmDataPin], mapping_gpio_pin[_pdmClockPin], &_audioClock, &_pdm_pcm_cfg);
    cyhal_pdm_pcm_register_callback(&_pdm_pcm, _pdm_pcm_isr_handler, this);
    cyhal_pdm_pcm_enable_event(&_pdm_pcm, CYHAL_PDM_PCM_ASYNC_COMPLETE, 3, true);
    cyhal_pdm_pcm_start(&_pdm_pcm);
    
    // Start async read to trigger interrupts when data is ready
    cyhal_pdm_pcm_read_async(&_pdm_pcm, _audioDataBuffer, FRAME_SIZE);
    
    _initialized = true;
    return 1; // Return 1 on success
}

void PDMClassPSOC::end() {
    // Implementation to deinitialize PDM
    cyhal_pdm_pcm_abort_async(&_pdm_pcm);
    cyhal_pdm_pcm_stop(&_pdm_pcm);
    cyhal_pdm_pcm_clear(&_pdm_pcm);
    cyhal_pdm_pcm_free(&_pdm_pcm);

    _initialized = false;
    _bufferInitialized = false;
}

int PDMClassPSOC::available() {
    int availableBytes = Cy_PDM_PCM_GetNumInFifo(_pdm_pcm.base);
    return availableBytes;
}

int PDMClassPSOC::read(void* buffer, size_t size) {
    // Implementation to read audio samples into the provided buffer
    return 0; // Placeholder
}

void PDMClassPSOC::onReceive(void (*callback)(void)) {
    // Implementation to set the callback function for when new data is available
    _user_onReceive = callback;
}
void PDMClassPSOC::setGain(int gain) {
    
}

void PDMClassPSOC::setBufferSize(uint16_t bufferSize) {
    _audioDataBuffer = (int16_t *)malloc(sizeof(int16_t) * bufferSize);
    if (_audioDataBuffer != nullptr) {
        _bufferInitialized = true;
    }
}

void PDMClassPSOC::_clockInit(void) {
    /* Initialize the PLL*/
    cyhal_clock_reserve(&_pllCLock, &CYHAL_CLOCK_PLL[0]);
    cyhal_clock_set_frequency(&_pllCLock, AUDIO_SYS_CLOCK_HZ, NULL);
    cyhal_clock_set_enabled(&_pllCLock, true, true);

    /* Initialize the audio subsystem clock (CLK_HF(1))
     * The CLK_HF[1] is the root clock for the I2S and PDM/PCM blocks */
    cyhal_clock_reserve(&_audioClock, &CYHAL_CLOCK_HF[1]);

    /* Source the audio subsystem clock  from PLL*/
    cyhal_clock_set_source(&_audioClock, &_pllCLock);
    cyhal_clock_set_enabled(&_audioClock, true, true);
}

void PDMClassPSOC::_pdm_pcm_isr_handler(void *arg, cyhal_pdm_pcm_event_t event) {
    PDMClassPSOC *pdm = static_cast<PDMClassPSOC *>(arg);
    pdm->_pdm_pcm_event_handler_member(event);
}

void PDMClassPSOC::_pdm_pcm_event_handler_member(cyhal_pdm_pcm_event_t event) {
    if (event == CYHAL_PDM_PCM_ASYNC_COMPLETE) {
        onReceiveService();
    }
}

void PDMClassPSOC::onReceiveService(void) {
    if (_user_onReceive) {
        _user_onReceive();
    }
    // Restart async read for continuous operation
    cyhal_pdm_pcm_read_async(&_pdm_pcm, _audioDataBuffer, FRAME_SIZE);
}