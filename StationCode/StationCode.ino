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
    wifiManager.autoConnect("WeatherStation", "connect");
    pinMode(2, OUTPUT); // on-board led
    bme.begin(0x76);  
    pinMode(LIGHTSENSORPIN, INPUT);
}

void loop() {
    if(WiFi.status() == WL_CONNECTED) {
      if(!isConnected) {
        digitalWrite(onBoardLED, HIGH);
        isConnected = true;
      }


      // create json to send
      const size_t capacity = JSON_OBJECT_SIZE(8);
      DynamicJsonDocument doc(capacity);
      doc["Station"] = String(ESP.getChipId());
      doc["AirPressure"] = String(bme.readPressure());
      doc["Humidity"] = String(bme.readHumidity());
      doc["Light"] = String(analogRead(LIGHTSENSORPIN));
      doc["Temperature"] = String(bme.readTemperature());
      serializeJson(doc, Serial);
      Serial.println("");

      HTTPClient http;    //Declare object of class HTTPClient
 
      http.begin("http://51.145.50.160:5000/api/values");      //Specify request destination
      http.addHeader("Content-Type", "application/json");  //Specify content-type header

      String json;
      serializeJson(doc, json);
      int httpCode = http.POST(json);   //Send the request
      
      String payload = http.getString();                  //Get the response payload
 
      //Serial.println(httpCode);   //Print HTTP return code
      Serial.println(payload);    //Print request response payload
 
      http.end();
 
    } else {
      if(isConnected) {
        digitalWrite(onBoardLED, LOW); 
        isConnected = false;
      }
      Serial.println("WIFI disconnected!");
    }
    delay(30000);
}
