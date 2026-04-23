/**
 * PDM Audio Raw Data Output
 * 
 * This sketch reads audio data from the PDM microphone and sends it
 * over serial as raw 16-bit samples for recording.
 * 
 * Use with pdm_to_wav.py Python script to record and save as WAV file.
 * 
 * Configuration:
 * - Sample rate: 16000 Hz
 * - Channels: 2 (stereo)
 * - Bit depth: 16-bit
 * - Baud rate: 1000000 (115200 is too slow for 16kHz/16-bit streaming)
 * - Frame size: 1024 samples
 * 
 * Note: This sketch only covers one of several possible configurations. Adjust parameters as needed,
 * but please aware that higher sample rates, longer word lengths, or bit depths may require faster baud rates and larger buffers to avoid data loss.
 * Also, ensure that your serial terminal or recording software can handle the chosen baud rate and data format.
 */

#include "PDM.h"

#define FRAME_SIZE 1024

PDMClassPSOC PDM = PDMClassPSOC(38, 37); // data=38, clock=37

volatile bool dataReady = false;

int32_t audioFrames[FRAME_SIZE] = {0};

void setup() {
  Serial.begin(1000000);
  
  // Wait for serial connection (optional, for debugging)
  while (!Serial) {
    delay(10);
  }
  
  PDM.setBufferSize(FRAME_SIZE);
  int returnValue = PDM.begin(2, 16000);  // 2 channels, 16000 Hz sample rate
  
  if (returnValue != 1) {
    Serial.println("ERROR: Failed to initialize PDM");
    while (1) {
      delay(1000);
    }
  }
  
  PDM.onReceive(callBackFunction);
}

void loop() {
  if (dataReady) {
    dataReady = false;

    int samplesRead = PDM.read(audioFrames, PDM.available());
    
    if (samplesRead > 0) {      
      for (int i = 0; i < samplesRead; i++) {
        int32_t sample = audioFrames[i];
        
        // Send as little-endian 16-bit
        Serial.write((uint8_t)(sample & 0xFF));         // Low byte
        Serial.write((uint8_t)((sample >> 8) & 0xFF));  // High byte
      }
    }
  }
}

void callBackFunction(void) {
  dataReady = true;
}
