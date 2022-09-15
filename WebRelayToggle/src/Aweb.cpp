#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_Sensor.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "wonson";
const char* password = "wonson0000";
int state = 0;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  Serial.println("");
  
  pinMode(2, INPUT_PULLUP); //pin 2 (Rotary switch)

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.printf("Connected"); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());

  if(MDNS.begin("WonSeongMin")){ 
    Serial.println("MDNS responder started");
  }
}

void loop() {
  if (digitalRead(2) == LOW){ //switch pushed
      MDNS.update();
      WiFiClient Client;
      HTTPClient http;
      Serial.print("[HTTP] begin...\n");

    if(state == 0){ //relay off state
      if(http.begin(Client,"http://192.168.1.34/On")){ //Address of board B IP
         int httpCode = http.GET();
        if(httpCode >0) {
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY){
          String payload = http.getString();
          Serial.println(payload);
          }
        }else{
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
         } 
        http.end();
      }else{
        Serial.printf("[HTTP] Unable to connect\n");
      }
      delay(200);
      state = 1;
    }

    else if(state == 1){ //relay on state
      if(http.begin(Client,"http://192.168.1.34/Off")){ //Address of board B IP
         int httpCode = http.GET();
        if(httpCode >0) {
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);

         if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY){
            String payload = http.getString();
            Serial.println(payload);
          }
        }else{
           Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
          http.end();
        }else{
           Serial.printf("[HTTP] Unable to connect\n");
        }
        delay(200);
        state = 0;
    }
  }
}
