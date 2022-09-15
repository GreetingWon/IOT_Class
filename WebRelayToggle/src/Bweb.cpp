#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_Sensor.h>

const int RELAY = 15;
const char* ssid = "wonson";
const char* password = "wonson0000";

ESP8266WebServer server(80);

void handleRoot() {
  String message = (server.method() == HTTP_GET)?"GET":"POST";
  message += " " + server.uri() + "\n";
  for(uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + " : " + server.arg(i) + "\n";
    }
    message += "\nHello from ESP8266!\n";
    server.send(200, "text/plain", message);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  server.send(404, "text/plain", message);
}

void mypage() {
  String message = "<html><head><meta charset=\"utf-8\"><title>Test</title></head>"
                    "<body>"
                    "<head>my page<head>"
                    "한글"
                    "</body></html>";
  server.send(200, "text/html", message);
}

void On() {
  Serial.print("RELAY on\n");
  digitalWrite(RELAY, 1);

    String message = "<html><head><meta charset=\"utf-8\"><title>Test</title></head>"
                    "<body>"
                    "<head><head>"
                    "Relay ON"
                    "</body></html>";
  server.send(200, "text/html", message);
}

void Off() {
  Serial.print("RELAY off\n");
  digitalWrite(RELAY, 0);

  String message = "<html><head><meta charset=\"utf-8\"><title>Test</title></head>"
                    "<body>"
                    "<head><head>"
                    "Relay OFF"
                    "</body></html>";
  server.send(200, "text/html", message);
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while(WiFi.status() != WL_CONNECTED) { //Connecting
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to "); Serial.println(ssid);
  Serial.print("IP address : "); Serial.println(WiFi.localIP());

  if(MDNS.begin("WonSeongMin")) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  server.on("/inline", []() {
    server.send(200, "text/plain", "Hello from the inline function\n");
  });
  server.on("/mypage", mypage);
  server.onNotFound(handleNotFound);

  server.on("/On", On); //Call back function (void On)
  server.on("/Off", Off); //Call back function (void Off)

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  MDNS.update(); //update repeatedly
  server.handleClient();
}
