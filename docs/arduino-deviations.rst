Arduino API Deviations
======================

This section documents the differences between the standard Arduino Language Reference and `PSoC6-for-Arduino`.

ADC
---

.. code-block:: cpp

    void analogReadResolution(uint8_t bits)

This API is **not implemented** in the `PSoC6-for-Arduino` core because the resolution is hardware-defined and fixed at 11 bits and the resolution cannot be dynamically reconfigured by firmware.

For applications requiring lower effective resolutions (e.g., 8-bit or 10-bit), users can manually scale the 11-bit ADC result in their application code by discarding the least significant bits (LSBs).


.. code-block:: cpp

    void analogReference(uint8_t mode)

Only the `DEFAULT` mode is supported in the `PSoC6-for-Arduino` core. The `DEFAULT` mode uses the internal reference voltage of the PSoC6 device, which is typically 3.3V.
