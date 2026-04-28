/**
 * PDM setGain Focused Test
 * Isolates whether hangs are due to gain value range or repeated calls.
 *
 * Known result: gain values 0-21 work, 22 returns HAL error, >=23 hangs MCU.
 */
#include "PDM.h"

#define FRAME_SIZE   512
#define SAMPLE_RATE  16000

PDMClassPSOC PDM_mic = PDMClassPSOC(38, 37);

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("=== PDM setGain Focused Test ===");
    Serial.flush();

    // Test 1: Single call with gain=20 (default from init)
    Serial.println("\n[Test 1] Single setGain(20) - same as init default");
    Serial.flush();
    PDM_mic.setBufferSize(FRAME_SIZE);
    if (PDM_mic.begin(1, SAMPLE_RATE) != 1) { Serial.println("FAIL: begin"); while(1); }
    Serial.println("  begin OK");
    Serial.flush();

    uint32_t r1 = PDM_mic.setGain(20);
    Serial.print("  setGain(20) returned: "); Serial.println(r1);
    Serial.flush();
    PDM_mic.end();
    Serial.println("  end OK");
    Serial.flush();

    delay(500);

    // Test 2: Single call with gain=10
    Serial.println("\n[Test 2] Single setGain(10)");
    Serial.flush();
    PDM_mic.setBufferSize(FRAME_SIZE);
    if (PDM_mic.begin(1, SAMPLE_RATE) != 1) { Serial.println("FAIL: begin"); while(1); }
    Serial.println("  begin OK");
    Serial.flush();

    uint32_t r2 = PDM_mic.setGain(10);
    Serial.print("  setGain(10) returned: "); Serial.println(r2);
    Serial.flush();
    PDM_mic.end();
    Serial.println("  end OK");
    Serial.flush();

    delay(500);

    // Test 3: Single call with gain=5
    Serial.println("\n[Test 3] Single setGain(5)");
    Serial.flush();
    PDM_mic.setBufferSize(FRAME_SIZE);
    if (PDM_mic.begin(1, SAMPLE_RATE) != 1) { Serial.println("FAIL: begin"); while(1); }
    Serial.println("  begin OK");
    Serial.flush();

    uint32_t r3 = PDM_mic.setGain(5);
    Serial.print("  setGain(5) returned: "); Serial.println(r3);
    Serial.flush();
    PDM_mic.end();
    Serial.println("  end OK");
    Serial.flush();

    delay(500);

    // Test 4: Two sequential calls in same session (10 then 20)
    Serial.println("\n[Test 4] Two sequential: setGain(10) then setGain(20)");
    Serial.flush();
    PDM_mic.setBufferSize(FRAME_SIZE);
    if (PDM_mic.begin(1, SAMPLE_RATE) != 1) { Serial.println("FAIL: begin"); while(1); }
    Serial.println("  begin OK");
    Serial.flush();

    uint32_t r4a = PDM_mic.setGain(10);
    Serial.print("  setGain(10) returned: "); Serial.println(r4a);
    Serial.flush();

    Serial.println("  Calling setGain(20)...");
    Serial.flush();

    uint32_t r4b = PDM_mic.setGain(20);
    Serial.print("  setGain(20) returned: "); Serial.println(r4b);
    Serial.flush();
    PDM_mic.end();
    Serial.println("  end OK");
    Serial.flush();

    Serial.println("\n=== ALL SETGAIN TESTS COMPLETED ===");
    Serial.flush();
}

void loop() {
    delay(10000);
}
