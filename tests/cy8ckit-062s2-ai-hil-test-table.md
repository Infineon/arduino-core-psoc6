# Pin Connections for HIL Test Setup Between 2 Boards

| Test Name     | Board 1 Pin   | Board 2 Pin   | Description                    |
|---------------|---------------|---------------|--------------------------------|
| UART          | P10.1         | P10.0         | UART TX to RX                  |
|               | P10.0         | P10.1         | UART RX to TX                  |
|               | P9.4          | P9.4          | IO synch signal                |
|               | GND           | GND           | Common Ground                  |
|               |               |               |                                |
| Wire 1        | P0.2          | P0.2          | I2C SCL to SCL (with pullup)   |
|               | P0.3          | P0.3          | I2C SDA to SDA (with pullup)   |
|               | GND           | GND           | Common Ground                  |
|               |               |               |                                |
| SPI           | P9_0          | P9_0          | MOSI to MOSI                   |
|               | P9_1          | P9_1          | MISO to MISO                   |
|               | P9_2          | P9_2          | SCLK to SCLK                   |
|               | P9_3 (IO_0)   | P9_3 (IO_0)   | GPIO pin as SPI SSEL           |
|               | P9.4          | P9.4          | IO synch signal                |
|               | GND           | GND           | Common Ground                  |
|               |               |               |                                |

# Pin Connections for HIL Test Setup on Single Board

| Test Name     | Board 1 Pin   | Board 1 Pin    | Description                    |
|---------------|---------------|----------------|--------------------------------|
| Digital IO    | P9.6          | P9.7           | IO6 to IO7                     |
|               |               |                |                                |
| Wire 2        | P0.2          | P9.0           | I2C SCL to SCL (with pullup)   |
|               | P0.3          | P9.1           | I2C SDA TO SDA (with pullup)   |
|               |               |                |                                |
| SPI           | P9_0          | P9_1           | MOSI and MISO shorted          |
|               | P9_2          | NC             | SCLK to SCLK                   |
|               |               |                |                                |
| Analog IO ADC | P10.0         | VDDA 3.3V      | A0 connected to VDDA 3.3V      |
|               | P10.1         | VoltageDivider | A1 connected to Voltage        |
|               |               |                | divider circuit as below:      |
|               |               |                |                                |
|               |               |                |  VDDA (3.3V)                   |
|               |               |                |   :                            |
|               |               |                |  [R1]   <-- Resistor 220 Ohms  |
|               |               |                |   :                            |
|               |               |                |   +-------> A1 (Mid-Point)     |
|               |               |                |   :                            |
|               |               |                |  [R2]   <-- Resistor 220 Ohms  |
|               |               |                |   :                            |
|               |               |                |  GND (0V)                      |
|               |               |                |                                |
| Analog IO PWM | P9.6          | P9.7           | IO6 (PWM feedback) to          |
|               |               |                |       IO7 (PWM Output)         |
|               |               |                |                                |
