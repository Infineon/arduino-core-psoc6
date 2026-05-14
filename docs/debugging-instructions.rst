Debugging
=========

Debugging in Arduino IDE 2
--------------------------

Arduino IDE 2 (2.2.0 or higher) includes a built-in debugger for boards that declare debug support in the core platform.
The PSOC™ 6 core configures OpenOCD with KitProg3 as the debug server, so no extra extensions are needed.

#. Build and upload your sketch at least once via *Sketch > Upload*.
#. Click the **Debug** button (🐛) in the toolbar, or go to *Sketch > Start Debugging*.
#. Arduino IDE will automatically start OpenOCD and attach GDB to the CM4 core.
#. Use the **Debug** panel (breakpoints, step over/into, variable inspection) as usual.

.. note::
   Make sure the board is connected and the correct port is selected before starting a debug session.

Debugging in VS Code
--------------------

Debugging support described here is for Visual Studio Code.

#. Install the `Cortex-Debug` extension in VS Code.
#. Copy the `task_psoc.json` file from `tools/vscode_profile` to the `.vscode` directory and rename it to `tasks.json` in your project root.
#. In VS Code, run the task: **Generate launch.json for debug (PSOC)**.
#. Required parameters for this task:
   * **fqbn**: Fully Qualified Board Name (e.g., `infineon:psoc6:cy8ckit_062s2_ai`)
   * **build path**: Directory where the `.elf` file will be placed
   * **example path**: Path to the sketch (`.ino` file) to debug (ensure it has been built at least once to generate the required build files)
#. Optional parameters:
   * **boards.txt path**: Path to a custom `boards.txt` file
   * **gdb path**: Path to a custom GDB executable

Manual Usage
------------

You can also generate the `launch.json` file manually using the `gen_launch.sh` script:

.. code-block:: bash

   ./tools/gen_launch.sh --fqbn <board_fqbn> --build-path <path_to_build> --example-path <path_to_sketch> [--boards-txt <path_to_boards.txt>] [--gdb-path <path_to_gdb>]

Refer to the documentation of your chosen debugger and scripts in the `tools/` folder for more details.

.. note::
   If you encounter an error indicating that ``libncurses.so.5`` or a similar library cannot be found, please search online and install the appropriate package for your environment.