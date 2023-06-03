# Swapper3D_Firmware
Swapper3D firmware that works with the Octoprint plugin

This firmware is loaded onto the Swapper3D (by BigBrain3D) using Arduino studio.

This code is essential for the operation of a 3D printer that uses multiple tools. It allows for precise control of the tool swapping mechanism, ensuring that the correct tool is used at the right time.


The code accepts and responds to the following commands:

    octoprint: This command is used for communication with Octoprint. The firmware responds with "swapper".
    load_insert: This command is used to load a tool. It requires a number part, which specifies the tool to be loaded. The firmware responds with "ok" and updates the LCD to show the loaded tool.
    unload_connect: This command is used to initiate the tool unloading process. The firmware responds with "ok" and updates the LCD to show "Connect".
    unload_pulldown: This command is used during the tool unloading process. The firmware responds with "ok" and updates the LCD to show "Pulldown".
    unload_deploycutter: This command is used to deploy the cutter during the tool unloading process. The firmware responds with "ok" and updates the LCD to show "Deploy cutter".
    unload_cut: This command is used to cut the filament during the tool unloading process. The firmware responds with "ok" and updates the LCD to show "Cut".
    unload_stowcutter: This command is used to stow the cutter after the tool unloading process. The firmware responds with "ok" and updates the LCD to show "Stow cutter".
    unload_dumpwaste: This command is used to dump waste during the tool unloading process. The firmware responds with "ok" and updates the LCD to show "Dump waste".
    unloaded_message: This command is used to indicate that the tool has been unloaded. The firmware responds with "ok" and updates the LCD to show "Insert: Empty".
    swap_message: This command is used to initiate a tool swap. It requires a number part, which specifies the tool to be swapped. The firmware responds with "ok" and updates the LCD to show the swapping process.

In addition to these commands, the code also includes a serialEvent function that continuously reads incoming serial data, appending each character to a string until it encounters a newline character, which signals the end of a message.
