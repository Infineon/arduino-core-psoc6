# Pin Connections for HIL Test Setup Between 2 Boards

| Test Name         | Board 1 Pin   | Board 2 Pin   | Description                    |
|-------------------|---------------|---------------|--------------------------------|
| UART              | P10.1         | P10.0         | UART TX to RX                  |
|                   | P10.0         | P10.1         | UART RX to TX                  |
|                   | P10.2         | P10.2         | IO synch signal                |
|                   | GND           | GND           | Common Ground                  |
|                   |               |               |                                |
| Wire 1            | P6.4          | P6.4          | I2C SCL to SCL (with pullup)   |
|                   | P6.5          | P6.5          | I2C SDA to SDA (with pullup)   |
|                   | GND           | GND           | Common Ground                  |
|                   |               |               |                                |
| SPI               | P9_0          | P9_0          | MOSI to MOSI                   |
|                   | P9_1          | P9_1          | MISO to MISO                   |
|                   | P9_2          | P9_2          | SCLK to SCLK                   |
|                   | P9_3          | P9_3          | GPIO pin as SPI SSEL           |
|                   | P10.2         | P10.2         | IO synch signal                |
|                   | GND           | GND           | Common Ground                  |
|                   |               |               |                                |
| Advanced IO Pulse | P10.5         | P10.5         | IO5 (Pulse Output) to          |
|                   |               |               |       IO5 (Pulse Input)        |
|                   | P10.2         | P10.2         | IO synch signal                |
|                   |               |               |                                |


# Pin Connections for HIL Test Setup Between Board and Sensor

| Test Name         | Board Pin     | Sensor Pin        | Description                                     |
|-------------------|---------------|-------------------|-------------------------------------------------|
| One Wire DS18x20  | GND           | GND (DS18B20)     | Connects to the ground of the circuit           |
|                   | 3.3V          | VDD (DS18B20)     | Powers the sensor                               |
|                   | P9.0          | DQ Data (DS18B20) | Data lines (with 4.7kΩ pull-up resistor to VDD) |

# Pin Connections for HIL Test Setup on Single Board

| Test Name         | Board 1 Pin   | Board 1 Pin    | Description                    |
|-------------------|---------------|----------------|--------------------------------|
| Digital IO        | P12.6         | P12.7          | IO6 to IO7                     |
|                   |               |                |                                |
| Wire 2            | P6.4          | P10.0          | I2C SCL to SCL (with pullup)   |
|                   | P6.5          | P10.1          | I2C SDA TO SDA (with pullup)   |
|                   |               |                |                                |
| SPI               | P9_0          | P9_1           | MOSI and MISO shorted          |
|                   | P9_2          | NC             | SCLK to SCLK                   |
|                   |               |                |                                |
| Analog IO ADC     | P10.3         | VDDA 3.3V      | A0 connected to VDDA 3.3V      |
|                   | P10.4         | VoltageDivider | A1 connected to Voltage        |
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
| Analog IO PWM     | P12.6         | P12.7          | IO6 (PWM feedback) to          |
|                   |               |                |       IO7 (PWM Output)         |
|                   |               |                |                                |
| Advanced IO Tone  | P12.6         | P12.7          | IO6 (Tone feedback) to         |
|                   |               |                |       IO7 (Tone Output)        |
|                   |               |                |                                |
s