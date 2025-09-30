## Project E PWM LED Emitter ##
This is a continuation from the ADC photoresistor setup, but using a pulse width modulator (PWM) to drive an LED bulb, along with the improvements. The improvements to the last project suggest reducing flicker, smoothing to reduce noise, and learning the min/max raw values of the environment to add a dimming agent. The wiring is the exact same as the ADC version, and has an additional function to find the average ADC value generated. 
The sketch uses analogWrite() to configure the PWM for pin number 15 and then update the duty cycle. Within the project loop, ADC average is calculated, an exponential moving average (EMA) is set for further calculations regarding the smoothing of noise, acting as a dampener to the voltage read from ADC . The voltage is tracked, using the photoresistor to track the highest and lowest levels of brightness read from its environment. The difference of these minimum and maximum values factors into the normalization variable, which is then taken to a power of 1/gamma, then multiplied to be the duty cycle value. 
# ADC vs PWM: # 
The ADC reads the photoresistor divider and produces a digital value. I map that value to a PWM duty cycle so the LED’s average power tracks ambient light.The MCU is now controlling power for the LED so the brightness is much smoother. Inside the MCU, a hardware timer counts from 0 up to TOP, which sets the PWM period/frequency. PWM has a compare register, when the output is HIGH while duty is greater than counter, and LOW when counter is greater than duty in the PWM cycle. 
Perceptual tweaks: I use an EMA to reduce sensor noise and a gamma curve so brightness changes feel more natural to the eye. A slew limiter caps how fast duty can change to avoid visible jumps.
# Variables #
raw - the instantaneous read ADC code from analogRead()
ema - initially assigned at raw, used to compare seenMin and seenMax, used in the norm calculation. Exponentially smoothed alternative value to raw. 
alpha - constant dampener to help reduce the value of ema. A larger alpha value is less smoothing. 
ema = raw *  + (1-)*ema
seenMin - takes the lowest ADC reading for calibration.
seenMax - takes the highest ADC reading for calibration.
span - dynamic range used to normalize the sensor value
norm -normalization value from 0 to 1
if(ema > seenMax) seenMax = ema
if(ema < seenMin) seenMin = ema
span = (seenMax - seenMin) if (seenMax -seenMin > 50) else span = 50
norm = (ema-seenMIN)/span 
GAMMA - constant value that is factored into shape. As gamma gets closer to 1, PWM becomes more linear. 

shaped - adds the perceptual curve to the dimming. 
shaped = 1/norm
duty - the PWM duty cycled derived from shaped. 
Vref - reference voltage of 3.3V from the pico. 
ADCMAX - max returned raw value from 12-bit ADC. 
volts - smoothed voltage estimate from ema
v = ema * (Vref/ADCMAX)
# Experimentation #
After experimenting with the alpha values to create a slower rise and fall of the duty cycles, it was apparent that the changes in the levels of brightness of the LED were easier to distinguish going from bright to dark. It was (personally) much more difficult to perceive the change in the LED’s brightness when brightness was increased. Although raising and lowering the duty cycle were treated at the same rate, the initial jump down or up were always drastic and made it more difficult to perceive the change in brightness. 
To cap those jumps, I added a slew-rate limiter so duty can only change by a small amount per update. The duty cycle was now working as a more noticeable dimmer with its less drastic changes, but one thing that I noticed was that when I shined a light on the photoresistor, the duty cycle would go from 255 to 0 instantaneously. Looking at the output, the normalization variable which is set from 0 to 1 goes to 0 and inevitably turns the duty cycle to 0. Since the device was run in a dark environment, the seenMin and seenMax variables are about the same if not equal. This makes it difficult for the normalization value to be anything but 0 when exposed to an LED flashlight. Beginning the run with the photoresistor exposed to some light provides the best results of slow dimming (though not as easy to perceive in darkness), now an LED flashlight slowly lowers the duty cycle value and dimming can be noticed. 

