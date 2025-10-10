#include <Arduino.h>
#include <Wire.h>            //TWI library      wire.begin(), wire.Setclock(), wire.Wrte, ect   
#include <Adafruit_GFX.h>       
#include <Adafruit_SSD1306.h>       


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DRAW_H 20
#define DRAW_W 88

static const char* ITEMS[] = {"Tuner", "ADC Bar", "About"};
static const int ITEM_COUNT = sizeof(ITEMS)/sizeof(ITEMS[0]);
#define OLED_RESET -1           // No reset pin on many I2C modules; share MCU reset
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int VREF = 5; 
const int ADCMAX = 1023; //may be different on Uno 
constexpr int POT_PIN = A0; 
constexpr int BTN_PIN = 2;

enum class Page: int{Menu, Tuner, Settings, About};
Page activePage = Page::Menu;      
uint8_t menuIndex = 0; 

int adcToMenuIndex(int adc) {
  // 3 equal buckets (0..1023) -> 0,1,2 with hysteresis
  static uint8_t sel = 0;
  static int lastChange = 0;
  int div = map(adc, 0, 1023, 0, (int)ITEM_COUNT); // range of 0 to 3

  const int HYST = 30; // ADC counts (~3%), prevents from drastic/shaky changes in OELD
  if (div != sel) {
    if (abs(adc - lastChange) > HYST) { sel = div; lastChange = adc; }
  }
  return sel;
}

void drawMenu(int selected){
    display.clearDisplay();
    display.setTextSize(1);
    //int w = map(raw, 0, ADCMAX, 0, SCREEN_WIDTH);     // remaps v to range of 0-1023 to the width of 0 to screen width
    //w = constrain(w,0,SCREEN_WIDTH);

    for(int i = 0; i < ITEM_COUNT; i++){
        display.drawRect(DRAW_H, 20*i, DRAW_W, 16, SSD1306_WHITE);               // (20,0) -> (20,20) = (+0, +20)
        display.setCursor(45, 5 +(20*i));             // (x,y) -> (0, 5) -> (+0, +20)
        display.print(ITEMS[i]);

        if (i == selected) {
        display.fillRect(DRAW_H, 20*i, DRAW_W, 16, SSD1306_WHITE);
  }
    }

   display.display(); 
}

void drawTuner(){
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(8, 8);
    display.print(F("Tuner"));
    display.display();
    }

void drawBarScreen(){
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

void drawSettings(){
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(8, 8);
    display.print(F("Settings"));
    display.display();
}

void drawAbout(){
    display.clearDisplay();
    display.setTextSize(3);
    display.setCursor(20,20);
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
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 init failed"));
    while (1) {}
  }
  display.clearDisplay();
  display.setTextSize(2);            // 2x scale
  display.setTextColor(SSD1306_WHITE); // monochrome
  display.setCursor(0, 0);
  display.println(F("Hello OLED"));
  display.setTextSize(1);
  display.println(F("Arduino UNO + I^2C"));
  display.display();
  delay(1200);
}

void loop() {
    int raw = analogRead(A0);
    switch(activePage){

    case Page::Menu: {
      // pot only moves HIGHLIGHT, never changes the page:
      menuIndex = adcToMenuIndex(raw);
      drawMenu(menuIndex);
      if (readButtonPressed()) {
        activePage = (menuIndex == 0) ? Page::Tuner
                   : (menuIndex == 1) ? Page::Settings
                                      : Page::About;
      }
    } break;
        
        case Page:: Tuner:{
            drawTuner();
            if (readButtonPressed()) activePage = Page::Menu; 
        } break;

        case Page::Settings:{
            drawBarScreen();
            if(readButtonPressed()) activePage = Page::Menu; 

        } break;

        case Page::About:{
            drawAbout();
            if(readButtonPressed()) activePage = Page::Menu; 
        } break;
    
    }
  delay(50);
}
