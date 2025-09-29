#include <Arduino.h>


bool status_enabled = false;           // start QUIET by default to avoid output ever 500ms
uint32_t status_period_ms = 500;       // periodic status interval (ms)
bool overflow = false; 
constexpr size_t MAX = 128;
char line[MAX]; 
size_t idx; 


// UART Console 
void uart_console_init() {
  Serial.begin(115200);
  unsigned long t0 = millis();
  while (!Serial && (millis() - t0 < 3000)) {} // wait 
  Serial.println("UART console ready.");
  Serial.println("Commands: help | ver | status=0/1 | rate=<ms>");
}

// line buffer
static char lineBuf[128];


void handle_command(const String& cmd) { // help, ver, status=0/1 rate=<> 
  if (cmd == "help") {
    Serial.println("Commands:");
    Serial.println("  help            - this help");
    Serial.println("  ver             - version string");
    Serial.println("  status=0/1      - disable/enable periodic status line");
    Serial.println("  rate=<ms>       - change status period (default 500 ms)");
  } else if (cmd == "ver") {
    Serial.println("pico-arduino USB-only (quiet) v2");
  } else if (cmd == "status=0") {
    status_enabled = false;
    Serial.println("status OFF");
  } else if (cmd == "status=1") {
    status_enabled = true;
    Serial.println("status ON");
  } else if (cmd.startsWith("rate=")) {
    long v = cmd.substring(5).toInt();
    if (v < 50) v = 50;
    status_period_ms = (uint32_t) v;
    Serial.print("rate="); Serial.println(status_period_ms);
  } else if (cmd.length() == 0) {
  } else {
    Serial.println("ERR: unknown cmd");
  }
}

void setup() {
  uart_console_init();
  // No GPIO/ADC/PWM initialization in USB-only mode
}

void loop() {
  // console input
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\r' || c == '\n') {
      lineBuf[idx] = 0;
      handle_command(String(lineBuf));
      idx = 0;
      overflow = false; 
    } else if (c == 8 || c == 127) {       // backspace and delete in ASCII 
      if (!overflow && idx) idx--; //removes last character
    } else if (isPrintable(c)) {  
      if(!overflow){      
      if (idx < sizeof(lineBuf) - 1) lineBuf[idx++] = c; // prevents buffer overflow
      else overflow = true; 
    }
  }}

  // periodic status 
  static unsigned long t_last = 0;
  if (status_enabled && (millis() - t_last > status_period_ms)) {
    t_last = millis();
    Serial.println("status: alive");
  }
}
