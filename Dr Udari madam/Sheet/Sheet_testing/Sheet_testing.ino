#include <Wire.h>
#include "SHTSensor.h"
#include "Adafruit_TSL2561_U.h"
#include "RTClib.h"

#include "WiFi.h"
#include <HTTPClient.h>

#define TSL2561_ADDR 0x39
#define RELAY_PIN 5 // Change this to the pin connected to your relay

// Add your WiFi credentials
const char* ssid = "Saji";
const char* password = "12345678";

// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbzlhXOckQxWGjCNQvV_5EJNrezszOpo9W_QUdHs_BhsBylAWlVWXTgq6EJ5Jp8prMLw";    // change Gscript ID

SHTSensor sht(SHTSensor::SHTC1);
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR, 12345);
RTC_DS3231 rtc;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(1000); // let serial console settle

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
  } else {
    Serial.println("\nFailed to connect to WiFi");
    return;
  }

  if (!sht.init()) {
    Serial.println("SHT init(): failed");
    while (1);
  }
  if (!tsl.begin()) {
    Serial.println("TSL2561 init(): failed");
    while (1);
  }
  if (!rtc.begin()) {
    Serial.println("RTC init(): failed");
    while (1);
  }

  pinMode(RELAY_PIN, OUTPUT); // Set the relay pin as output

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

    // After getting the sensor data, send temperature, humidity, and light to Google Sheets
    sendToGoogleSheet(sht.getTemperature(), sht.getHumidity(), event.light);
  } else {
    Serial.println("Error in readSample()");
  }

  delay(1000);
}

void sendToGoogleSheet(float temperature, float humidity, float light) {
  HTTPClient http;

  // Your Google Script URL with the script ID
  String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec";

  // Data to be sent to the Google Sheet
  String data = "temperature=" + String(temperature) +
                "&humidity=" + String(humidity) +
                "&light=" + String(light);

  Serial.print("Sending data to Google Sheet: ");
  Serial.println(data);

  // Send HTTP POST request
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.POST(data);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error sending data to Google Sheet. HTTP Response code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
