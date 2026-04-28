/**
 * PDM Hardware Diagnostic Test
 * Tests PDM microphone initialization, data capture, and API functions
 * on CY8CKIT-062S2-AI board.
 * Outputs human-readable results via serial at 115200 baud.
 */
#include "PDM.h"

#define SERIAL_BAUD   115200
#define FRAME_SIZE    512
#define SAMPLE_RATE   16000
#define NUM_CHANNELS  1

// PDM pins for CY8CKIT-062S2-AI: data=P10_5(38), clock=P10_4(37)
PDMClassPSOC PDM_mic = PDMClassPSOC(38, 37);

int32_t audioBuffer[FRAME_SIZE] = {0};
volatile bool dataReady = false;
volatile int callbackCount = 0;

void pdmCallback(void) {
    dataReady = true;
    callbackCount++;
}

void printHeader(const char *testName) {
    Serial.println();
    Serial.print("=== TEST: ");
    Serial.print(testName);
    Serial.println(" ===");
}

void printResult(const char *testName, bool passed) {
    Serial.print("[");
    Serial.print(passed ? "PASS" : "FAIL");
    Serial.print("] ");
    Serial.println(testName);
}

// ---------- Test 1: Basic Initialization (Mono) ----------
bool testInitMono() {
    printHeader("Init Mono 16kHz");
    PDM_mic.setBufferSize(FRAME_SIZE);
    int result = PDM_mic.begin(1, 16000);
    Serial.print("  begin(1, 16000) returned: ");
    Serial.println(result);
    if (result == 1) {
        PDM_mic.end();
    }
    return (result == 1);
}

// ---------- Test 2: Basic Initialization (Stereo) ----------
bool testInitStereo() {
    printHeader("Init Stereo 16kHz");
    PDM_mic.setBufferSize(FRAME_SIZE);
    int result = PDM_mic.begin(2, 16000);
    Serial.print("  begin(2, 16000) returned: ");
    Serial.println(result);
    if (result == 1) {
        PDM_mic.end();
    }
    return (result == 1);
}

// ---------- Test 3: Data Capture ----------
bool testDataCapture() {
    printHeader("Data Capture");

    PDM_mic.setBufferSize(FRAME_SIZE);
    int result = PDM_mic.begin(NUM_CHANNELS, SAMPLE_RATE);
    if (result != 1) {
        Serial.println("  ERROR: Failed to initialize PDM");
        return false;
    }

    PDM_mic.onReceive(pdmCallback);
    dataReady = false;
    callbackCount = 0;

    // Wait for data (up to 3 seconds)
    unsigned long startTime = millis();
    while (!dataReady && (millis() - startTime < 3000)) {
        delay(10);
    }

    if (!dataReady) {
        Serial.println("  ERROR: No callback received within 3s");
        PDM_mic.end();
        return false;
    }

    Serial.print("  Callback received after ");
    Serial.print(millis() - startTime);
    Serial.println(" ms");

    int bytesAvailable = PDM_mic.available();
    Serial.print("  Bytes available: ");
    Serial.println(bytesAvailable);

    if (bytesAvailable <= 0) {
        Serial.println("  ERROR: No bytes available after callback");
        PDM_mic.end();
        return false;
    }

    int samplesRead = PDM_mic.read(audioBuffer, bytesAvailable);
    Serial.print("  Samples read: ");
    Serial.println(samplesRead);

    PDM_mic.end();
    return (samplesRead > 0);
}

// ---------- Test 4: Audio Signal Quality ----------
bool testAudioSignalQuality() {
    printHeader("Audio Signal Quality");

    PDM_mic.setBufferSize(FRAME_SIZE);
    int result = PDM_mic.begin(NUM_CHANNELS, SAMPLE_RATE);
    if (result != 1) {
        Serial.println("  ERROR: Failed to initialize PDM");
        return false;
    }

    PDM_mic.onReceive(pdmCallback);
    dataReady = false;

    unsigned long startTime = millis();
    while (!dataReady && (millis() - startTime < 3000)) {
        delay(10);
    }

    if (!dataReady) {
        Serial.println("  ERROR: No data received");
        PDM_mic.end();
        return false;
    }

    int bytesAvail = PDM_mic.available();
    int samplesRead = PDM_mic.read(audioBuffer, bytesAvail);

    if (samplesRead <= 0) {
        Serial.println("  ERROR: No samples read");
        PDM_mic.end();
        return false;
    }

    int32_t minVal = audioBuffer[0], maxVal = audioBuffer[0];
    int64_t sum = 0;
    int nonZeroCount = 0;

    for (int i = 0; i < samplesRead; i++) {
        int32_t sample = audioBuffer[i];
        if (sample < minVal) minVal = sample;
        if (sample > maxVal) maxVal = sample;
        sum += sample;
        if (sample != 0) nonZeroCount++;
    }

    int32_t mean = (int32_t)(sum / samplesRead);
    int32_t peakToPeak = maxVal - minVal;

    Serial.print("  Samples analyzed: ");
    Serial.println(samplesRead);
    Serial.print("  Min value: ");
    Serial.println(minVal);
    Serial.print("  Max value: ");
    Serial.println(maxVal);
    Serial.print("  Peak-to-peak: ");
    Serial.println(peakToPeak);
    Serial.print("  Mean: ");
    Serial.println(mean);
    Serial.print("  Non-zero samples: ");
    Serial.print(nonZeroCount);
    Serial.print(" / ");
    Serial.println(samplesRead);

    PDM_mic.end();

    bool hasVariation = (peakToPeak > 0);
    bool hasNonZero = (nonZeroCount > samplesRead / 4);

    if (!hasVariation) Serial.println("  WARNING: No signal variation detected");
    if (!hasNonZero) Serial.println("  WARNING: Too many zero samples");

    return hasVariation && hasNonZero;
}

// ---------- Test 5: Multiple Callbacks ----------
bool testMultipleCallbacks() {
    printHeader("Multiple Callbacks (1s capture)");

    PDM_mic.setBufferSize(FRAME_SIZE);
    int result = PDM_mic.begin(NUM_CHANNELS, SAMPLE_RATE);
    if (result != 1) {
        Serial.println("  ERROR: Failed to initialize PDM");
        return false;
    }

    PDM_mic.onReceive(pdmCallback);
    callbackCount = 0;
    dataReady = false;

    unsigned long startTime = millis();
    int totalSamplesRead = 0;

    while (millis() - startTime < 1000) {
        if (dataReady) {
            dataReady = false;
            int bytesAvail = PDM_mic.available();
            if (bytesAvail > 0) {
                totalSamplesRead += PDM_mic.read(audioBuffer, bytesAvail);
            }
        }
        delay(1);
    }

    Serial.print("  Callbacks received: ");
    Serial.println(callbackCount);
    Serial.print("  Total samples read: ");
    Serial.println(totalSamplesRead);

    PDM_mic.end();

    return (callbackCount >= 5 && totalSamplesRead > 0);
}

// ---------- Test 6: Set Gain ----------
bool testSetGain() {
    printHeader("Set Gain");

    PDM_mic.setBufferSize(FRAME_SIZE);
    int result = PDM_mic.begin(NUM_CHANNELS, SAMPLE_RATE);
    Serial.print("  begin returned: ");
    Serial.println(result);
    Serial.flush();
    if (result != 1) {
        Serial.println("  ERROR: Failed to initialize PDM");
        return false;
    }

    Serial.println("  Calling setGain(10)...");
    Serial.flush();
    uint32_t gainResult = PDM_mic.setGain(10);
    Serial.print("  setGain(10) returned: ");
    Serial.println(gainResult);
    Serial.flush();

    Serial.println("  Calling setGain(20)...");
    Serial.flush();
    uint32_t gainResult2 = PDM_mic.setGain(20);
    Serial.print("  setGain(20) returned: ");
    Serial.println(gainResult2);
    Serial.flush();

    Serial.println("  Calling end()...");
    Serial.flush();
    PDM_mic.end();
    Serial.println("  end() completed");
    Serial.flush();

    return (gainResult == 0 && gainResult2 == 0);
}

// ---------- Test 7: End and Reinit ----------
bool testEndAndReinit() {
    printHeader("End and Reinitialize");

    PDM_mic.setBufferSize(FRAME_SIZE);
    int r1 = PDM_mic.begin(NUM_CHANNELS, SAMPLE_RATE);
    Serial.print("  First begin() returned: ");
    Serial.println(r1);
    PDM_mic.end();
    Serial.println("  end() called");

    PDM_mic.setBufferSize(FRAME_SIZE);
    int r2 = PDM_mic.begin(NUM_CHANNELS, SAMPLE_RATE);
    Serial.print("  Second begin() returned: ");
    Serial.println(r2);

    if (r2 == 1) {
        PDM_mic.onReceive(pdmCallback);
        dataReady = false;
        unsigned long startTime = millis();
        while (!dataReady && (millis() - startTime < 3000)) {
            delay(10);
        }
        Serial.print("  Data after reinit: ");
        Serial.println(dataReady ? "YES" : "NO");
        PDM_mic.end();
    }

    return (r1 == 1 && r2 == 1 && dataReady);
}

// ---------- Main ----------
void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(2000);

    Serial.println();
    Serial.println("######################################");
    Serial.println("# PDM Hardware Diagnostic Test Suite #");
    Serial.println("# Board: CY8CKIT-062S2-AI           #");
    Serial.println("######################################");
    Serial.println();

    int passCount = 0;
    int totalTests = 7;

    bool results[7];
    results[0] = testInitMono();
    printResult("Init Mono", results[0]);

    results[1] = testInitStereo();
    printResult("Init Stereo", results[1]);

    results[2] = testDataCapture();
    printResult("Data Capture", results[2]);

    results[3] = testAudioSignalQuality();
    printResult("Audio Signal Quality", results[3]);

    results[4] = testMultipleCallbacks();
    printResult("Multiple Callbacks", results[4]);

    results[5] = testSetGain();
    printResult("Set Gain", results[5]);

    results[6] = testEndAndReinit();
    printResult("End and Reinit", results[6]);

    for (int i = 0; i < totalTests; i++) {
        if (results[i]) passCount++;
    }

    Serial.println();
    Serial.println("======================================");
    Serial.print("RESULTS: ");
    Serial.print(passCount);
    Serial.print(" / ");
    Serial.print(totalTests);
    Serial.println(" tests passed");
    Serial.println("======================================");

    if (passCount == totalTests) {
        Serial.println("*** ALL TESTS PASSED ***");
    } else {
        Serial.println("*** SOME TESTS FAILED ***");
    }
}

void loop() {
    delay(10000);
}
