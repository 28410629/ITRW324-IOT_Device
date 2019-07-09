#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>       
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>    
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h> 

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
        digitalWrite(onBoardLED, LOW);
        isConnected = true;
      }
      StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
      JsonObject& JSONencoder = JSONbuffer.createObject(); 
 
      JSONencoder["Station"] = String();
      JSONencoder["Temperature"] = String(bme.readTemperature());
      JSONencoder["Humidity"] = String(bme.readHumidity());
      JSONencoder["AirPressure"] = String(bme.readPressure());
      JSONencoder["Light"] = String(analogRead(LIGHTSENSORPIN));
 
      char JSONmessageBuffer[300];
      JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      Serial.println(JSONmessageBuffer);
 
      HTTPClient http;    //Declare object of class HTTPClient
 
      http.begin("http://51.143.152.177:5000/api/values");      //Specify request destination
      http.addHeader("Content-Type", "application/json");  //Specify content-type header
 
      int httpCode = http.POST(JSONmessageBuffer);   //Send the request
      String payload = http.getString();                                        //Get the response payload
 
      Serial.println(httpCode);   //Print HTTP return code
      Serial.println(payload);    //Print request response payload
 
      http.end();  //Close connection
    } else {
      if(isConnected) {
        digitalWrite(onBoardLED, HIGH); 
        isConnected = false;
      }
      Serial.println("WIFI disconnected!");
    }
    delay(5000);
}
