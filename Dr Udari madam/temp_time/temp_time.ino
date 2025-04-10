#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2561_U.h"
#include "RTClib.h"

#define TSL2561_ADDR (0x39)    // Default I2C address for TSL2561
#define SDA_PIN 21              // Define SDA pin
#define SCL_PIN 22              // Define SCL pin

// Calibration factor
#define CALIBRATION_FACTOR 0.87 // Adjust this value according to your calibration needs

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR, 12345);
RTC_DS3231 rtc;

void setup(void) {
  Serial.begin(9600);
  if (!tsl.begin()) {
    Serial.println("No TSL2561 sensor found!");
    while (1);
  }
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
}

void loop(void) {
  sensors_event_t event;
  tsl.getEvent(&event);

  DateTime now = rtc.now();

  if (event.light) {
    // Apply calibration factor
    float calibratedLight = event.light * CALIBRATION_FACTOR;
    
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
    Serial.print(" - Real Light: ");
    Serial.print(calibratedLight);
    Serial.println(" lux");
  } else {
    Serial.println("No light detected");
  }
  delay(500);
}
