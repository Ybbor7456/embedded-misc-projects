### Project 3.2 Potentiometer + LDR LED Dimmer ###

## Description ##
This build combines a photoresistor (LDR) and a potentiometer to control an LED with hardware PWM. The LDR measures ambient light and produces an “auto” brightness (autoDuty) after smoothing and normalization. The potentiometer sets a maximum brightness ceiling. The final LED duty is the minimum of (autoDuty, ceiling).

## Wiring ##
Wire the potentiometer’s outer pins to ground and 3V3, take the center pin to an analog port. If turning the potentiometer one way does not go the way intended, swap places of the GND and 3V3 wires.  
An additional analog port for the ADC reading of the photoresistor junction. Wire this to the photoresistor leg without* the resistor. There is also a PWM pin used to turn the LED on, it will be connected to the anode of the LED. Make sure the cathode has an appropriate resistor, if using the pi pico or a different internal 3.3V board, a 330 ohms resistor will work. 

## How it Works ##
# Pot -> ADC -> ceiling -> (minimum with auto duty) -> PWM -> LED #
# Ldr -> ADC -> autoDuty #
The LDR detects and produces a voltage from light and the potentiometer reads voltage and sets the ceiling for brightness. The LDR reading is smoothed over with EMA, scaled, and then normalized as autoDuty. The reading for the potentiometer’s ceiling is a rate between the reading and the maximum read value. The minimum between autoDuty and the ceiling is set and that minimum value is set to duty, analogWrite(PWM, duty) is called. A PWM pulse determined by duty is sent to the LED. 

## Experimentation ##
The duty cycle was never able to reach a full output of 255 with the potentiometer. This was believed to have been for two reasons, either the dutyOut value derived from the ADC reading at the photoresistor, or the ADC reading derived from the potentiometer. I had gone back to project 3, an LDR LED Dimmer, where the environmental seenMin and seenMax variables were used to reach a maximum duty cycle of 255. Since the code for LDR ADC readings from this project and project 3 were the same, the potentiometers inability to reach 255 must have been from the ceiling variable. The potentiometer pin takes an ADC reading isolated from the LDR junction. If the potentiometer’s ceiling value cannot reach 4095, then it cannot output max duty. 
From the results, the duty cycle had only reached up to about 130 max. Fixing this issue required for there to be an automatic calibration from the potentiometer pin, setting its min and max values and substituting them in the normalization and scaling factors. 
