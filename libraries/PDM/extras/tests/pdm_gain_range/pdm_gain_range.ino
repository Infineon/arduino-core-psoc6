/**
 * PDM Gain Range Boundary Test
 * Sweeps gain values to find the exact boundary where setGain fails or hangs.
 *
 * WARNING: This test will hang the MCU when it reaches gain >= 23.
 * The last printed gain value before the hang marks the boundary.
 *
 * Known result on CY8CKIT-062S2-AI:
 *   0-21  -> CY_RSLT_SUCCESS (0)
 *   22    -> HAL error 0x04018201
 *   >= 23 -> MCU hangs (cyhal_pdm_pcm_set_gain does not return)
 */
#include "PDM.h"

#define FRAME_SIZE   512
#define SAMPLE_RATE  16000

PDMClassPSOC PDM_mic = PDMClassPSOC(38, 37);

bool testGainValue(int16_t gain) {
    PDM_mic.setBufferSize(FRAME_SIZE);
    if (PDM_mic.begin(1, SAMPLE_RATE) != 1) {
        Serial.println("  FAIL: begin");
        return false;
    }

    Serial.print("  setGain("); Serial.print(gain); Serial.print(")...");
    Serial.flush();

    uint32_t r = PDM_mic.setGain(gain);
    Serial.print(" returned "); Serial.println(r);
    Serial.flush();

    PDM_mic.end();
    delay(200);
    return (r == 0);
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("=== PDM Gain Range Test ===");
    Serial.flush();

    int16_t testValues[] = {0, 10, 15, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
    int numTests = sizeof(testValues) / sizeof(testValues[0]);

    for (int i = 0; i < numTests; i++) {
        Serial.print("[Gain "); Serial.print(testValues[i]); Serial.println("]");
        Serial.flush();
        bool ok = testGainValue(testValues[i]);
        Serial.print("  Result: "); Serial.println(ok ? "PASS" : "FAIL");
        Serial.flush();
    }

    Serial.println("\n--- Negative gains ---");
    Serial.flush();
    int16_t negValues[] = {-1, -5, -10, -20, -21, -22};
    for (int i = 0; i < 6; i++) {
        Serial.print("[Gain "); Serial.print(negValues[i]); Serial.println("]");
        Serial.flush();
        bool ok = testGainValue(negValues[i]);
        Serial.print("  Result: "); Serial.println(ok ? "PASS" : "FAIL");
        Serial.flush();
    }

    Serial.println("\n=== GAIN RANGE TEST COMPLETE ===");
    Serial.flush();
}

void loop() {
    delay(10000);
}
