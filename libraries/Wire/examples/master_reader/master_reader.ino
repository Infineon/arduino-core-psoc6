// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

int led = LED_BUILTIN;

void setup()
{
  pinMode(led, OUTPUT);
  Wire.begin();             // join i2c bus (address optional for master)
  Serial.begin(9600);       // start serial for output
}

void loop()
{
  Serial.print("read: ");

  digitalWrite(led, HIGH);  // briefly flash the LED
  Wire.requestFrom(8, 6);   // request 6 bytes from slave device #8

  while(Wire.available()) { // slave may send less than requested
    char c = Wire.read();   // receive a byte as character
    Serial.print(c);        // print the character
  }

  Serial.println();
  digitalWrite(led, LOW);
  delay(500);
}