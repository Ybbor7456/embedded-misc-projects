#include <Arduino.h>

const int latchPin = 10; 
const int clockPin = 11; 
const int dataPin = 12; 

void setup(){
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT); 
    pinMode(dataPin, OUTPUT); 
}

/*
void loop(){
    for (int numberToDisplay = 0; numberToDisplay <256; numberToDisplay++){ // 2^8 = 256 

        digitalWrite(latchPin, LOW); // 
        shiftOut(dataPin, clockPin, MSBFIRST, numberToDisplay); // shiftOut(data, clock, order, value)
        digitalWrite(latchPin, HIGH);   // 
        delay(500); 
    }
}
*/

void loop(){
        uint8_t mask = (1 << 3) | (1 << 2) | (1 << 0); // QD, Qc, QA
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, MSBFIRST, mask); // shiftOut(data, clock, order, value)
        digitalWrite(latchPin, HIGH);
        delay(500); 
    }