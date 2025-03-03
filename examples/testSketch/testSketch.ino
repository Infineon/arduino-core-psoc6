

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(112500);
  Serial.println("LED toggles first with 1 second delay and next with 0.5 seconds delay!");
  
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on
  delay(1000); // Delay for 1000 milliseconds (1 second)
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off
  delay(1000); // Delay for 1000 milliseconds (1 second)

  // Toggle the LED using delayMicroseconds
  digitalWrite(LED_BUILTIN, HIGH);
  delayMicroseconds(500000); // Delay for 500000 microseconds (0.5 seconds)
  digitalWrite(LED_BUILTIN, LOW);
  delayMicroseconds(500000); // Delay for 500000 microseconds (0.5 seconds)
}
