#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <DHTesp.h>

#define ledPin 15
#define ledNum 4

/* For Rotary Encoder */
const int pulseA = 12;
const int pulseB = 13;
const int pushSW = 2;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;

/* For MQTT */
const char*     ssid = "IoT518";               //change to your ssid
const char*     password = "iot123456";      // change to your password
const char*     mqttServer = "34.193.131.206";
const int       mqttPort = 1883;
char            msg[100];
char            weather_name[100];
int             interval = 10000;
unsigned long   lastPublished = - interval;

WiFiClient        espClient;
PubSubClient      client(espClient);
Adafruit_NeoPixel pixels;

/* For Mood Lamp */
int R, G, B;
int illumination;
int brightness;
int lamp_brightness;
char Mood_Lamp_Mode;
int Color;
char Color_name[100];
char Publish_flag;
char LampCtrl = 1;

/* For DHT22 */
DHTesp dht;
char buf_temp[100];
char buf_humi[100];
float Temperature;
float Humidity;
unsigned long lastDHTReadMillis;

void readDHT22() {
    unsigned long currentMillis = millis();
        if(currentMillis - lastDHTReadMillis >= interval) {
        lastDHTReadMillis = currentMillis;

        Humidity = dht.getHumidity();              // Read humidity (percent)
        Temperature = dht.getTemperature();        // Read temperature as Fahrenheit
    }
}

void select_color_user(int Color)
{
  switch(Color/10)
  {
  case 0: 
    R = random(0, 255); G = random(0, 255); B = random(0, 255);
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Random", 100);
    break;

  case 1: 
    R = 255; G = 0; B = 0;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Red", 100);
    break;
    
  case 2: 
    R = 255; G = 94; B = 0;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Light Orange", 100);
    break;

  case 3: 
    R = 255; G = 187; B = 0;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Selective Yellow", 100);
    break;

  case 4: 
    R = 255; G = 228; B = 0;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Light Yellow", 100);
    break;

  case 5: 
    R = 171; G = 242; B = 0;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Green", 100);
    break;

  case 6: 
    R = 29; G = 219; B = 22;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Light Green", 100);
    break;
  
  case 7: 
    R = 0; G = 216; B = 255;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Sky Blue", 100);
    break;
  
  case 8: 
    R = 0; G = 84; B = 255;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Navy Blue", 100);
    break;

  case 9: 
    R = 1; G = 0; B = 255;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Navy", 100);
    break;

  case 10: 
    R = 95; G = 0; B = 255;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Electric Indigo", 100);
    break;

  case 11: 
    R = 255; G = 0; B = 221;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Dark pink", 100);
    break;

  case 12: 
    R = 255; G = 0; B = 127;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Bright Pink", 100);
    break;

  case 13:
    R = 255; G = 255; B = 255;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "White", 100); 
    break;

  case 14:  
    R = 0; G = 0; B = 0;
    memset(Color_name, 0, 100);
    memcpy(Color_name, "Black", 100);    
    break;    
  }
}

void select_color_weather()
{
  if(strcmp(weather_name, "clear sky") == 0) // selective_yellow
  {
    R = 255; G = 187; B = 0;
  }
  else if(strcmp(weather_name, "few clouds") == 0) // sky_blue
  {
    R = 0; G = 216; B = 255;
  }
  else if(strcmp(weather_name, "scattered clouds") == 0) // sky_blue
  {
    R = 0; G = 216; B = 255;
  }
  else if(strcmp(weather_name, "broken clouds") == 0) // sky_blue
  {
    R = 0; G = 216; B = 255;
  }
  else if(strcmp(weather_name, "shower rain") == 0) // electric_indigo
  {
    R = 95; G = 0; B = 255;
  }
  else if(strcmp(weather_name, "rain") == 0) // electric_indigo
  {
    R = 95; G = 0; B = 255;
  }
  else if(strcmp(weather_name, "thunderstorm") == 0) // light yellow
  {
    R = 255; G = 228; B = 0;
  }
  else if(strcmp(weather_name, "snow") == 0) // white
  {
    R = 255; G = 255; B = 255;
  }
  else if(strcmp(weather_name, "mist") == 0) // gray
  {
    R = 140; G = 140; B = 140;
  }
  else if(strcmp(weather_name, "overcast clouds") == 0) // gray
  {
    R = 120; G = 120; B = 120;
  }
  else // black
  {
    R = 0; G = 0; B = 0;
  }
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  delay(500);

  Serial.print("Message: ");
  for (int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
    msg[i] = (char)payload[i];
  }
  Serial.print("\n");

  if(strcmp(topic, "cloud/weather") == 0)
  {
    memset(weather_name, NULL, 100);
    memcpy(weather_name, msg, 100);
    Serial.println("Weather Mode"); 
  }
  else if(strcmp(topic, "lamp") == 0)
  {
    if(strcmp(msg, "on") == 0)
    {
      Serial.println("Lamp On");
      LampCtrl = 1;  
    }
    else if(strcmp(msg, "off") == 0)
    {
      Serial.println("Lamp Off");
      LampCtrl = 0;
    }
  }
  memset(msg, 0, 100);
}

IRAM_ATTR void handleRotary()
{
  // Never put any long instruction
  int MSB = digitalRead(pulseA); // MSB
  int LSB = digitalRead(pulseB); // LSB

  int encoded = (MSB << 1) | LSB; // converting the 2 pin value to single number
  int sum = (lastEncoded << 2) | encoded; // adding it to the previous encoded value
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
  {
    encoderValue++;
    Color = encoderValue;
  } 
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue--;
  lastEncoded = encoded; // store this value for next time
  
  if(encoderValue > 149)
  {
    encoderValue = 0;
  }
  else if (encoderValue < 0)
  {
    encoderValue = 149;
  }
}

IRAM_ATTR void buttonClicked()
{
  Serial.println("Mood Lamp Mode Changed");
  Mood_Lamp_Mode = !Mood_Lamp_Mode;
}

void setup() 
{
  Serial.begin(115200);

  /* Mood Lamp setup */
  pixels = Adafruit_NeoPixel(ledNum, ledPin, NEO_GRB + NEO_KHZ800);
  
  /* Rotary Encoder setup */
  pinMode(pushSW, INPUT_PULLUP);
  pinMode(pulseA, INPUT_PULLUP);
  pinMode(pulseB, INPUT_PULLUP);
  attachInterrupt(pushSW, buttonClicked, FALLING);
  attachInterrupt(pulseA, handleRotary, CHANGE);
  attachInterrupt(pulseB, handleRotary, CHANGE);

  /* MQTT setup */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
  Serial.println("Connecting to MQTT...");
  if (client.connect("ESP8266Client")) {
      Serial.println("connected");
  }else {
      Serial.print("failed with state "); Serial.println(client.state());
      delay(2000);
    }
  }
  client.subscribe("cloud/weather");
  client.subscribe("lamp");
  
  dht.setup(14, DHTesp::DHT22); // Connect DHT sensor to GPIO 14
  pixels.begin();
  delay(500);
  Serial.println("Starting");
}

void loop() 
{  
  /* MQTT Loop */
  client.loop();

  /* Upload Temperature and Humidity */
  unsigned long currentMillis = millis();
  if(currentMillis - lastPublished >= interval) {
      lastPublished = currentMillis;
      readDHT22();
      char buf[10];
      sprintf(buf, "%.1f", Temperature);
      client.publish("evt/temperature", buf);
      sprintf(buf, "%.1f", Humidity);
      client.publish("evt/humidity", buf);
      Serial.printf("Tmp : %.1f\tHmd : %.1f\t\n", Temperature, Humidity);
  }

  /* Determination of bright value */
  /* Maximum of light sensor value = 1024 */
  /* Maximum of bright value = 255 */
  /* Change the brightness gradually */
  illumination = analogRead(0);
  brightness = illumination / 4;
  if((brightness - 10 < lamp_brightness) && (lamp_brightness < brightness + 10))
  {
    lamp_brightness = brightness;
  }
  else
  {
    if(brightness > lamp_brightness)
    {
      lamp_brightness += 10;
    }
    else
    {
      lamp_brightness -= 10;
    }
  }  
  pixels.setBrightness(lamp_brightness);

  if(LampCtrl) // Lamp On
  {
    /* Mood Lamp Mode */
    if(!Mood_Lamp_Mode)
    {
      if(Publish_flag)
      {
        Publish_flag = !Publish_flag;
      }
      else
      {
        /* mood lamp case 1 : color selected by user */
        for (int i = 0; i < ledNum; i++)
        {
          select_color_user(Color);
          pixels.setPixelColor(i, pixels.Color(R,G,B));
          yield();
        }
        pixels.show();
        Serial.printf("Select Color : %s, Illumintion : %d, Brightness : %d Lamp_Brightness : %d\n\n", Color_name, illumination, brightness, lamp_brightness);
        delay(1000);
      }
    }
    else 
    {
      /* mood lamp case 2 : color selected by weather */
      if(!Publish_flag)
      {
        client.publish("pi", "2");
        Serial.println("Weather Information Request...");
        Publish_flag = !Publish_flag;
      }
      else
      {
        for (int i = 0; i < ledNum; i++)
        {
          select_color_weather();
          pixels.setPixelColor(i, pixels.Color(R,G,B));
          yield();
        }
        pixels.show();
        Serial.printf("Weather : %s, Illumintion : %d, Brightness : %d Lamp_Brightness : %d\n\n", weather_name, illumination, brightness, lamp_brightness);
        delay(1000);
      }
    }
  }
  else // Lamp Off
  {
    pixels.setBrightness(0);
    pixels.show();
  }  
}   
