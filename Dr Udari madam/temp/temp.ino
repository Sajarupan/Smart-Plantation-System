#include <Wire.h>
#include "SHTSensor.h"

SHTSensor sht(0x70); // Set I2C address to 0x70

void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(1000); // let serial console settle

  if (sht.init()) {
      Serial.println("SHT sensor initialized successfully.");
  } else {
      Serial.println("Failed to initialize SHT sensor.");
  }
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
}

void loop() {
  if (sht.readSample()) {
      Serial.println("SHT sensor data:");
      Serial.print("  Relative Humidity: ");
      Serial.print(sht.getHumidity(), 2);
      Serial.println("%");
      Serial.print("  Temperature: ");
      Serial.print(sht.getTemperature(), 2);
      Serial.println("°C");
  } else {
      Serial.println("Error reading data from SHT sensor.");
  }

  delay(1000);
}
