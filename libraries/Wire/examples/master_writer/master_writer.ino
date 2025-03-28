// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

int led = LED_BUILTIN;

void setup()
{
  pinMode(led, OUTPUT);
  Wire.begin(); // join i2c bus (address optional for master)
}

byte x = 0;

void loop()
{
  digitalWrite(led, HIGH);    // briefly flash the LED
  Wire.beginTransmission(9);  // transmit to device #9
  Wire.write("x is ");        // sends five bytes
  Wire.write(x);              // sends one byte
  Wire.endTransmission();     // stop transmitting
  digitalWrite(led, LOW);

  x++;
  delay(500);
}