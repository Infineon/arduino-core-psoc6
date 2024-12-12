Supported features
===================

.. warning::

   This is a work in progress version of the Arduino Core for PSOC™ documentation. 
   While this warning notice is present, the documentation is NOT ready for public usage.

The PSOC™ for Arduino platform provides a comprehensive set of built-in libraries that enable developers to easily access 
and utilize the PSOC™ microcontrollers' features and peripherals. This section lists these libraries, including their 
functionality and implementation status.


Built-in libraries
^^^^^^^^^^^^^^^^^^^

.. list-table:: 
    :header-rows: 1

    * - Features
      - Description
      - Status
    * - Serial/UART
      - `Serial <https://docs.arduino.cc/language-reference/en/functions/communication/serial/>`_
      - ❌ Not implemented
    * - Stream
      - `Stream <https://docs.arduino.cc/language-reference/en/functions/communication/stream/>`_
      - ❌ Not implemented
    * - GPIO
      - | `digitalRead <https://docs.arduino.cc/language-reference/en/functions/digital-io/digitalread/>`_
        | `digitalWrite <https://docs.arduino.cc/language-reference/en/functions/digital-io/digitalwrite/>`_
        | `pinMode <https://docs.arduino.cc/language-reference/en/functions/digital-io/pinMode/>`_   
      - ❌ Not implemented
    * - Delay
      - | `delay <https://docs.arduino.cc/language-reference/en/functions/time/delay/>`_
        | `delayMicroseconds <https://docs.arduino.cc/language-reference/en/functions/time/delayMicroseconds/>`_ 
      - ❌ Not implemented
    * - WiFi
      - `WiFi Network <https://docs.arduino.cc/language-reference/en/functions/wifi/wificlass/>`_
      - ❌ Not implemented
    * - Wire/I2C
      - `Wire <https://docs.arduino.cc/language-reference/en/functions/communication/wire/>`_
      - ❌ Not implemented
    * - Interrupts
      - | `interrupts <https://docs.arduino.cc/language-reference/en/functions/interrupts/interrupts/>`_
        | `noInterrupts <https://docs.arduino.cc/language-reference/en/functions/interrupts/noInterrupts/>`_       
      - ❌ Not implemented
    * - External Interrupts
      - | `attachInterrupt <https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/>`_
        | `detachInterrupt <https://docs.arduino.cc/language-reference/en/functions/external-interrupts/detachInterrupt/>`_
        | `digitalPinToInterrupt <https://docs.arduino.cc/language-reference/en/functions/external-interrupts/digitalPinToInterrupt/>`_        
      - ❌ Not implemented
    * - SPI
      - `SPI <https://docs.arduino.cc/language-reference/en/functions/communication/SPI/>`_
      - ❌ Not implemented
    * - ADC
      - | `analogRead <https://docs.arduino.cc/language-reference/en/functions/analog-io/analogRead/>`_
        | `analogReadResolution <https://docs.arduino.cc/language-reference/en/functions/analog-io/analogReadResolution/>`_
        | `analogReference <https://docs.arduino.cc/language-reference/en/functions/analog-io/analogReference/>`_
      - ❌ Not implemented
    * - PWM
      - | `analogWrite <https://docs.arduino.cc/language-reference/en/functions/analog-io/analogWrite/>`_
        | `analogWriteResolution <https://docs.arduino.cc/language-reference/en/functions/analog-io/analogWriteResolution/>`_
      - ❌ Not implemented
    * - Timer
      - | `micros <https://docs.arduino.cc/language-reference/en/functions/time/micros/>`_ 
        | `millis <https://docs.arduino.cc/language-reference/en/functions/time/millis/>`_ 
      - ❌ Not implemented


.. note::

  The features that are currently not implemented are planned to be implemented soon. Stay tuned for updates in future releases.
