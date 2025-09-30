#include <Arduino.h>

// Pins (Pico)
const int POT_PIN = A0;      // GP26, ADC pin has been replaced 
const int PWM_PIN = 15;      // GP15, only switch from prev project

// Settings
const float VREF   = 3.3f;
const int   ADCMAX = 4095;   // 12-bit 2^n  -1 
//bool INVERT = true;         // true -> darker to brighter
// int seenMin = ADCMAX, seenMax = 0;
//const int MAX_STEP_UP = 5;
//const int MAX_SET_DOWN = 4; 
//const int ALPHA = 0.005;
// float ema = -1; 
//const int GAMMA = 1.4; 
// the same but using a potentiometer to control dimmed lights. 
void setup(){
    Serial.begin(115200); 
    unsigned long t = millis(); 
    while(!Serial && millis() - t <  1500){
        //wait 
    }                 
    analogReadResolution(12);
    pinMode(PWM_PIN, OUTPUT);
    analogWrite(PWM_PIN, 0);
    Serial.println("Turn the knob to change brightness!");
}

static uint16_t avg_adc(uint8_t n = 8, int pin = POT_PIN) {
  uint32_t sum = 0;
  for (uint8_t i = 0; i < n; ++i) {
    sum += analogRead(pin);        // 0..4095
  }
  return (uint16_t)(sum / n);
}

void loop(){
uint16_t raw = avg_adc(8, POT_PIN);                          
int duty = (int)lroundf((raw / (float)ADCMAX) * 255.0f);
duty = constrain(duty, 0, 255);
analogWrite(PWM_PIN, duty); 
// debug
 static uint32_t tLast = 0;
  if (millis() - tLast >= 200) {
    tLast = millis();
    float volts = (raw * VREF) / ADCMAX;
    Serial.print("raw: "); Serial.print(raw);   Serial.print('\t');
    Serial.print("duty: ");  Serial.print(duty);  Serial.print('\t');
    Serial.print("volts: "); Serial.println(volts, 3);
  }

  delay(10);
}