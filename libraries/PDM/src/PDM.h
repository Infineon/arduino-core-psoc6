#ifndef PDM_H
#define PDM_H

#include "Arduino.h"
#include "cyhal_pdmpcm.h"

#define PDM_DATA   P10_5
#define PDM_CLK    P10_4

#define SAMPLE_RATE_HZ   8000u
#define DECIMATION_RATE  64u

class PdmClass {
public:
    bool begin(int channels, int sample_rate);
    void end();

private:
    cy_rslt_t pdm_status;
    cyhal_clock_t audio_clock;
    cyhal_pdm_pcm_t pdm_pcm;
};

extern PdmClass PDM;

#endif
