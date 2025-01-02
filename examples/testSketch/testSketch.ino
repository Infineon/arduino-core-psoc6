#include "cy_gpio.h" 

// Pin definition for the LED
#define LED_PORT GPIO_PRT5
#define LED_PIN 3

// the setup function runs once when you press reset or power the board
void setup() {

  Cy_GPIO_Pin_FastInit(LED_PORT, LED_PIN, CY_GPIO_DM_STRONG_IN_OFF, 0, HSIOM_SEL_GPIO);
}

// the loop function runs over and over again forever
void loop() {
  // Toggle the LED using delay
  Cy_GPIO_Write(LED_PORT, LED_PIN, 1);
  delay(1000); // Delay for 1000 milliseconds (1 second)
  Cy_GPIO_Write(LED_PORT, LED_PIN, 0);
  delay(1000); // Delay for 1000 milliseconds (1 second)

  // Toggle the LED using delayMicroseconds
  Cy_GPIO_Write(LED_PORT, LED_PIN, 1);
  delayMicroseconds(500000); // Delay for 500000 microseconds (0.5 seconds)
  Cy_GPIO_Write(LED_PORT, LED_PIN, 0);
  delayMicroseconds(500000); // Delay for 500000 microseconds (0.5 seconds)
}
