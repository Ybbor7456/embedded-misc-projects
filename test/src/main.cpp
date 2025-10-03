#include <Arduino.h>

constexpr int ledpin = 6; 
constexpr int senspin = 8; 


void setup() {
Serial.begin(9600); 
pinMode(ledpin, OUTPUT); 
pinMode(senspin, INPUT); 
digitalWrite(ledpin, LOW); 
}

void loop() {
 
//create a loop that if the sensor is high, then light the led
    if(digitalRead(senspin) == HIGH){
      digitalWrite(ledpin, HIGH);
    }     
    else{ digitalWrite(ledpin, LOW); }
    delay(10);
}
