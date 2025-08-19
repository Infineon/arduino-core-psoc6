Metafiles Naming Conventions
=============================


This section describes the naming conventions for metafiles used in the Arduino Core for PSOC™ 6.
When adding new variables to these metafiles, follow these conventions to maintain consistency and clarity.


.. note::

    Most variables are already defined and likely will not require modification soon. They are documented here to clarify their usage and the rationale behind their values.

The conventions apply to the following files:

- ``boards.txt``
- ``platform.txt``
- ``package/package_psoc6_index.template.json``

boards.txt
-----------

.. list-table:: 
    :header-rows: 1
    :widths: 20 30 30 30

    * - Parameter
      - Value
      - Description
      - Example
    * - ``<board>``
      - | **OPN (Original Part number)** 
        | of the board converted to **snake_case**    
      - | This defines the ``board_unique_id`` field 
        | of the `Fully Qualified Board Name (FQBN) <https://docs.arduino.cc/arduino-cli/platform-specification/>`_ 
        | used in arduino-cli.
      - ``cy8ckit_062s2_ai``
    * - ``<board>.name``      
      - | **OPN (Original Part number)** of 
        | the board 
      - | Human readable friendly Board Name 
        | shown in Arduino IDE menus.
      - ``CY8CKIT-062S2-AI``

platform.txt
-------------

.. list-table:: 
    :header-rows: 1
    :widths: 20 30 30

    * - Parameter
      - Value
      - Description
    * - ``name=``
      - ``Infineon PSOC Boards``
      - | The **name** will be shown as the Arduino IDE's Board
        | menu section title or the Name field of arduino-cli core 
        | list's output for the platform.
    * - ``version=``
      - | Empty. 
        | Automatically set during 
        | release core packaging.
      - | The **version** is currently unused, it is reserved for 
        | future use (probably together with the Boards Manager 
        | to handle dependencies on cores).

package_psoc6_index.json
------------------------

.. list-table:: 
    :header-rows: 1
    :widths: 20 40 30

    * - Parameter
      - Value
      - Description
    * - JSON file name
      - ``package_psoc6_index.json`` 
      - | `Specification <https://arduino.github.io/arduino-cli/1.3/package_index_json-specification/>`_ requirement: 
        | package_<YOURNAME_PACKAGENAME>_index.json
    * - ``name``
      - ``infineon``
      - | This defines the vendor directory `name` of 
        | the installed package in the Arduino installation
        | path.
        | It also defines the **vendor** field of the 
        | `Fully Qualified Board Name (FQBN) <https://docs.arduino.cc/arduino-cli/platform-specification/>`_.
    * - ``maintainer`` 
      - ``Infineon Technologies AG``
      - | The extended name of the vendor that is displayed
        | on the Arduino IDE Boards Manager GUI
    * - ``websiteURL`` 
      - `Link to docs <https://arduino-core-psoc6.readthedocs.io/en/latest/index.html>`_
      - | The URL to the vendor's website, appears
        | on the Arduino IDE Boards Manager as "More info" link.
    * - ``email``
      - ``maker@infineon.com``
      - Contact email of the maintainer
    * - ``platforms.name``
      - ``Infineon PSOC Boards``
      - | The extended name of the platform that
        | is displayed on the Boards Manager GUI.
    * - ``platforms.architecture``
      - ``psoc6``
      - | It defines the **architecture** field of the 
        | `Fully Qualified Board Name (FQBN) <https://docs.arduino.cc/arduino-cli/platform-specification/>`_.
    * - ``platforms.category``
      - ``Contributed``
      - | This field is reserved. Third-party
        | cores must set it to "Contributed."
    * - ``platforms.help.online``
      - Empty
      - | Used only in legacy Arduino IDE 1.x.
        | Not used in Arduino IDE 2.x.
    * - ``platforms.boards.name``
      - | **OPN (Original Part number)** 
        | of the board
      - | The list of supported boards shown in
        | the Arduino IDE's Boards Manager GUI
        | core entry.
    * - ``platforms.toolsDependencies.packager``
      - ``infineon``
      - | No specification. Use the same name
        | as the vendor.
    * - ``platforms.tools``
      - **kebab-case** naming
      - | No additional requirements other 
        | than the `package index specification <https://arduino.github.io/arduino-cli/1.3/package_index_json-specification/>`_.