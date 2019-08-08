#include <ESP8266WiFi.h>
#include <ArduinoJson.h>       
#include <DNSServer.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>    
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h> 
#include <ESP8266HTTPClient.h>

#define SEALEVELPRESSURE_HPA (1020)
#define LIGHTSENSORPIN A0

Adafruit_BME280 bme;
bool isConnected = false;
const int onBoardLED = 2;

void setup() {    
    Serial.begin(115200);
    WiFiManager wifiManager;
    wifiManager.autoConnect("WeatherStation : " + ESP.getChipId(), "connect");
    pinMode(onBoardLED, OUTPUT); // on-board led
    bme.begin(0x76);  
    pinMode(LIGHTSENSORPIN, INPUT);
    // Print user register information.
    Serial.println("This is the sketch for interacting with AnchormenStations service.");
    Serial.print("Please use the following Station ID when registering: ");
    Serial.println(ESP.getChipId());
}

void loop() {
    if(WiFi.status() == WL_CONNECTED) {
      if(!isConnected) {
        // Turn alert off when connected to WIFI
        digitalWrite(onBoardLED, LOW);
        isConnected = true;
      }
      
      // Create JSON
      const size_t capacity = JSON_OBJECT_SIZE(8);
      DynamicJsonDocument doc(capacity);
      doc["StationId"] = String(ESP.getChipId());
      doc["AirPressure"] = String(bme.readPressure());
      doc["Humidity"] = String(bme.readHumidity());
      doc["AmbientLight"] = String(analogRead(LIGHTSENSORPIN));
      doc["Temperature"] = String(bme.readTemperature());

      // Send JSON to Server
      HTTPClient http;
      http.begin("http://weatherstationapi.ddns.net:5000/api/post/reading");
      http.addHeader("Content-Type", "application/json");
      String json;
      serializeJson(doc, json);
      http.POST(json);   //Send the request
      http.end();
 
    } else {
      if(isConnected) {
        // Alert user that station is disconnected from WIFI
        digitalWrite(onBoardLED, HIGH); 
        isConnected = false;
      }
    }
    Serial.println("Entering deep sleep mode.");
    ESP.deepSleep(60000000, WAKE_RF_DEFAULT);
}
