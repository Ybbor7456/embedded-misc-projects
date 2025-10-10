## Project - I^2C OLED. ##

# Description #
I^2C OLED with a pushbutton and potentiometer. The potentiometer controls a meter that reads that ADC value. There is also a pushbutton that can change “pages” and load a new display. 

# Wiring #
OLED has four pins, power, ground, SCLK and SDA.
GND -> GND		VCC -> 3V		SCLK -> A5		SDA -> A4
The potentiometer has three pins. Its outer pins are wired to GND and power, the center being wired to A0. 
Place the pushbutton across the divide in a breadboard, and use the front two pins, attach one to ground and the other to D2. 
INMP441 has six pins, power (VDD), ground, R/L, SCL, SD, WS
GND -> GND		VDD -> 3,3V		R/L -> GND
SCL ->	14		SD ->	32		WS -> 15


# How it Works #
I^2C - A controller-target relationship with multiple devices, sends data through two wires, SCL and SDA.
Needs a start condition, and then sends the device address of the device it is trying to communicate with, then confirms if the controller device wants to read or
write data to the target device. The data transferal to the controller device is then acknowledged one byte at a time. Communication can be terminated with a stop condition. 
In its idle state, both SCL and SDA are HIGH. The start condition is signaled when SDA is LOW and then SCL is pulled down LOW, claiming the pulldown node as the controller device in
the process. The device address is typically 7-bits with a read and write bit attached. The controller device sends this bit to the target device to determine whether
to read or write data.After each byte, the receiver pulls SDA LOW for one clock (ACK) or leaves it HIGH (NACK). Finally, the stop condition is activated when SCL is HIGH and SDA is
moved to HIGH afterward. 
I^2C is an open-drain system with outputs on SDA and SCL. All of the devices connected to GND on a multidrop network through I^2C can pull the current to GND to zero, effectively making SCL and SDA LOW. They can then be released and sent back to HIGH without any command due to the pullup resistors. One rule for START and STOP signals is the devices on the bus will never send SCL HIGH and SDA HIGH, or SDA LOW and SCL LOW. When transmitting data, SDA doesn’t change on the rising or falling edge of SCL, only changing when the clock is LOW. 

OLED - SCL goes to the I^2C clock line, and needs a pullup resistor to VCC.  SDA to I^2C data line. I²C needs pull-ups on SDA/SCL. Most boards include them tied to VCC. If your MCU is 3.3 V and the board is powered at 5 V, check that the pull-ups don’t pull the lines to 5 V. The OLED is 64rows * 128 columns, rows consisting of 8 vertical pixels that make the screen. There is a 1024-byte frame buffer that pushes bits when display() is called. The MCU sends commands and data bytes through the SCL and SDA connection. On a rising edge, the receiver samples the current bit on SDA. SDA carries several types of bits, bit addresses, control bytes, commands, and pixel data. It is the pairing of SCL and SDA that send the data commands to the OLED. The OLED uses hardware specified control bytes 0x00 and 0x40 to distinguish between commands and data.  



The INMP441 outputs digital audio over I²S. LRCLK/WS toggles at the sample rate and selects which channel’s timeslot is active. The BCLK then subdivides each channel into bit times, each being one pulse of BCLK. After each LRCLK edge, there’s a one-bit delay where the sample’s MSB appears and the rest of the bits shift MSB-first. The mic drives DOUT during its selected timeslots set by the L/R pin. Inside the mic, there is a transducer that moves with sound, giving a small ADC voltage proportional to sound. That small voltage is then boosted by a pre-amp. Then 24-bit PCM samples are produced and serialized to reach DOUT. The sample PCM is 24 bits within a 32-bit slot, the mic’s true resolution is at 24 bits, but I^2C uses 32-bit time slots. 
