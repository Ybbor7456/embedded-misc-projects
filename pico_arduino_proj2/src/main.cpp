#include <Arduino.h>




// Raspberry Pi Pico (Arduino core)
const int LDR_PIN = A0;   // GP26 on Pico ~10K
const int LED_PIN = 8;    // external LED with ~330 to GND


const float VTH      = 1.6f;   // volts
const float VREF     = 3.3f;   // Pico ADC reference voltage
const int   ADC_MAX  = 4095;   // raw , 2^12) -1
const int   THRESH_RAW = (int)(VTH / VREF * ADC_MAX); // = 1987, adc count threshold

void setup() {
  Serial.begin(115200);
  // opt wait
  unsigned long t0 = millis();
  while (!Serial && (millis() - t0 < 2000)) {}
  analogReadResolution(12);   // the RP2040’s ADC is a 12-bit SAR ADC
  pinMode(LED_PIN, OUTPUT);

  Serial.println("Pico LDR test ready");
}

void loop() {
  int raw = analogRead(LDR_PIN);               // On RP2040 the ADC block lives at 0x4004C000 and exposes 1. C&S. 2. 12bit conv result 
                                              // raw is digitlalcode adc returns from voltage at junction on photoresistor (binary)
  float volts = raw * (VREF / ADC_MAX);        // convert to volts (1987) * (3.3/4095)~ 1.600V

  Serial.print("raw="); Serial.print(raw);
  Serial.print("  V=");  Serial.println(volts, 3);
  if (raw < 1000) {
  digitalWrite(LED_PIN, HIGH);
} //else if (raw > THRESH_RAW) {
  //digitalWrite(LED_PIN, HIGH);
//}
 else {
  digitalWrite(LED_PIN, LOW);
}
 // digitalWrite(LED_PIN, (raw > THRESH_RAW) ? HIGH : LOW);
  delay(100);
}

// ADC takes a code (binary) that matches the voltage input from the nearest step 
// the enarest step - 12bit ADC with 3.3V references. 3.3/)2^12)-1 ~ .8mV, the LSB or spacing between steps
//rounds the analog voltage to the closest level 

//1 MCU routes voltage to ADC input via MUX
//2 ADC knows Vref
// 3 charges up to Vin
// 4 DAC used for Vdac trial voltage from 12bit value
// 5 comparator used to determien Vin > or < Vdac, makes a 0 or 1 based on which
// 6 SAR used to build bit by bit, comparator used as a yes/no switch for the current trial bit
// 7 after 12 conparator selections, 12 bit code is established
//8 placed in RESULT register for analogRead() 