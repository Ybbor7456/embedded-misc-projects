#include <Arduino.h> 

const int POT_PIN = A0;      // GP26, ADC pin has been replaced 
const int PWM_PIN = 15;      // GP15, only switch from prev project
const int LDR_PIN = A1;     // GP27, photoresistor junction 
const int PWM_PIN1 = 14;    // test LED brightnesses
const int PWM_PIN2 = 13; 

// Settings
const float VREF   = 3.3f;
const int   ADCMAX = 4095; 

const float ALPHA = .015; 
const float GAMMA = 2.0;
float ema = -1; 
int  seenMax = 0; 
int  seenMin = ADCMAX; 
bool INVERT = true; 

void setup(){
    Serial.begin(115200); 
    unsigned long t = millis(); 
    while(!Serial && millis()-t < 1500){

    }
    Serial.println("starting");
    pinMode(PWM_PIN, OUTPUT);
    pinMode(PWM_PIN1, OUTPUT);
    pinMode(PWM_PIN2, OUTPUT);
    analogWrite(PWM_PIN, 0);
    analogReadResolution(12); 
}

static uint16_t avg_adc(uint8_t n = 8, int pin = POT_PIN) {
  uint32_t sum = 0;
  for (uint8_t i = 0; i < n; ++i) {
    sum += analogRead(pin);        // 0..4095
  }
  return (uint16_t)(sum / n);
}

void loop(){
    uint16_t ldrraw = avg_adc(8, LDR_PIN);
    uint16_t potraw = avg_adc(8, POT_PIN);
    static uint16_t potMin=4095, potMax=0;
    potMin = min(potMin, potraw);
    potMax = max(potMax, potraw);
    if(ema < 0) ema = ldrraw;
    ema = ALPHA*ldrraw + (1-ALPHA)*ema; 

    if (ema < seenMin) seenMin = (int)ema;
    if (ema > seenMax) seenMax = (int)ema;
    int span = max(20, (int)potMax - (int)potMin); // change span to no longer need ema for seenMax/Min

    float norm = (potraw - potMin) / (float)span;         // 0..1
    norm = constrain(norm, 0.0f, 1.0f);
    if (INVERT) norm = 1.0f - norm;                        // bright room -> lower n
    float shaped = powf(norm, 1.0f / GAMMA);      // perceptual curve, rounds norm^1/gamma, 0-1
    int duty = (int)round(shaped * 255.0f);       //  0-255
    duty = constrain(duty, 0, 255);

    int floor = 20; 
    int ceiling = floor + (int)lroundf((potraw / (float)ADCMAX) * 255.0f); // normalizes and scales, use floats for lroundf() safer
    int dutyOut = min(duty, ceiling);     // takes lowest of val between duty and ceiling, duty derived from ldrraw vs ceiling -> potraw
    dutyOut = constrain(dutyOut, 0, 255);   // scale it
    analogWrite(PWM_PIN, dutyOut);  
    analogWrite(PWM_PIN1, 255); 
    analogWrite(PWM_PIN2, 50);  //*** perceived brightness isn't linear*** 
    
    static uint32_t tLast = 0;
    //debug screen
  if (millis() - tLast > 500) {
    tLast = millis();

    float volts = ema * (VREF / ADCMAX);
    Serial.print("adc_avg_potentiometer: "); Serial.print(potraw); Serial.print("\n");
    Serial.print("adc_avg_photoresistor: "); Serial.print(ldrraw);  Serial.print("\n");
    Serial.print("raw=");   Serial.print((int)ema);Serial.print("\n");
    Serial.print(" V=");    Serial.print(volts, 3);Serial.print("\n");
    Serial.print(" min/max="); Serial.print(seenMin); Serial.print("/"); Serial.print(seenMax); Serial.print("\n");
    Serial.print(" norm="); Serial.print(norm, 2); Serial.print("\n");
    Serial.print(" duty="); Serial.println(dutyOut); Serial.print("\n");
  }

  delay(100);
}