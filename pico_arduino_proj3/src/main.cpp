#include <Arduino.h>

// Pins (Pico)
const int ADC_PIN = A0;      // GP26
const int PWM_PIN = 15;      // GP15, only switch from prev project

// Settings
const float VREF   = 3.3f;
const int   ADCMAX = 4095;   // 12-bit 2^n  -1 
bool INVERT = true;         // true -> darker to brighter

const int MAX_STEP_UP = 5;
const int MAX_SET_DOWN = 4; 


// Smoothing 
const float ALPHA = 0.005f;
 // larger alpha = faster & less smooth
float ema = -1.0f;            // added to stop a flicker
// Optional gamma correction
const float GAMMA = 1.4f;    // perceived brightness, changes brightness 'evenly' since our eyes do not perceive light that way
// change to 1 for linear PWM


// range to learn from environment
int seenMin = ADCMAX, seenMax = 0; // upgrade from proj2, learns environmental lows and highs , read the average adc values to test

void setup() {
  Serial.begin(115200); //
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 1500) {} // wait 1.5s

  analogReadResolution(12);          // 12bit
  pinMode(PWM_PIN, OUTPUT);
  analogWrite(PWM_PIN, 0);           // start off

  Serial.println("Auto-dimming: LDR (A0) -> PWM (GP15)");
  Serial.println("Cover and shine to let it learn min/max...");
}

static uint16_t adc_avg(uint8_t n = 8) {
  uint32_t s = 0;
  for (uint8_t i = 0; i < n; ++i) s += analogRead(ADC_PIN);
  return (uint16_t)(s / n); // returns avg adc
}

void loop() {
  // 1) Sample + smooth
  uint16_t raw = adc_avg();                     
  if (ema < 0) ema = raw;                      // first sample, 
  ema = ALPHA * raw + (1 - ALPHA) * ema;       // .15*avg + (.85)avg
  // 2) Track environment
  if (ema < seenMin) seenMin = (int)ema;
  if (ema > seenMax) seenMax = (int)ema;
  int span = max(50, seenMax - seenMin);       // avoid dividing by 0 

  // 3) Normalize to 0..1
  float norm = (ema - seenMin) / (float)span; // normalizes to 1 for clean math
  norm = constrain(norm, 0.0f, 1.0f);
  if (INVERT) norm = 1.0f - norm;   // if invert =1 , reverse

  // 4) Gamma → 8-bit PWM (0..255)
  float shaped = pow(norm, 1.0f / GAMMA);      // perceptual curve, rounds norm^1/gamma, 0-1
  int duty = (int)round(shaped * 255.0f);       //  0-255
  duty = constrain(duty, 0, 255);
  
  // added to control duty 
  static int dutyOut = -1;          // -1 the first time
  if(dutyOut < 0) dutyOut = duty; // assigns dutyOut to current duty
  int delta = duty - dutyOut;     // 0 first time
  if(abs(delta) > 0){             // skips first iteration 
    if(delta > 0){
     dutyOut += (delta > MAX_STEP_UP) ? MAX_STEP_UP : delta; // adds max step if cond true to dutyOut (only 20 max)
    }         // delta could* be greate than 20
    else {
      int step = -delta;
      dutyOut -= (step >MAX_SET_DOWN) ? MAX_SET_DOWN : step;
      }
  }
  // 5) Output PWM

  analogWrite(PWM_PIN, duty);

  // Debug (optional)
  static uint32_t tLast = 0;
  if (millis() - tLast > 500) {
    tLast = millis();
    float volts = ema * (VREF / ADCMAX);
    Serial.print("raw=");   Serial.print((int)ema);
    Serial.print(" V=");    Serial.print(volts, 3);
    Serial.print(" min/max="); Serial.print(seenMin); Serial.print("/"); Serial.print(seenMax);
    Serial.print(" norm="); Serial.print(norm, 2);
    Serial.print(" duty="); Serial.println(duty);
  }

  delay(200);
}

