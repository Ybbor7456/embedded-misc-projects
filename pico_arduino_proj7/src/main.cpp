#include <Arduino.h>
#include <Wire.h>            //TWI library      wire.begin(), wire.Setclock(), wire.Wrte, ect   
#include <Adafruit_GFX.h>       
#include <Adafruit_SSD1306.h>       
#include <SdFat.h>
#include <SPI.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DRAW_H 20
#define DRAW_W 88

static const char* ITEMS[] = {"Tuner", "SD Log", "About"};
static const int ITEM_COUNT = sizeof(ITEMS)/sizeof(ITEMS[0]);
#define OLED_RESET -1           // No reset pin on many I2C modules; share MCU reset
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

constexpr int POT_PIN = 26;//may be different on Uno 
constexpr int BTN_PIN = 2;
constexpr int CS_PIN = 17; 

const float VREF = 3.3f;
const int   ADCMAX = 4095;
SdFat sd;
SdFile file;
bool sdReady = false;
bool redraw = false; 

enum class Page: int{Menu, Tuner, MicroSD, About};
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

void drawMicroSD() {
  if (!sdReady) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(F("SD not ready"));
    display.display();
    return;
  }

  // Make sure recent writes are visible
  file.flush();

  SdFile rf;    // open logs
  if (!rf.open("log.csv", O_READ)) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(F("Open log.csv failed"));
    display.display();
    return;
  }

  // n lines
  const int N = 5;
  const int n_ch = 20;      // 20 char
  char lines[N][n_ch + 1];  // +1 for null terminator
  int count = 0, head = 0;

  char cur[n_ch + 1];
  int pos = 0;

  int c;
  while ((c = rf.read()) >= 0) {    // reads until eof
    if (c == '\r') continue;  
    if (c == '\n') {  // new line
      cur[pos] = '\0';
      strncpy(lines[head], cur, n_ch + 1);      // head 0 -> 1,2,3,4,0 count = N, rewries over previous lines until last 5 are written
      head = (head + 1) % N;
      if (count < N) count++;
      pos = 0;               // start a new line
    } 
    else {
      if (pos < n_ch) cur[pos++] = (char)c; // cuts long lines 
    }
  }
  rf.close();

  // draw read lines from oldest first
  display.clearDisplay();
  display.setTextSize(1);
  int start = (head - count + N) % N; //  head = 4, start = 1
  for (int i = 0; i < count; i++) {     // prints newest lines first. 
    int idx = (start + i) % N; // 
    display.setCursor(0, i * 12);   // 12px line spacing looks nice
    display.print(lines[idx]);
  }

  display.display();
}

void setup() {
  Serial.begin(115200);
  //Serial.print("beat1 \n"); 
  pinMode(BTN_PIN, INPUT_PULLUP); 
  Wire.setSDA(20);
  //Serial.print("beat2 \n"); 
  Wire.setSCL(21);
  Wire.begin();              // SDA and SCLK
  delay(100);
  //Serial.print("beat3 \n");
  SPI.begin();                               // SPI0: SCK=18 MISO=16 MOSI=19
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  //Serial.print("beat4 \n");
  sdReady = sd.begin(CS_PIN, SD_SCK_MHZ(12));
  //Serial.print("beat5 \n"); 


  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 init failed"));
    while (1) {}
  }
  if (!sdReady) {
    Serial.println("SD init failed (no card or wiring). Running without SD.");
  } 
  if (sdReady) {
    Serial.println("SD OK.");
    if (!file.open("log.csv", O_WRITE | O_CREAT | O_AT_END)) {
      Serial.println("Open failed; logging disabled.");
      sdReady = false;
    } else if (file.fileSize() == 0) {
      file.println(F("millis,adc_A0"));
      file.flush();
    }
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
  if (sdReady) {
   Serial.print("sd is Ready ");
    file.print(millis()); file.print(','); file.println(raw);
    file.flush();
  }
  //Serial.print("beat1");
    switch(activePage){

    case Page::Menu: {
      // pot only moves HIGHLIGHT, never changes the page:
      menuIndex = adcToMenuIndex(raw);
      drawMenu(menuIndex);
      if (readButtonPressed()) {
        activePage = (menuIndex == 0) ? Page::Tuner
                   : (menuIndex == 1) ? Page::MicroSD
                                      : Page::About;
                            
        if (activePage == Page::MicroSD) redraw = true; 
      }
    } break;
        
        case Page:: Tuner:{
            drawTuner();
            if (readButtonPressed()) activePage = Page::Menu; 
        } break;

        case Page::MicroSD:{
          if(redraw){
              if(file.isOpen()) file.flush();
              drawMicroSD();
              redraw = false; 
            }
          if (readButtonPressed()) activePage = Page::Menu; 
        } break;

        case Page::About:{
            drawAbout();
            if(readButtonPressed()) activePage = Page::Menu; 
        } break;
    
    }
  delay(50);
}
