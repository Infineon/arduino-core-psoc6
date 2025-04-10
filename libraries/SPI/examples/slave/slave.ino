#include <SPI.h>

// Variable to store the received data
volatile uint8_t receivedData = 0;
volatile uint8_t dataToSend = 0;

// Create an instance of SPIClassPSOC for SPI communication as slave
static SPIClassPSOC SPI1 = SPIClassPSOC(MOSI, MISO, SCK, SS, true);

void setup() {
    // Start the SPI bus as a slave with default settings: (1 MHz, MSBFIRST, SPI_MODE0)
    SPI1.begin();
    Serial.begin(9600);

    // Optional: Set SPI settings
    SPI.beginTransaction(SPISettings(ARDUINO_SPI_CLOCK, MSBFIRST, SPI_MODE0));
    Serial.println("SPI Slave Started");
}

void loop() {
    receivedData = SPI1.transfer(dataToSend);

    Serial.print("Data Received: ");
    Serial.println(receivedData);

    dataToSend = receivedData + 1;

    delay(1000); 
}
