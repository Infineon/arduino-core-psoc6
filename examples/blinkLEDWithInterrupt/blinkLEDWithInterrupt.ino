volatile bool ledState = HIGH;

void toggleLED() {
    ledState = !ledState; 
    digitalWrite(LED_BUILTIN, ledState);
}


void setup() {
    Serial.begin(112500);
    Serial.println("LED toggles with USER_BUTTON interrupt at Falling edge!");

    pinMode(USER_BUTTON, INPUT_PULLUP); 
    pinMode(LED_BUILTIN, OUTPUT); 

    // Attach interrupt to the button pin
    attachInterrupt(USER_BUTTON, toggleLED, FALLING); 
}

void loop() {
    // Do nothing
}
