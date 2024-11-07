#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

void setup () {
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Adjust time as follows: DateTime(year, month, day, hour, minute, second)
  rtc.adjust(DateTime(2024, 3, 21, 12, 36, 00)); // Adjust time to 12:30:00 on 21st March 2024
}

void loop () {
  DateTime now = rtc.now();

  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  delay(1000);
}
