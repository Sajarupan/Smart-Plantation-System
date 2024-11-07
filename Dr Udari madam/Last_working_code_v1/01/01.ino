#include <Wire.h>
#include "SHTSensor.h"
#include "Adafruit_TSL2561_U.h"
#include "RTClib.h"
#include "WiFi.h"
#include <HTTPClient.h>

#define TSL2561_ADDR 0x39
#define RELAY_PIN 5 // Change this to the pin connected to your relay

SHTSensor sht(SHTSensor::SHTC1);
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR, 12345);
RTC_DS3231 rtc;

// WiFi credentials
const char* ssid = "Saji";         // change SSID
const char* password = "12345678";    // change password

// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbwXxH5LqMQWSVOjhkOMpa4ZjcSb8If3Y5mrHgeE2q2ensGLrU54fLC1zfnaNprBZLyu";    // change Gscript ID

void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(1000); // let serial console settle

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

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

    String Date = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day());
    String Time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
    String lux = String(event.light);
    String Temp = String(sht.getTemperature(), 2);
    String Humid = String(sht.getHumidity(), 2);
    String Relay_State = digitalRead(RELAY_PIN) == HIGH ? "ON" : "OFF";

    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"Date=" + Date + "&Time=" + Time + "&LUX=" + lux + "&Temperature=" + Temp + "&Humidity=" + Humid + "&Relay_State=" + Relay_State;
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: "+payload);    
    }
    //---------------------------------------------------------------------
    http.end();

    // Check if the current time is between 8:00 and 17:00
    if(now.hour() >= 8 && now.hour() < 17) {
      if(event.light < 300) {
        digitalWrite(RELAY_PIN, HIGH); // Turn on the relay
        Serial.println("Relay is ON");
      } else {
        digitalWrite(RELAY_PIN, LOW); // Turn off the relay
        Serial.println("Relay is OFF");
      }
    } else {
      digitalWrite(RELAY_PIN, LOW); // Turn off the relay
      Serial.println("Relay is OFF");
    }
  } else {
    Serial.println("Error in readSample()");
  }

  delay(10000); // Delay for 10 seconds (10000 milliseconds)
}
