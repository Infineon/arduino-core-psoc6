# Pin Connections for HIL Test Setup Between 2 Boards

| Test Name         | Board 1 Pin   | Board 2 Pin   | Description                    |
|-------------------|---------------|---------------|--------------------------------|
| UART              | P10_1         | P10_0         | UART TX to RX                  |
|                   | P10_0         | P10_1         | UART RX to TX                  |
|                   | P9_4          | P9_4          | IO synch signal                |
|                   | GND           | GND           | Common Ground                  |
|                   |               |               |                                |
| Wire 1            | P0_2          | P0_2          | I2C SCL to SCL (with pullup)   |
|                   | P0_3          | P0_3          | I2C SDA to SDA (with pullup)   |
|                   | GND           | GND           | Common Ground                  |
|                   |               |               |                                |
| SPI               | P9_0          | P9_0          | MOSI to MOSI                   |
|                   | P9_1          | P9_1          | MISO to MISO                   |
|                   | P9_2          | P9_2          | SCLK to SCLK                   |
|                   | P9_3          | P9_3          | GPIO pin as SPI SSEL           |
|                   | P9_4          | P9_4          | IO synch signal                |
|                   | GND           | GND           | Common Ground                  |
|                   |               |               |                                |
| Advanced IO Pulse | P9_5          | P9_5          | IO5 (Pulse Output) to          |
|                   |               |               |       IO5 (Pulse Input)        |
|                   | P9_4          | P9_4          | IO synch signal                |
|                   |               |               |                                |


# Pin Connections for HIL Test Setup Between Board and Sensor

| Test Name         | Board Pin     | Sensor Pin        | Description                                     |
|-------------------|---------------|-------------------|-------------------------------------------------|
| One Wire DS18x20  | GND           | GND (DS18B20)     | Connects to the ground of the circuit           |
|                   | 3.3V          | VDD (DS18B20)     | Powers the sensor                               |
|                   | P9_0          | DQ Data (DS18B20) | Data lines (with 4.7kΩ pull-up resistor to VDD) |

# Pin Connections for HIL Test Setup on Single Board

| Test Name         | Board 1 Pin   | Board 1 Pin    | Description                    |
|-------------------|---------------|----------------|--------------------------------|
| Digital IO        | P9_6          | P9_7           | IO6 to IO7                     |
|                   |               |                |                                |
| Wire 2            | P0_2          | P9_0           | I2C SCL to SCL (with pullup)   |
|                   | P0_3          | P9_1           | I2C SDA TO SDA (with pullup)   |
|                   |               |                |                                |
| SPI               | P9_0          | P9_1           | MOSI and MISO shorted          |
|                   | P9_2          | NC             | SCLK to SCLK                   |
|                   |               |                |                                |
| Analog IO ADC     | P10_0         | VDDA 3.3V      | A0 connected to VDDA 3.3V      |
|                   | P10_1         | VoltageDivider | A1 connected to Voltage        |
|                   |               |                | divider circuit as below:      |
|                   |               |                |                                |
|                   |               |                |  VDDA (3.3V)                   |
|                   |               |                |   :                            |
|                   |               |                |  [R1]   <-- Resistor 220 Ohms  |
|                   |               |                |   :                            |
|                   |               |                |   +-------> A1 (Mid-Point)     |
|                   |               |                |   :                            |
|                   |               |                |  [R2]   <-- Resistor 220 Ohms  |
|                   |               |                |   :                            |
|                   |               |                |  GND (0V)                      |
|                   |               |                |                                |
| Analog IO PWM     | P9_6          | P9_7           | IO6 (PWM feedback) to          |
|                   |               |                |       IO7 (PWM Output)         |
|                   |               |                |                                |
| Advanced IO Tone  | P9_6          | P9_7           | IO6 (Tone feedback) to         |
|                   |               |                |       IO7 (Tone Output)        |
|                   |               |                |                                |
