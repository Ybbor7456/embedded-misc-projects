# Project 3.1 # 
Goal: Replace the photoresistor from Project 3 with a 10 kΩ potentiometer to control LED brightness via hardware PWM on a Pi Pico.


This is an extension of project 3 where a potentiometer is used as an LED dimmer rather than a photoresistor. A 10k ohm potentiometer was used to control PWM. The main differences between projects 3 and 3.1 are minimal in wiring and code.
# Wiring # 
The potentiometer has three pins, its outer pins going to power and ground, and its center pin is connected to an analog pin located on the MCU.
Potentiometer: one outer pin to 3V3, the other outer pin to GND, and the wiper to A0. 
If turning the knob clockwise makes the LED darker, swap the outer potentiometer pins.
# How it Works #
ADC is still reading a value, but analogRead() is now called having the potentiometer pin as its argument, and adc_avg() now uses the potentiometer value. ADC generates a code with wiper voltage, and that code is mapped to PWM duty. 
Since the photoresistor does not have an application, values to dampen or help with visibility of light are excluded. Duty still needs to be calculated, with the lack of dampening/visibility variables a new equation was used. 
The potentiometer is not* on the LEDs power path and twisting the knob does not directly feed the current/voltage of the LED. The wiper voltage from the potentiometer is read by an ADC pin, and that value is then used to determine the duty cycle of the PWM pin connected to the LED. 


# Improvements # 
Combine the photoresistor and potentiometer. 

