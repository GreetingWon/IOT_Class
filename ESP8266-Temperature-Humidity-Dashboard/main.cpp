#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#define EEPROM_LENGTH 1024
#include<DHTesp.h>
#include <WiFiClient.h>

DHTesp dht;

char buf_temp[100];
char buf_humi[100];
char buf_light[100];

float humidity = 0;
float temperature = 0;
int light;
int flag =0;
HTTPClient http;
WiFiClient client;

char eRead[30];
byte len;
char ssid[30];
char password[30];
char IP[30] ;
bool captive = true;
const byte DNS_PORT = 53;

IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);



String responseHTML = ""
  "<!DOCTYPE html><html><meta charset=\"utf-8\">"
  "<head><title>CaptivePortal</title></head><body><center>"
  "<p>Captive Server에 접속하였습니다.</p>"
  "<form action='/save'>"
  "<p><input type='text' name='ssid' placeholder='SSID' onblur='this.value=removeSpaces(this.value);'></p>"
  "<p><input type='password' name='password' placeholder='WLAN Password'></p>"
  "<p><input type='text' name='IP' placeholder='IP Address'></p>"
  "<p><input type='submit' value='접속'></p></form>"
  "<p>접속버튼을 누르시면 해당 네트워크에 접속합니다.</p></center></body>"
  "<script>function removeSpaces(string) {"
  " return string.split(' ').join('');"
  "}</script></html>";

//------------------------------------버튼에서 저장하는 부분---------------------------------
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

void save() {
  Serial.println("button pressed");
  Serial.println(webServer.arg("ssid"));
  SaveString( 0, (webServer.arg("ssid")).c_str());
  SaveString(30, (webServer.arg("password")).c_str());
  SaveString(60, (webServer.arg("IP")).c_str());
  webServer.send(200, "text/plain", "OK");
  ESP.restart();
}
//---------------------------------End----------------------------------

IRAM_ATTR void GPIO0() {
  SaveString(0, ""); // blank out the SSID field in EEPROM
  ESP.restart();
}

//-----------------------captive portal setting-------------------------
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

//----------------------------------handleNotFound---------------------------------
void handleNotFound(){
String message = "File Not Found\n\n";
webServer.send(404, "text/plain", message);
}

//----------------------------runtime setup----------------------------------------

void setup_runtime() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  int i = 0;
  dht.setup(14,DHTesp::DHT22);
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
//---------------------------------------------End------------------------------------

void aa() {
  http.addHeader("Content-Type", "text/plain");
  
  light = analogRead(0);
  humidity = dht.getHumidity();                    
  temperature = dht.getTemperature();   
  int httpCode;

  sprintf(buf_temp,"temperature,host=server01,region=us-west value=%.2f",temperature);
  httpCode = http.POST(buf_temp);
  sprintf(buf_humi,"humidity,host=server01,region=us-west value=%.2f",humidity);
  httpCode = http.POST(buf_humi);
  sprintf(buf_light,"light,host=server01,region=us-west value=%d",light);
  httpCode = http.POST(buf_light);
  
   Serial.print("Light : "), Serial.println(light,DEC);  
   Serial.print("Temperature : "); Serial.print(temperature);     
   Serial.print(", Humidity : "); Serial.println(humidity);
   
  String payload = http.getString();
  Serial.println(httpCode);
  Serial.println(payload);
  http.end();
  delay(1000);
}



void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_LENGTH);
  ReadString(0, 30);
  
  if (!strcmp(eRead, "")) {
    setup_captive();
  } else {
    captive = false;
    strcpy(ssid, eRead);
    ReadString(30, 30);
    strcpy(password, eRead);
    setup_runtime();
  }
}

void loop() {
  if (captive) {
  dnsServer.processNextRequest();
  }
  MDNS.update();
  webServer.handleClient();
  if(flag==1){
    aa();
  }
}

