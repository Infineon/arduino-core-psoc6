Arduino API Deviations
======================

This section documents the differences between the standard Arduino Language Reference and `PSOC6-for-Arduino`.

Digital IO
----------

Once a pin is configured using pinMode as GPIO pins, it may no longer be available for use with any other peripherals (e.g., PWM, I2C, SPI, GPIO, etc.).


Interrupts
----------

.. code-block:: cpp

    void attachInterrupt(pin_size_t interruptNumber, voidFuncPtr callback, PinStatus mode)

Interrupts are triggered only for the RISING or FALLING edge by the hardware. The LOW and HIGH modes are not natively supported but are internally mapped for compatibility:

- LOW mode is mapped to FALLING edge detection.
- HIGH mode is mapped to RISING edge detection.

This mapping simplifies compatibility but may not replicate the exact behavior of other Arduino platforms that natively support LOW and HIGH interrupt levels.

Analog IO
---------

Once a pin is configured and used as an ADC input, it may no longer be available for use with any other peripherals (e.g., PWM, I2C, SPI, GPIO, etc.).

.. code-block:: cpp

    void analogReadResolution(uint8_t bits)

The default ADC_RESOLUTION is fixed at 11 bits (range: 0–2047) due to hardware limitations.
Depending on the bits parameter, the function modifies the resolution of the ADC value:

- If bits < ADC_RESOLUTION: The resolution of the ADC value is reduced by truncating the least significant bits (LSBs).
- If bits > ADC_RESOLUTION: The resolution of the ADC value is increased by appending zeros to the least significant bits (LSBs).

.. code-block:: cpp

    void analogReference(uint8_t mode)

Only the `DEFAULT` mode is supported and uses the internal reference voltage of the PSOC6 device, which is typically 3.3V.


.. code-block:: cpp

    void analogWrite(int pin, int value)

The PWM resolution is fixed at 8 bits by default unless the user explicitly sets a different resolution using the `analogWriteResolution()` function.
The default frequency of the PWM signal is fixed at **1 kHz** and cannot be changed.


.. code-block:: cpp

    void analogWriteResolution(int res)

The input resolution is selected from following fixed options:

- Resolutions greater than 12 bits mapped to **16-bit PWM**.
- Resolutions greater than 10 bits mapped to **12-bit PWM**.
- Resolutions greater than 8 bits mapped to **10-bit PWM**.
- Resolutions of 8 bits or less mapped to **8-bit PWM**.

Advanced IO
------------

.. code-block:: cpp

    void tone(uint8_t _pin, unsigned int frequency, unsigned long duration)

The `tone()` function in the `PSOC6-for-Arduino` core leverages the PWM (Pulse Width Modulation) peripheral to generate square wave signals at specific frequencies.

Do not call `pinMode()` on the pin prior to using the `tone()` function.
The `tone()` function automatically configures the pin for output mode and sets it up internally. Calling `pinMode()` beforehand may result in unexpected behavior or conflicts with the PWM peripheral.

SPI
----------
The `PSOC6-for-Arduino` core does not support the following Arduino APIs:

.. code-block:: cpp

    void usingInterrupt(int interruptNumber)
    void notUsingInterrupt(int interruptNumber)
    void attachInterrupt()
    void detachInterrupt()

The SPI transfer functions are interrupt-driven; manually enabling or disabling interrupts and  attaching or detaching separate interrupts via these APIs is not applicable.

.. code-block:: cpp
    
    void setDataMode(uint8_t dataMode)
    void setBitOrder(uint8_t bitOrder)
    void setClockDivider(uint8_t div)

These APIs are retained only for backward compatibility with older Arduino code but are no longer recommended for use.
Instead, use the `SPISettings` object with `SPI.beginTransaction()` for configuring SPI modes, bit order, and clock frequency.

Random Number Generation
-------------------------

.. code-block:: cpp
    
    void randomSeed(unsigned long seed)

The function is optional. Random seed initialization is not required because the `PSOC6-for-Arduino` core uses the hardware-based True Random Number Generator (TRNG) for generating random numbers.

Calling `randomSeed(seed)` does nothing but is provided for compatibility with the standard Arduino API.
