Debugging (VS Code)
===================

Debugging support described here is for Visual Studio Code. The Arduino IDE already provides a built-in debugger for supported boards.

#. Install the `Cortex-Debug` extension in VS Code.
#. Copy the `task_psoc.json` file from `tools/vscode-profile` to the `.vscode` directory and rename it to `tasks.json` in your project root.
#. In VS Code, run the task: **Generate launch.json for debug (PSoC6)**.
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