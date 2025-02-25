Supported Features
===================

.. warning::

   This is a work in progress version of the Arduino Core for PSOC™ documentation. 
   While this warning notice is present, the documentation is NOT ready for public usage.


This section indicates implementation status of Arduino functions and libraries.


Built-in Libraries
^^^^^^^^^^^^^^^^^^^

.. list-table:: 
    :header-rows: 1

    * - Features
      - Description
      - Status
    * - Serial/UART
      - `Serial <https://reference.arduino.cc/reference/en/language/functions/communication/serial/>`_
      - ✅ Implemented
    * - Stream
      - `Stream <https://reference.arduino.cc/reference/en/language/functions/communication/stream/>`_
      - ✅ Implemented
    * - Digital I/O
      - | `digitalRead <https://reference.arduino.cc/reference/en/language/functions/digital-io/digitalread/>`_
        | `digitalWrite <https://reference.arduino.cc/reference/en/language/functions/digital-io/digitalwrite/>`_
        | `pinMode <https://reference.arduino.cc/reference/en/language/functions/digital-io/pinmode/>`_   
      - ✅ Implemented
    * - Time
      - | `delay <https://reference.arduino.cc/reference/en/language/functions/time/delay/>`_
        | `delayMicroseconds <https://reference.arduino.cc/reference/en/language/functions/time/delaymicroseconds/>`_ 
        | `micros <https://reference.arduino.cc/reference/en/language/functions/time/micros/>`_ 
        | `millis <https://reference.arduino.cc/reference/en/language/functions/time/millis/>`_ 
      - ✅ Implemented
    * - WiFi
      - `WiFi Network <https://docs.arduino.cc/libraries/wifi/>`_
      - ✅ Implemented
    * - Wire/I2C
      - `Wire <https://reference.arduino.cc/reference/en/language/functions/communication/wire/>`_
      - ✅ Implemented
    * - Interrupts
      - | `interrupts <https://reference.arduino.cc/reference/en/language/functions/interrupts/interrupts/>`_
        | `noInterrupts <https://reference.arduino.cc/reference/en/language/functions/interrupts/nointerrupts/>`_       
      - ✅ Implemented
    * - External Interrupts
      - | `attachInterrupt <https://reference.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/>`_
        | `detachInterrupt <https://reference.arduino.cc/reference/en/language/functions/external-interrupts/detachinterrupt/>`_
        | `digitalPinToInterrupt <https://reference.arduino.cc/reference/en/language/functions/external-interrupts/digitalpintointerrupt/>`_        
      - ✅ Implemented
    * - SPI
      - `SPI <https://reference.arduino.cc/reference/en/language/functions/communication/spi/>`_
      - ❌ Not Implemented
    * - Analog I/O
      - | `analogRead <https://reference.arduino.cc/reference/en/language/functions/analog-io/analogread/>`_
        | `analogReadResolution <https://reference.arduino.cc/reference/en/language/functions/analog-io/analogreadresolution/>`_
        | `analogReference <https://reference.arduino.cc/reference/en/language/functions/analog-io/analogreference/>`_
        | `analogWrite <https://reference.arduino.cc/reference/en/language/functions/analog-io/analogwrite/>`_
        | `analogWriteResolution <https://reference.arduino.cc/reference/en/language/functions/analog-io/analogwriteresolution/>`_
      - ❌ Not Implemented
    * - Advanced I/O
      - | `noTone <https://reference.arduino.cc/reference/en/language/functions/advanced-io/notone/>`_
        | `pulseIn <https://reference.arduino.cc/reference/en/language/functions/advanced-io/pulsein/>`_
        | `pulseInLong <https://reference.arduino.cc/reference/en/language/functions/advanced-io/pulseinlong/>`_
        | `shiftIn <https://reference.arduino.cc/reference/en/language/functions/advanced-io/shiftin/>`_
        | `shiftOut <https://reference.arduino.cc/reference/en/language/functions/advanced-io/shiftout/>`_
        | `tone <https://reference.arduino.cc/reference/en/language/functions/advanced-io/tone/>`_
      - ❌ Not Implemented


.. note::

  The features that are currently not implemented are planned to be implemented soon. Stay tuned for updates in future releases.
