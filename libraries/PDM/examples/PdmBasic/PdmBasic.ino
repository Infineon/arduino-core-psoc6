#include "PDM.h"

void setup()
{
    Serial.begin(115200);
    if (!PDM.begin(2, 16000)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }

    Serial.println("PDM Initialized!");


  PDM.end();
}

void loop()
{
    // Main loop does nothing, all handled via ISR and callback.
}
