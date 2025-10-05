## Project 4.0 ##
# Description #
This project demonstrates driving multiple LEDs with minimal MCU pins using an SN74HC595 serial-in/parallel-out shift register on a Raspberry Pi Pico with PlatformIO (Arduino core).
The pico clocks an 8-bit value into the register and then latches it with RCLK to light the LEDs simultaneously. This project is SPI compatible, but not fully using SPI.
The bit register is being fed by serial-in and SCLK to write only. Many peripherals are write-only, so omitting MISO is fine. Rather than using CS, the 595 listens to SRCLK as a substitute. 
# Wiring #
The 8-bit register (74HC595) has a total of 16 pins, 9 of which are outputs (one inverted), one serial input (SER), a power (VCC), GND, output enable (OE),
serial register clock (SRCLK), storage register clock (RCLK), and a reset for the serial register pin (SRCLR). Each pin is labeled 1-16.

16. VCC -> 3V
15. Output 1 (QA) -> 330 ohm resistor -> LED
14. SER -> GP12
13. OE - > GND
12. RCLK -> GP10
11. SRCLK -> GP11
10. SRCLR -> 3V
9. Output 8 (Serial Out) -> 330 ohm resistor -> LED
8. GND -> GND
1-7. Output (QB-QH) -> 330 ohm resistor -> LED
 
# How it Works # 
Shift Registers are sequential logic circuits used for storage and transfer of binary data. They can convert between serial and parallel data. 
They can be used for communication, often used as memory or buffer stages within other chips. There are a variety of shift registers, SIPO, PISO, PIPO, SISO. Used for data storage and buffering.
The bit registers used here are SIPO, expanding its number of outputs. SIPO consists of flip-flops, the serial in feeds the flip flop and clocked through. 
The Arduino library can be used with SPI for shift register pins. 
Bits from the serial pin are clocked into the shift register on every rising edge on the serial clock and existing bits are shifted toward QH’.
The storage register clock then copies 8 bits to the output all at once. The register isn’t automatically cleared, it is cleared when new bits are shifted in or pull the serial clock LOW. OE can temporarily stop the outputs without changing stored data. Each LED is driven by a 595 output through a series resistor;
outputs are digital at 0 V or VCC. QH’ provides a serial output to cascade additional 595s.
In the code, three pins are declared, latchPin (RCLK) , clockPin (SRCLK), and the dataPin (SER), and set as OUTPUT pins. Within the loop, the byte is created by counting to 256 (every 8 bit pattern) in a for loop, updates the outputs after a latch, and lights the LED. An alternative to the for loop that would light all lights at once is to send a bitmask at once for the outputs of concern. uint8_t bitmask = (1 << 6) | (1 << 5) | (1 << 0) lighting LEDs on the outputs A, F and G. The bits were sent with MSBFIRST. LSBFIRST would have the opposite effect on the map and order of outputs. 


# Experimentation #
After wiring and running initial test code, only one of the three LEDs were lit. The bit shift register had its outputs as QA, QD,
and QE but would not light sequentially due to how the initial code was written.The sketch counts every byte value (0–255) and latches it; 
each output pin, QA to QH simply displays one bit of that latched byte. Time it takes to shift to the n pin is t = 2n in seconds. 
Because a binary count sets lower bits earlier and more often, QA was always the first to light up, meaning that later output pins take longer to show their specific latched byte. 
