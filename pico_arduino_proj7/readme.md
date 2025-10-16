# Project SPI MicroSD & I^2C OLED #

# Description #
This project logs sensor data to a microSD card using the Raspberry Pi Pico over SPI. Using the SdFat library, the firmware initializes the card, mounts a FAT filesystem, opens/creates files, and appends time-stamped readings. This file data is displayed through an OLED using the same Raspberry Pi Pico over I^2C. The OLED has a menu screen controlled by a potentiometer for scrolling menu items and a push button as an item selector. Only the five most recent readings are displayed on the OLED. 

# Wiring #
Micro SD Adapter
MISO -> GP16		CS -> GP17		SCLK -> GP18
MOSI -> GP19		VCC -> VBUS		GND -> GND
OLED
GND -> GND		VCC -> 3V3		SCL -> 21		SDA -> 20
Push Button
GND and GP2
Potentiometer 
GND, 3V3, GP26

# How it Works #
SPI path: sd.begin(CS, SD_SCK_MHZ(..)) initializes the card, mounts FAT, and the sketch appends millis,adc rows to log.csv.
I²C path: the menu uses the pot to pick items; the MicroSD page flushes the writer and tails the last 5 lines from log.csv to the OLED.
