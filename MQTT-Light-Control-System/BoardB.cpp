#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

unsigned long       interval = 2000;

const char*         ssid[30];
const char*         password[30];
const char*         mqttServer[30];
const int           mqttPort = 1883;

int flag = 0;

char eRead[30];
byte len;
bool captive = true;
const byte DNS_PORT = 53;

unsigned long       pubInterval = 5000;
unsigned long       lastPublished = - pubInterval;
unsigned long       currentMillis = millis();

WiFiClient espClient;
PubSubClient client(espClient);
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

String responseHTML = ""
  "<!DOCTYPE html><html><meta charset=\"utf-8\">"
  "<head><title>CaptivePortal</title></head><body><center>"
  "<p>WiFi & MQTT Setup</p>"
  "<form action='/save'>"
  "<p><input type='text' name='ssid' placeholder='SSID' onblur='this.value=removeSpaces(this.value);'></p>"
  "<p><input type='password' name='password' placeholder='WLAN Password'></p>"
  "<p><input type='text' name='mqttServer' placeholder='MQTT Server'></p>"
  "<p><input type='submit' value='접속'></p></form>"
  "<p>WiFi & MQTT Setup Page</p></center></body>"
  "<script>function removeSpaces(string) {"
  " return string.split(' ').join('');"
  "}</script></html>";

void SaveString(int startAt, const char* id) {                                        
  for(byte i = 0; i <= strlen(id); i++) {                                                 
    EEPROM.write(i + startAt, (uint8_t) id[i]);                                         
  }                                                                                       
  EEPROM.commit();
} 

void ReadString(byte startAt, byte bufor) {
  for(byte i = 0; i <= bufor; i++) {
    eRead[i] = (char)EEPROM.read(i + startAt);
  }
}

IRAM_ATTR void GPIO0() {
  SaveString(0, "");
  ESP.restart();
}

void save() {
  Serial.println("button pressed");
  Serial.println(webServer.arg("ssid"));
  SaveString( 0, (webServer.arg("ssid")).c_str());
  SaveString(30, (webServer.arg("password")).c_str());
  SaveString(60, (webServer.arg("IP")).c_str());
  webServer.send(200, "text/plain", "OK");
  ESP.restart();
}

void callback(char* topic, byte* payload, unsigned int length){
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);

    Serial.print("Message: ");
    for (int i = 0; i < length; i++){
      Serial.print((char)payload[i]);
    }
    if((char)payload[0] == '1'){
      digitalWrite(15,HIGH);
    }
    else{
      digitalWrite(15,LOW);
    }
}

void setup_captive() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("WonCap");
  dnsServer.start(DNS_PORT, "*", apIP);
  webServer.on("/save", save);
  webServer.onNotFound([]() {
  webServer.send(200, "text/html", responseHTML);
  });
  webServer.begin();
  Serial.println("Captive Portal Started");
}

void setup_runtime() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(i++ > 15) {
      captive = true;
      setup_captive();
      return;
      }
    }
    
    Serial.println("");
    Serial.print("Connected to "); Serial.println(ssid);
    Serial.print("IP address: "); Serial.println(WiFi.localIP());
    http.begin(client, "http://3.90.5.110:8086/write?db=sensorDB");
    if (MDNS.begin("WonCap")) {
    Serial.println("MDNS responder started");
  }
  
  webServer.onNotFound(handleNotFound);
  webServer.begin();
  attachInterrupt(0,GPIO0,FALLING);
  Serial.println("HTTP server started");
  flag =1;
}

void aa(){
  http.addHeader("Content-Type", "text/plain");

  int httpCode;
  String payload = http.getString();
  Serial.println(httpCode);
  Serial.println(payload);
  http.end();
  delay(1000);
}
void setup() {
    Serial.begin(115200);
    EEPROM.begin(EEPROM_LENGTH);
    ReadString(0,30);
    pinMode(15,OUTPUT);
    if (!strcmp(eRead, "")) {
      setup_captive();
    } else {
    captive = false;
    strcpy(ssid, eRead);
    ReadString(30, 30);
    strcpy(password, eRead);
    setup_runtime();
  }

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to the WiFi network");

    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);

    while (!client.connected()) {
        Serial.println("Connecting to MQTT...");
        if (client.connect("ESP8266Client")) {
            Serial.println("connected");  
        } else {
            Serial.print("failed with state "); Serial.println(client.state());
            delay(2000);
        }
    }
  client.subscribe("deviceid/2016146033/cmd/lamp");
}

void loop() {
  client.loop(); 
  if (captive) {
  dnsServer.processNextRequest();
  }
  MDNS.update();
  webServer.handleClient();
  if(flag==1){
    aa();
  }
}
