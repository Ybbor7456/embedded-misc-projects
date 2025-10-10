#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>


constexpr int CS_PIN = 17; 

// Use the core's built-in LED symbol; if it doesn't light, try 25 explicitly or use an external LED.
constexpr int LED_PIN = 2;

SdFat sd;
SdFile file;
bool sdReady = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);                 // heartbeat available immediately

  Serial.begin(115200);
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 2000) {}  // don't wait forever

  SPI.begin();                               // SPI0: SCK=18 MISO=16 MOSI=19
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  // Try slower speed first; cards can be picky
  sdReady = sd.begin(CS_PIN, SD_SCK_MHZ(12));
  if (!sdReady) {
    Serial.println("SD init failed (no card or wiring). Running without SD.");
  } else {
    Serial.println("SD OK.");
    if (!file.open("log.csv", O_WRITE | O_CREAT | O_AT_END)) {
      Serial.println("Open failed; logging disabled.");
      sdReady = false;
    } else if (file.fileSize() == 0) {
      file.println(F("millis,adc_A0"));
      file.flush();
    }
  }
}

void loop() {
  // Heartbeat so you can SEE loop is running even if SD is missing:
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  Serial.print("alive");
  int v = analogRead(26); // A0 on Pico

  if (sdReady) {
    file.print(millis()); file.print(','); file.println(v);
    file.flush();
    Serial.print("Logged: "); Serial.println(v);
  } else {
    // No SD: just print to Serial so you can verify life
    Serial.print("Value: "); Serial.println(v);
  }

  delay(200);
}
