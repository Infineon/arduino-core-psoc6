Arduino API Deviations
======================

This section documents the differences between the standard Arduino Language Reference and `PSoC6-for-Arduino`.

Digital IO
----------

Once a pin is configured using pinMode as GPIO pins, it may no longer be available for use with any other peripherals (e.g., PWM, I2C, SPI, GPIO, etc.).

Analog IO
---------

Once a pin is configured and used as an ADC input, it may no longer be available for use with any other peripherals (e.g., PWM, I2C, SPI, GPIO, etc.).

The `PSoC6-for-Arduino` core does not support the following Arduino APIs:


.. code-block:: cpp

    void analogReadResolution(uint8_t bits)

This API is **not implemented** because the resolution is hardware-defined and fixed at 11 bits and the resolution cannot be dynamically reconfigured by firmware.

For applications requiring lower effective resolutions (e.g., 8-bit or 10-bit), users can manually scale the 11-bit ADC result in their application code by discarding the least significant bits (LSBs).


.. code-block:: cpp

    void analogReference(uint8_t mode)

Only the `DEFAULT` mode is supported and uses the internal reference voltage of the PSoC6 device, which is typically 3.3V.


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

