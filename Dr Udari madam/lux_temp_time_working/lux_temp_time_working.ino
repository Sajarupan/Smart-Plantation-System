#include <Wire.h>
#include "SHTSensor.h"
#include "Adafruit_TSL2561_U.h"
#include "RTClib.h"


#define TSL2561_ADDR 0x39

//SHTSensor sht;
// To use a specific sensor instead of probing the bus use this command:
SHTSensor sht(SHTSensor::SHTC1);
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR, 12345);
RTC_DS3231 rtc;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(1000); // let serial console settle

  if (!sht.init()) {
    Serial.println("SHT init(): failed");
    while (1);
  }
  if (!tsl.begin()) {
    Serial.println("TSL2561 init(): failed");
    while (1);
  }
  if (! rtc.begin()) {
    Serial.println("RTC init(): failed");
    while (1);
  }

  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
}

void loop() {
  if (sht.readSample()) {
    sensors_event_t event;
    tsl.getEvent(&event);

    DateTime now = rtc.now();

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
    Serial.print(" - Temperature: ");
    Serial.print(sht.getTemperature(), 2);
    Serial.print(" C, Humidity: ");
    Serial.print(sht.getHumidity(), 2);
    Serial.print(" %, Light: ");
    Serial.print(event.light);
    Serial.println(" lux");
  } else {
    Serial.println("Error in readSample()");
  }

  delay(1000);
}
