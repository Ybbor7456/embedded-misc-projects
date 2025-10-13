## Project 7 - Button Interrupts and Debounce ##

# Description #
A simple ESP32 interrupt demo with a debounced active-LOW push button(GPIO25). The push button triggers an ISR on the FALLING edge to toggle an LED (GPIO26). Debounce uses a time lockout (50 ms) with a microsecond timer to ignore switch bounce, ensuring one clean toggle per press.

# Wiring #
LED -> GP26
BTN -> GP25

# How it Works #
In earlier projects I read inputs (e.g., button pushes) by polling inside loop(). That works, but it ties reaction time to the loop’s speed and wastes CPU cycles checking “did it happen yet?”. Interrupts solve this: they’re hardware signals from peripherals (GPIO edges, timers, UART/SPI/I²C, etc.) that preempt normal code and jump immediately to a tiny Interrupt Service Routine (ISR). After the ISR runs, execution resumes where it left off. This delivers low latency, low jitter, and lets the CPU sleep or do other work instead of spinning. In previous projects that used UART/SPI/I²C interrupts were not directly called, rather the drivers used the interrupts under the hood. Events are the best when there are events that can occur faster than the loop. 
I have used debouncing in previous projects, particularly with push buttons. Debouncing allows for there to be a singular button press that only registers as a singular button press. Debouncing is used because push buttons have a bounce associated with them, chattering between HIGH and LOW upon press. In the code, each edge of the clock is recorded in microseconds and is then compared to a debounce value (50ms). If a new edge is presented within the comparison of the debounce value, then edges are ignored until the recording exceeds the debouncing time. Though not used in this project, there are various methods to debouncing available. One alternative is to use a state machine where debouncing is set true as a boolean value. Throughout the loop, the ISR records time of a fallen edge and swaps the boolean value. If the bool value is true (button pressed) and the time recorded by the ISR is greater than the set debounce time, the push button will activate and trigger what is intended to, the boolean value is then reset back to false. 

# Continuation #
Interrupts are best when they are minimal and concise, a better way to handle interrupts is to be as minimal as possible with ISRs and have the loop() do the majority of the work + interrupt priority. 
