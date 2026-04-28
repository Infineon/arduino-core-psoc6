/**
 * PDM Comprehensive Hardware Test Suite
 * Tests: reinit, sample rates, buffer sizes, negative gain, stereo, stability
 *
 * Run after pdm_diagnostic for extended coverage.
 */
#include "PDM.h"

#define SERIAL_BAUD   115200
#define DEFAULT_RATE  16000
#define DEFAULT_BUF   512

PDMClassPSOC PDM_mic = PDMClassPSOC(38, 37);

int32_t audioBuffer[2048] = {0};
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
    Serial.flush();
}

void printResult(const char *testName, bool passed) {
    Serial.print("[");
    Serial.print(passed ? "PASS" : "FAIL");
    Serial.print("] ");
    Serial.println(testName);
    Serial.flush();
}

bool waitForData(int timeoutMs) {
    dataReady = false;
    unsigned long start = millis();
    while (!dataReady && (millis() - start < (unsigned long)timeoutMs)) {
        delay(1);
    }
    return dataReady;
}

// ====== Test: End and Reinitialize ======
bool testReinit() {
    printHeader("End and Reinitialize");

    PDM_mic.setBufferSize(DEFAULT_BUF);
    int r1 = PDM_mic.begin(1, DEFAULT_RATE);
    Serial.print("  1st begin: "); Serial.println(r1); Serial.flush();
    if (r1 != 1) return false;

    PDM_mic.onReceive(pdmCallback);
    bool got1 = waitForData(3000);
    Serial.print("  1st data: "); Serial.println(got1 ? "YES" : "NO"); Serial.flush();
    PDM_mic.end();
    Serial.println("  end() OK"); Serial.flush();

    delay(500);

    PDM_mic.setBufferSize(DEFAULT_BUF);
    int r2 = PDM_mic.begin(1, DEFAULT_RATE);
    Serial.print("  2nd begin: "); Serial.println(r2); Serial.flush();
    if (r2 != 1) return false;

    PDM_mic.onReceive(pdmCallback);
    bool got2 = waitForData(3000);
    Serial.print("  2nd data: "); Serial.println(got2 ? "YES" : "NO"); Serial.flush();

    int avail = PDM_mic.available();
    int read = PDM_mic.read(audioBuffer, avail);
    Serial.print("  2nd samples: "); Serial.println(read); Serial.flush();

    PDM_mic.end();
    return (r1 == 1 && r2 == 1 && got1 && got2 && read > 0);
}

// ====== Test: Different Sample Rates ======
bool testSampleRate(int rate) {
    Serial.print("  Rate "); Serial.print(rate); Serial.print("Hz: ");
    Serial.flush();

    PDM_mic.setBufferSize(DEFAULT_BUF);
    int r = PDM_mic.begin(1, rate);
    if (r != 1) {
        Serial.println("begin FAILED");
        Serial.flush();
        return false;
    }

    PDM_mic.onReceive(pdmCallback);
    bool got = waitForData(3000);
    if (!got) {
        Serial.println("no data");
        Serial.flush();
        PDM_mic.end();
        return false;
    }

    callbackCount = 0;
    int totalSamples = 0;
    unsigned long start = millis();
    while (millis() - start < 500) {
        if (dataReady) {
            dataReady = false;
            int a = PDM_mic.available();
            if (a > 0) totalSamples += PDM_mic.read(audioBuffer, a);
        }
        delay(1);
    }

    float effectiveRate = totalSamples * 2.0;
    Serial.print("OK, ~"); Serial.print((int)effectiveRate);
    Serial.print(" samples/s ("); Serial.print(callbackCount*2);
    Serial.println(" callbacks/s)");
    Serial.flush();

    PDM_mic.end();
    return true;
}

bool testSampleRates() {
    printHeader("Sample Rates");
    int rates[] = {8000, 16000, 22050, 44100, 48000};
    bool allOk = true;
    for (int i = 0; i < 5; i++) {
        bool ok = testSampleRate(rates[i]);
        if (!ok) allOk = false;
        delay(300);
    }
    return allOk;
}

// ====== Test: Buffer Sizes ======
bool testBufferSize(uint16_t bufSize) {
    Serial.print("  Buffer "); Serial.print(bufSize); Serial.print(": ");
    Serial.flush();

    PDM_mic.setBufferSize(bufSize);
    int r = PDM_mic.begin(1, DEFAULT_RATE);
    if (r != 1) {
        Serial.println("begin FAILED");
        Serial.flush();
        return false;
    }

    PDM_mic.onReceive(pdmCallback);
    bool got = waitForData(3000);
    if (!got) {
        Serial.println("no data");
        Serial.flush();
        PDM_mic.end();
        return false;
    }

    int avail = PDM_mic.available();
    int read = PDM_mic.read(audioBuffer, avail);
    Serial.print("OK, avail="); Serial.print(avail);
    Serial.print(", read="); Serial.println(read);
    Serial.flush();

    PDM_mic.end();
    return (read > 0);
}

bool testBufferSizes() {
    printHeader("Buffer Sizes");
    uint16_t sizes[] = {128, 256, 512, 1024, 2048};
    bool allOk = true;
    for (int i = 0; i < 5; i++) {
        bool ok = testBufferSize(sizes[i]);
        if (!ok) allOk = false;
        delay(300);
    }
    return allOk;
}

// ====== Test: Negative Gain Values ======
bool testNegativeGains() {
    printHeader("Negative Gain Values");
    bool allOk = true;
    int16_t gains[] = {0, -1, -5, -10, -15, -20, -21};
    int numGains = sizeof(gains) / sizeof(gains[0]);

    for (int i = 0; i < numGains; i++) {
        PDM_mic.setBufferSize(DEFAULT_BUF);
        int r = PDM_mic.begin(1, DEFAULT_RATE);
        if (r != 1) {
            Serial.print("  begin FAILED for gain "); Serial.println(gains[i]);
            allOk = false;
            continue;
        }

        Serial.print("  setGain("); Serial.print(gains[i]); Serial.print("): ");
        Serial.flush();

        uint32_t gr = PDM_mic.setGain(gains[i]);
        Serial.print("returned "); Serial.println(gr);
        Serial.flush();

        PDM_mic.end();
        delay(200);

        if (gr != 0) allOk = false;
    }
    return allOk;
}

// ====== Test: Stereo Capture ======
bool testStereoCapture() {
    printHeader("Stereo Data Capture");

    PDM_mic.setBufferSize(1024);
    int r = PDM_mic.begin(2, DEFAULT_RATE);
    Serial.print("  begin(2, 16000): "); Serial.println(r); Serial.flush();
    if (r != 1) return false;

    PDM_mic.onReceive(pdmCallback);
    bool got = waitForData(3000);
    if (!got) {
        Serial.println("  No data received");
        PDM_mic.end();
        return false;
    }

    delay(200);
    int avail = PDM_mic.available();
    int read = PDM_mic.read(audioBuffer, avail);
    Serial.print("  Samples read: "); Serial.println(read); Serial.flush();

    if (read < 4) {
        PDM_mic.end();
        return false;
    }

    int32_t leftMin = audioBuffer[0], leftMax = audioBuffer[0];
    int32_t rightMin = audioBuffer[1], rightMax = audioBuffer[1];
    int leftNonZero = 0, rightNonZero = 0;

    for (int i = 0; i < read; i += 2) {
        if (audioBuffer[i] != 0) leftNonZero++;
        if (audioBuffer[i] < leftMin) leftMin = audioBuffer[i];
        if (audioBuffer[i] > leftMax) leftMax = audioBuffer[i];
        if (i + 1 < read) {
            if (audioBuffer[i+1] != 0) rightNonZero++;
            if (audioBuffer[i+1] < rightMin) rightMin = audioBuffer[i+1];
            if (audioBuffer[i+1] > rightMax) rightMax = audioBuffer[i+1];
        }
    }

    Serial.print("  L: min="); Serial.print(leftMin);
    Serial.print(" max="); Serial.print(leftMax);
    Serial.print(" nonzero="); Serial.println(leftNonZero);

    Serial.print("  R: min="); Serial.print(rightMin);
    Serial.print(" max="); Serial.print(rightMax);
    Serial.print(" nonzero="); Serial.println(rightNonZero);
    Serial.flush();

    PDM_mic.end();

    bool leftOk = (leftMax - leftMin) > 0;
    return leftOk;
}

// ====== Test: Long Duration Stability ======
bool testLongDuration() {
    printHeader("Long Duration (5s)");

    PDM_mic.setBufferSize(1024);
    int r = PDM_mic.begin(1, DEFAULT_RATE);
    if (r != 1) {
        Serial.println("  begin FAILED");
        return false;
    }

    PDM_mic.onReceive(pdmCallback);
    callbackCount = 0;
    dataReady = false;

    unsigned long startTime = millis();
    int totalSamples = 0;
    int readErrors = 0;

    while (millis() - startTime < 5000) {
        if (dataReady) {
            dataReady = false;
            int avail = PDM_mic.available();
            if (avail > 0) {
                int read = PDM_mic.read(audioBuffer, min(avail, 2048));
                if (read > 0) {
                    totalSamples += read;
                } else {
                    readErrors++;
                }
            }
        }
        delay(1);
    }

    float elapsed = (millis() - startTime) / 1000.0;
    float sampleRate = totalSamples / elapsed;

    Serial.print("  Duration: "); Serial.print(elapsed, 1); Serial.println("s");
    Serial.print("  Total samples: "); Serial.println(totalSamples);
    Serial.print("  Effective rate: "); Serial.print((int)sampleRate); Serial.println(" Hz");
    Serial.print("  Callbacks: "); Serial.println(callbackCount);
    Serial.print("  Read errors: "); Serial.println(readErrors);
    Serial.flush();

    PDM_mic.end();

    bool rateOk = (sampleRate > 10000 && sampleRate < 20000);
    bool noErrors = (readErrors == 0);

    return rateOk && noErrors;
}

// ====== Main ======
void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(2000);

    Serial.println();
    Serial.println("##########################################");
    Serial.println("# PDM Comprehensive Test Suite           #");
    Serial.println("# Board: CY8CKIT-062S2-AI               #");
    Serial.println("##########################################");
    Serial.flush();

    int passCount = 0;
    int totalTests = 6;

    bool results[6];

    results[0] = testReinit();
    printResult("End and Reinitialize", results[0]);

    results[1] = testSampleRates();
    printResult("Sample Rates", results[1]);

    results[2] = testBufferSizes();
    printResult("Buffer Sizes", results[2]);

    results[3] = testNegativeGains();
    printResult("Negative Gains", results[3]);

    results[4] = testStereoCapture();
    printResult("Stereo Capture", results[4]);

    results[5] = testLongDuration();
    printResult("Long Duration Stability", results[5]);

    for (int i = 0; i < totalTests; i++) {
        if (results[i]) passCount++;
    }

    Serial.println();
    Serial.println("==========================================");
    Serial.print("RESULTS: ");
    Serial.print(passCount);
    Serial.print(" / ");
    Serial.print(totalTests);
    Serial.println(" tests passed");
    Serial.println("==========================================");

    if (passCount == totalTests) {
        Serial.println("*** ALL TESTS PASSED ***");
    } else {
        Serial.println("*** SOME TESTS FAILED ***");
    }
    Serial.flush();
}

void loop() {
    delay(10000);
}
