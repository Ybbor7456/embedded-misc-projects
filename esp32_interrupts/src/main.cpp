#include <Arduino.h>
#include "esp_timer.h"

constexpr int BTN_PIN = 25;     // button to GND nad 25
constexpr int LED_PIN = 26;      // onboard LED on many v1 boards
constexpr int64_t DEBOUNCE = 50000;    // esp_timer_get_time() returns int64_t

volatile int      g_ledState   = LOW;
volatile int64_t  g_lastEdge = 0;       // volatile for interrupts, change state outside loop 
volatile bool     g_pressed    = false;   // debug flag set by ISR

void IRAM_ATTR btn_dbn() {    // debounce handled in function, can be handled in loop() 
  int64_t now = esp_timer_get_time();
  if (now - g_lastEdge < DEBOUNCE) return; 
  g_lastEdge = now;

  g_ledState = !g_ledState;
  digitalWrite(LED_PIN, g_ledState);
  g_pressed = true;  // tell loop to print 
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("Ready");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // initially low

  pinMode(BTN_PIN, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), btn_dbn, FALLING); // button + debounce triggers on HIGH to LOW
}

void loop() {
  static uint32_t lastBeat = 0;
  uint32_t now = millis();
  if (now - lastBeat >= 1000) { // serial issues, mismatched baud/port
    lastBeat = now;
    Serial.println("heartbeat");
  }

  if (g_pressed) {    // button pressed, ISR pressed
    g_pressed = false;
    Serial.printf("Toggled LED -> %d\n", g_ledState);
  }
}
