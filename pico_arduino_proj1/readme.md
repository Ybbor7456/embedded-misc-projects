Project C UART

This project reads and writes from UART over Pico's USB CDC COM port at 115200 baud. 
On startup, the firmware initializes the serial interface and will print a message of all commands available. The commands, help, Status, ver, and rate are typed and a response is sent back to the terminal. There was an added quiet mode to stop writing to the terminal every 500ms. This can be set with the status=0/1 command, 0 meaning off. 

The loop takes in an array of characters and ends the array with a terminal \0 to distinguish when the string has ended. 
Commands:

Help — Print the help menu with brief descriptions.


Ver — Show the firmware version.

Status=0/1 - Changes the status True or False which turns the serial writing on and off. 

rate<ms> - Set the period for status messages (default 500 ms).

Notes:

This uses USB CDC so using TX and RX for UART were unnecessary. 
For best results, set the terminal to CRLF line endings; echo can be enabled via PlatformIO monitor settings.
