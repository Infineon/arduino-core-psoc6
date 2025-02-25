# Pin Connections for HIL Test Setup Between 2 Boards

| Test Name     | Board 1 Pin   | Board 2 Pin   | Description                    |
|---------------|---------------|---------------|--------------------------------|
| UART          | P10.1         | P10.0         | UART TX to RX                  |
|               | P10.0         | P10.1         | UART RX to TX                  |
|               |               |               |                                |
| Wire 1        | P0.2          | P0.2          | I2C SCL to SCL (with pullup)   |
|               | P0.3          | P0.3          | I2C SDA to SDA (with pullup)   |
|               | GND           | GND           | Common Ground                  |
|               |               |               |                                |
   

# Pin Connections for HIL Test Setup on Single Board

| Test Name     | Board 1 Pin   | Board 1 Pin   | Description                    |
|---------------|---------------|---------------|--------------------------------|
| Digital IO    | P9.6          | P9.7          | IO6 to IO7                     |
|               |               |               |                                |
| Wire 2        | P0.2          | P9.0          | I2C SCL to SCL (with pullup)   |
|               | P0.3          | P9.1          | I2C SDA TO SDA (with pullup)   |
