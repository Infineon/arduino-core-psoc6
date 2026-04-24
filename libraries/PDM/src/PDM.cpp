#include "PDM.h"

PDMClassPSOC::PDMClassPSOC(pin_size_t pdmClockPin, pin_size_t pdmDataPin)
    : _pdmClockPin(pdmClockPin),
    _pdmDataPin(pdmDataPin),
    _ringBuffer(nullptr),
    _ringHead(0),
    _ringTail(0),
    _ringCapacity(0),
    _user_onReceive(nullptr),
    _initialized(false),
    _bufferInitialized(false) {
}

PDMClassPSOC::~PDMClassPSOC() {
    end();
}

int PDMClassPSOC::begin(int channels, int sampleRate) {
    cy_rslt_t result = 0;

    if (!_bufferInitialized) {
        return -1;
    }
    if (_initialized) {
        return 0;
    }

    _pdm_pcm_cfg = {
        .sample_rate = (uint32_t)sampleRate,
        .decimation_rate = 64,
        .mode = channels == 1 ? CYHAL_PDM_PCM_MODE_LEFT : CYHAL_PDM_PCM_MODE_STEREO,
        .word_length = 16,
        .left_gain = 20,
        .right_gain = 20
    };

    _clockInit();

    result = cyhal_pdm_pcm_init(&_pdm_pcm, mapping_gpio_pin[_pdmDataPin], mapping_gpio_pin[_pdmClockPin], &_audioClock, &_pdm_pcm_cfg);
    if (result != CY_RSLT_SUCCESS) {
        return -1;
    }
    cyhal_pdm_pcm_register_callback(&_pdm_pcm, _pdm_pcm_isr_handler, this);
    cyhal_pdm_pcm_enable_event(&_pdm_pcm, CYHAL_PDM_PCM_ASYNC_COMPLETE, CYHAL_ISR_PRIORITY_DEFAULT, true);
    result = cyhal_pdm_pcm_start(&_pdm_pcm);
    if (result != CY_RSLT_SUCCESS) {
        return -1;
    }
    result = cyhal_pdm_pcm_read_async(&_pdm_pcm, _dmaBuffer, SIZEOF_DMA_BUFFER);
    if (result != CY_RSLT_SUCCESS) {
        return -1;
    }

    _initialized = true;
    return 1;
}

void PDMClassPSOC::end() {
    if (!_initialized) {
        return;
    }
    cyhal_pdm_pcm_abort_async(&_pdm_pcm);
    cyhal_pdm_pcm_stop(&_pdm_pcm);
    cyhal_pdm_pcm_clear(&_pdm_pcm);
    cyhal_pdm_pcm_free(&_pdm_pcm);

    if (_ringBuffer != nullptr) {
        free(_ringBuffer);
        _ringBuffer = nullptr;
    }
    _ringHead = 0;
    _ringTail = 0;
    _ringCapacity = 0;

    _initialized = false;
    _bufferInitialized = false;
}

int PDMClassPSOC::available() {
    return _ringAvailable();
}

uint32_t PDMClassPSOC::read(void *buffer, size_t size) {
    if (buffer == nullptr) {
        return 0;
    }

    size_t samplesToRead = size;
    size_t samplesRead = _ringRead((int32_t *)buffer, samplesToRead);

    return samplesRead;
}

void PDMClassPSOC::onReceive(void (*callback)(void)) {
    _user_onReceive = callback;
}

uint32_t PDMClassPSOC::setGain(int16_t gain) {
    cy_rslt_t result = 0;
    result = cyhal_pdm_pcm_set_gain(&_pdm_pcm, gain, gain);
    return result;
}

void PDMClassPSOC::setBufferSize(uint16_t bufferSize) {
    if (_ringBuffer != nullptr) {
        free(_ringBuffer);
        _ringBuffer = nullptr;
    }

    _ringCapacity = bufferSize + 1;
    _ringBuffer = (int32_t *)malloc(sizeof(int32_t) * _ringCapacity);

    if (_ringBuffer != nullptr) {
        _ringHead = 0;
        _ringTail = 0;
        _bufferInitialized = true;
    } else {
        _ringCapacity = 0;
        _bufferInitialized = false;
    }
}

size_t PDMClassPSOC::_ringAvailable() const {
    size_t head = _ringHead;
    size_t tail = _ringTail;
    if (head >= tail) {
        return head - tail;
    } else {
        return _ringCapacity - tail + head;
    }
}

size_t PDMClassPSOC::_ringFree() const {
    return _ringCapacity - _ringAvailable() - 1;
}

size_t PDMClassPSOC::_ringWrite(const int32_t *data, size_t count) {
    size_t freeSpace = _ringFree();
    if (count > freeSpace) {
        count = freeSpace;
    }

    for (size_t i = 0; i < count; i++) {
        _ringBuffer[_ringHead] = data[i];
        _ringHead = (_ringHead + 1) % _ringCapacity;
    }

    return count;
}

size_t PDMClassPSOC::_ringRead(int32_t *data, size_t count) {
    size_t avail = _ringAvailable();
    if (count > avail) {
        count = avail;
    }

    for (size_t i = 0; i < count; i++) {
        data[i] = _ringBuffer[_ringTail];
        _ringTail = (_ringTail + 1) % _ringCapacity;
    }

    return count;
}

void PDMClassPSOC::_clockInit(void) {
    cyhal_clock_reserve(&_pllClock, &CYHAL_CLOCK_PLL[0]);
    cyhal_clock_set_frequency(&_pllClock, AUDIO_SYS_CLOCK_HZ, NULL);
    cyhal_clock_set_enabled(&_pllClock, true, true);

    cyhal_clock_reserve(&_audioClock, &CYHAL_CLOCK_HF[1]);

    cyhal_clock_set_source(&_audioClock, &_pllClock);
    cyhal_clock_set_enabled(&_audioClock, true, true);
}

void PDMClassPSOC::_pdm_pcm_isr_handler(void *arg, cyhal_pdm_pcm_event_t event) {
    PDMClassPSOC *pdm = static_cast < PDMClassPSOC * > (arg);
    pdm->_pdm_pcm_event_handler_member(event);
}

void PDMClassPSOC::_pdm_pcm_event_handler_member(cyhal_pdm_pcm_event_t event) {
    if (event == CYHAL_PDM_PCM_ASYNC_COMPLETE) {
        onReceiveService();
    }
}

void PDMClassPSOC::onReceiveService(void) {
    if (_pdm_pcm_cfg.word_length <= 16) {
        int16_t *src = (int16_t *)_dmaBuffer;
        for (int i = SIZEOF_DMA_BUFFER - 1; i >= 0; i--) {
            _dmaBuffer[i] = (int32_t)src[i];
        }
    }

    _ringWrite(_dmaBuffer, SIZEOF_DMA_BUFFER);

    if (_user_onReceive) {
        _user_onReceive();
    }

    cyhal_pdm_pcm_read_async(&_pdm_pcm, _dmaBuffer, SIZEOF_DMA_BUFFER);
}

#if (PDM_HOWMANY > 0)
PDMClassPSOC PDM(PDM_CLK_PIN, PDM_DATA_PIN);
#endif
