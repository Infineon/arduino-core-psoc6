Environment Setup
=================

.. note::
   | The development environment setup is (currently) only supported on **Linux** |:penguin:|. 
   | As many tools and scripts are not cross-platform, some of them will not directly work on Windows |:abcd:| or macOS |:apple:|.


0. Obviously |:neutral_face:|, install `Arduino IDE (2.0 or higher) <https://docs.arduino.cc/software/ide-v2/tutorials/getting-started/ide-v2-downloading-and-installing/>`_ or `Arduino CLI (1.0.0 or higher) <https://arduino.github.io/arduino-cli/0.24/installation/>`_.

1. Install the PSOC™ 6 Arduino core as described :ref:`here <psoc6_core_installation>`.

2. Locate, where the Arduino15 packages are installed on your computer. Usually:

   ::

      ~/.arduino15/


3. Starting from the Arduino installation location, change directories:

   ::

      cd packages/infineon/hardware/psoc6

4. Remove any existing installed "x.y.z" version of the core:

   ::

      rm -rf x.y.z

5. | Clone this repo in a folder with a semver version name. A orderly practice could be to bump the existing version, and add a suffix to indicate the feature or bugfix.
   | For example, from installed ``1.1.0`` to ``1.2.0-new-feature``:

   ::

      git clone https://github.com/Infineon/arduino-core-psoc6.git 1.2.0-new-feature

   You can check if the correct version is installed by running:

   ::

      arduino-cli core list

   or in the Arduino IDE, navigate to *Tools > Board > Boards Manager...* and search for *PSOC6*.

   .. warning::
      | The name of the repo directory needs to be a valid semver (x.y.z) version number. 
      | For more information about the valid Arduino semver notation, see the `Package index specification - Platforms definitions <https://arduino.github.io/arduino-cli/0.34/package_index_json-specification/#platforms-definitions>`_.

6. From the root directory of the core, run the setup script |:computer:|:

   ::

      bash tools/dev-setup.sh

   .. note::
      Alternatively, you can setup the development environment manually |:wrench:| following the steps in :ref:`Manual setup <dev_manual_setup>`.

7. If using the Arduino IDE, restart the application.

8. Start developing |:tools:|!


.. _dev_manual_setup:

Manual Setup 
------------

| This section describes the manual setup of the development environment for the Arduino core for PSOC™ 6.
| You can consider this a detailed explanation of the development setup process. 
| Unless you want to know the details |:detective:|, it is easier and recommended to use the automated setup script |:scroll:| in :doc:`env-setup`.

Installing the ArduinoCore-API
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Locate in your computer where the Arduino15 packages are installed. By default:

   .. tabs::

      .. group-tab:: Linux

         ::

             ~/.arduino15/

      .. group-tab:: Windows

         ::

            C:/Users/%USERNAME%/AppData/Local/Arduino15                

2. Change directories to the following path, where your ``arduino-core-psoc6`` repository should be cloned:

   ::

      cd packages/infineon/hardware/psoc6/<x.y.z>

3. Initialize the ArduinoCore-API submodule:

   ::

      git submodule update --init --recursive


4. Change directories:

   ::

      cd cores/psoc6

5. Copy or symlink the ``api`` folder from the ArduinoCore-API submodule:

   .. tabs::
      
      .. group-tab:: Linux

        ::

            ln -s ../../extras/arduino-core-api/api .

      .. group-tab:: Windows

         Use here absolute paths instead: 

         ::

            mklink /D "C:\Users\%USERNAME%\AppData\Local\Arduino15\packages\infineon\hardware\psoc6\<x.y.z>\cores\psoc6\api" "C:\Users\%USERNAME%\AppData\Local\Arduino15\packages\infineon\hardware\psoc6\<x.y.z>\extras\arduino-core-api\api"

6. You should see now the ``api`` folder in the ``cores/psoc6`` directory.


