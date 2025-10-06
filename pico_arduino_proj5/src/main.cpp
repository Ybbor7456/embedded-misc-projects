#include <Arduino.h>
#include <Wire.h>            //TWI library      wire.begin(), wire.Setclock(), wire.Wrte, ect   
#include <Adafruit_GFX.h>       
#include <Adafruit_SSD1306.h>       


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1           // No reset pin on many I2C modules; share MCU reset
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int VREF = 5; 
const int ADCMAX = 1023; //may be different on Uno 
constexpr int POT_PIN = A0; 
constexpr int BTN_PIN = 2;
uint8_t page = 0; 


void drawScreenTwo(){
  int raw = analogRead(A0); 
  int w = map(raw, 0, ADCMAX, 0, SCREEN_WIDTH);     // remaps v to range of 0-1023 to the width of 0 to screen width
  w = constrain(w,0,SCREEN_WIDTH);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("A0: ")); // use F to save RAM on uno
  display.print(raw);

  display.fillRect(0, 20, w, 14, SSD1306_WHITE);
  display.drawRect(0, 20, SCREEN_WIDTH, 14, SSD1306_WHITE); // change w to be affected by potentiometer. 
  display.display();
}

void drawScreenOne(){
    display.clearDisplay();
    int raw = analogRead(A0); 
    //int w = map(raw, 0, ADCMAX, 0, SCREEN_WIDTH);     // remaps v to range of 0-1023 to the width of 0 to screen width
    //w = constrain(w,0,SCREEN_WIDTH);

    if(raw >= 250 && raw < 450){
    display.fillRect(20, 0, 88, 16, SSD1306_WHITE);
    }
    else if(raw >= 450 && raw < 650){
    display.fillRect(20, 22, 88, 16, SSD1306_WHITE);
    }
    else if(raw >= 650 && raw < 850){ 
    display.fillRect(20, 44, 88, 16, SSD1306_WHITE);
    }
   
    // draws 4 rectangles for menu with text
    display.drawRect(20, 0, SCREEN_WIDTH - 40, 16, SSD1306_WHITE);
    display.setCursor(45,5);
    display.print("Tuner"); 
    display.drawRect(20, 20, SCREEN_WIDTH -40, 16, SSD1306_WHITE);
    display.setCursor(45,25);
    display.print("Settings");
    display.drawRect(20, 40, SCREEN_WIDTH -40, 16, SSD1306_WHITE);
    display.setCursor(45,45);
    display.print("About"); 
    display.display();
    }

bool readButtonPressed() {
  static uint32_t lastChange = 0;
  static bool lastStable = false;
  static bool lastRead = true;
  bool raw = digitalRead(BTN_PIN); // true if pressed
  if (raw != lastRead) { lastRead = raw; lastChange = millis(); } ///updates last read time
  if (millis() - lastChange > 15) {  // debounce 15ms
    if (raw != lastStable) { lastStable = raw; return (lastStable == LOW); } // returns true
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP); 
  Wire.begin();              // SDA and SCLK
  delay(100);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 init failed"));
    while (1) {}
  }
  display.clearDisplay();
  display.setTextSize(2);            // 2x scale
  display.setTextColor(SSD1306_WHITE); // monochrome
  display.setCursor(0, 0);
  display.println(F("Hello OLED"));
  display.setTextSize(1);
  display.println(F("Arduino UNO + I2C"));
  display.display();
  delay(1200);

/*
  display.clearDisplay();
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.fillCircle(100, 16, 10, SSD1306_WHITE);
  display.drawLine(0, 32, 127, 32, SSD1306_WHITE);
  display.setCursor(6, 10);
  display.setTextSize(1);
  display.println(F("SSD1306 demo"));
  display.display();
*/
}

void loop() {
    if (readButtonPressed()) {
    page ^= 1; // toggle 0<->1 xor
  } 
    if (page == 0) drawScreenOne();
        else   drawScreenTwo();

  delay(50);
}
