#include "PDM.h"

#define PDM_assert(cy_ret) if (cy_ret != CY_RSLT_SUCCESS) { \
            return; \
}

bool PdmClass::begin(int channels, int sample_rate) {
    // Set up config struct (non-const!)
    cyhal_pdm_pcm_cfg_t pdm_pcm_cfg = {
        .sample_rate = SAMPLE_RATE_HZ,
        .decimation_rate = DECIMATION_RATE,
        .mode = (channels == 2) ? CYHAL_PDM_PCM_MODE_STEREO : CYHAL_PDM_PCM_MODE_LEFT,
        .word_length = 16,
        .left_gain = 0,
        .right_gain = 0,
    };
    pdm_pcm_cfg.sample_rate = sample_rate;
    pdm_status = cyhal_pdm_pcm_init(&pdm_pcm, PDM_DATA, PDM_CLK, &audio_clock, &pdm_pcm_cfg);
    if (pdm_status != CY_RSLT_SUCCESS) {
        return false;
    }

    return true;
}

void PdmClass::end() {
    pdm_status = cyhal_pdm_pcm_stop(&pdm_pcm);
    PDM_assert(pdm_status);
    cyhal_pdm_pcm_free(&pdm_pcm);
}
