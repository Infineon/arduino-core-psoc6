Supported Features
===================

.. warning::

   This is a work in progress version of the Arduino Core for PSOC™ 6 documentation. 
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
      - | `Serial <https://docs.arduino.cc/language-reference/en/functions/communication/serial/>`_
      - ✅ Implemented
    * - Stream
      - | `Stream <https://docs.arduino.cc/language-reference/en/functions/communication/stream/>`_
      - ✅ Implemented
    * - Digital I/O
      - | `digitalRead <https://docs.arduino.cc/language-reference/en/functions/digital-io/digitalread/>`_
        | `digitalWrite <https://docs.arduino.cc/language-reference/en/functions/digital-io/digitalwrite/>`_
        | `pinMode <https://docs.arduino.cc/language-reference/en/functions/digital-io/pinMode/>`_   
      - ✅ Implemented
    * - Time
      - | `delay <https://docs.arduino.cc/language-reference/en/functions/time/delay/>`_
        | `delayMicroseconds <https://docs.arduino.cc/language-reference/en/functions/time/delayMicroseconds/>`_ 
        | `micros <https://docs.arduino.cc/language-reference/en/functions/time/micros/>`_ 
        | `millis <https://docs.arduino.cc/language-reference/en/functions/time/millis/>`_ 
      - ✅ Implemented
    * - WiFi Network
      - | `WiFi Network <https://docs.arduino.cc/language-reference/en/functions/wifi/wificlass/>`_  
      - ✅ Implemented 
    * - WiFi Server 
      - | `WiFiServer <https://docs.arduino.cc/language-reference/en/functions/wifi/server/>`_  
      - ✅ Implemented  (except SSL)
    * - WiFi Client
      - | `WiFiClient <https://docs.arduino.cc/language-reference/en/functions/wifi/client/>`_  
      - ✅ Implemented  (except SSL)
    * - WiFi UDP
      - | `WiFiUDP <https://docs.arduino.cc/language-reference/en/functions/wifi/udp/>`_  
      - ✅ Implemented  (except Multicast)
    * - Wire/I2C
      - | `Wire <https://docs.arduino.cc/language-reference/en/functions/communication/wire/>`_
      - ✅ Implemented
    * - Interrupts
      - | `interrupts <https://docs.arduino.cc/language-reference/en/functions/interrupts/interrupts/>`_
        | `noInterrupts <https://docs.arduino.cc/language-reference/en/functions/interrupts/noInterrupts/>`_       
      - ✅ Implemented
    * - External Interrupts
      - | `attachInterrupt <https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/>`_
        | `detachInterrupt <https://docs.arduino.cc/language-reference/en/functions/external-interrupts/detachInterrupt/>`_
        | `digitalPinToInterrupt <https://docs.arduino.cc/language-reference/en/functions/external-interrupts/digitalPinToInterrupt/>`_        
      - ✅ Implemented
    * - SPI
      - | `SPI <https://docs.arduino.cc/language-reference/en/functions/communication/SPI/>`_
      - ✅ Implemented
    * - Analog I/O
      - | `analogRead <https://docs.arduino.cc/language-reference/en/functions/analog-io/analogRead/>`_
        | `analogReadResolution <https://docs.arduino.cc/language-reference/en/functions/analog-io/analogReadResolution/>`_
        | `analogReference <https://docs.arduino.cc/language-reference/en/functions/analog-io/analogReference/>`_
        | `analogWrite <https://docs.arduino.cc/language-reference/en/functions/analog-io/analogWrite/>`_
        | `analogWriteResolution <https://docs.arduino.cc/language-reference/en/functions/analog-io/analogWriteResolution/>`_
      - ✅ Implemented
    * - Advanced I/O
      - | `noTone <https://docs.arduino.cc/language-reference/en/functions/advanced-io/noTone/>`_
        | `pulseIn <https://docs.arduino.cc/language-reference/en/functions/advanced-io/pulseIn/>`_
        | `pulseInLong <https://docs.arduino.cc/language-reference/en/functions/advanced-io/pulseInLong/>`_
        | `shiftIn <https://docs.arduino.cc/language-reference/en/functions/advanced-io/shiftIn/>`_
        | `shiftOut <https://docs.arduino.cc/language-reference/en/functions/advanced-io/shiftOut/>`_
        | `tone <https://docs.arduino.cc/language-reference/en/functions/advanced-io/tone/>`_
      - ✅ Implemented
    * - Random
      - | `random <https://docs.arduino.cc/language-reference/en/functions/random-numbers/random/>`_
        | `randomSeed <https://docs.arduino.cc/language-reference/en/functions/random-numbers/randomSeed/>`_
      - ✅ Implemented


.. note::

  The features that are currently not implemented are planned to be implemented soon. Stay tuned for updates in future releases.
