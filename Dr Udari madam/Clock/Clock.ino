#include <Wire.h>    // Include the Wire library for I2C communication
#include "RTClib.h"  // Include the RTC library

RTC_DS3231 rtc; // Create an instance of the DS3231 RTC

void setup() {
    Serial.begin(9600); // Initialize serial communication
    Wire.begin();       // Initialize the I2C communication as master
    rtc.begin();        // Initialize the RTC

    // Uncomment the line below if you want to set the time manually
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
    DateTime now = rtc.now(); // Get the current time from the RTC

    // Print the time in HH:MM:SS format
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    delay(1000); // Wait for 1 second before reading the time again
}
