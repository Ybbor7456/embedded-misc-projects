# embedded-misc-projects
A collection of small, focused embedded projects to reinforce fundamentals—both electronics and firmware—on the Raspberry Pi Pico (RP2040)

It's nice to be able to be bale to step away from long-term projects. Sometimes parts needs to be ordered, sometimes there's a mental block, and sometimes you just need a short break. 
These projects are to reinforce concepts that I learned through school. It can be easy to forget many things that were learned during class, as grades and deadlines push for results
rather than understanding. Being a CSE major, I have taken courses like embedded systems, but the way in which this class was setup was primarily around group work. 
It was typical for electrical engineers to design and wire the MCUs and for the CSE majors to do the coding and debugging. Which gets to one of the main learning objectives of these 
various projects... confidentally wiring and understanding electrical components and theory. 
The other goal is to enhance my understanding of just not how the code should be written, but how it works. It's one thing to call analogRead(), but it's better to understand 
the process of ADC and the internal workings of the MCU. 
Currently, I have six projects planned out to reinform me of the basics. 
# UART Debug Console # 
# ADC Light Meter #
# PWM LED Dimmer #
# SPI Sensor and Display #
# Button Interrupts with Debouncing #
# Timer Interrupts # 

## Learning Goals ##
Wire and debug circuits confidently.

Understand how peripherals work (ADC, PWM, UART, SPI, timers, interrupts)

Practice reading datasheets, reasoning about registers, and converting that into code.


## Hardware ##
Raspberry Pi Pico / Pico W
Breadboard, jumpers
LEDs + resistors (220–470 Ω)
LDR (e.g., GL5528) + 10 kΩ
...

## Software ##
PlatformIO in VS Code
Arduino core for RP2040 (Earle Philhower)
