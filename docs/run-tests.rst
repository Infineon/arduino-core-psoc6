Running Hardware Tests
=========================

This document provides instructions on running tests on hardware using the Unity framework. These tests call the 
Arduino APIs to execute and verify functionality.

.. _env_test_setup:

Test Environment Setup
-----------------------

.. note::
   | The development environment setup is (currently) only supported on **Linux** |:penguin:|. 
   | As many tools and scripts are not cross-platform, some of them will not directly work on Windows |:abcd:| or macOS |:apple:|.

.. important::
   | To successfully run the tests, ensure that your development environment is properly configured.
   | See the section on :ref:`Environment Setup <env-setup>`.

Board Setup
-------------

1. Connect your PSOC™ 6 board (e.g. CY8CKIT-062S2-AI) to your computer |:computer:| via USB.

2. Make the board-to-board pin connections as described below:

   CY8CKIT-062S2-AI:

      .. include:: ../tests/cy8ckit-062s2-ai-hil-test-table.md
         :parser: myst_parser.sphinx_

Executing Test Cases Using `make`
---------------------------------

The provided Makefile leverages arduino-cli for tasks such as compilation, uploading, and monitoring, while using Unity as a testing framework.

1. Navigate to the Test Directory:

   ::

      cd extras/arduino-core-tests

2. Run a Specific Test:
   To execute a specific test, for example, a digital I/O test:

   .. code-block:: bash
      
      make FQBN= `FQBN` PORT= `PORT` test_digitalio_single monitor ENABLE_SYNC=0 

   - `FQBN`: select the connected board (e.g., `infineon:psoc6:cy8ckit_062s2_ai`).
   - `PORT`: set the appropriate serial port (e.g., `/dev/ttyUSB0`).
   - monitor : opens the serial monitor to view the test results.
   - ENABLE_SYNC : disable synchronization as this feature is currently used only in CI/CD hil-checks 

3. List All Available Test Targets:
   
   .. code-block:: bash
      
      make list_tests

4. Display Help for Makefile Targets:
   
   .. code-block:: bash

      make help

5. Start Exploring Other Test Features |:tools:| !
   Begin testing other board-specific features with ease!
