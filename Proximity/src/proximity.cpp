#include <Arduino.h>
#include <SparkFun_APDS9960.h>
#include <SPI.h>

SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;
const int RELAY = 15;

void ProximityReading(){
  uint8_t val = apds.getProximityGain();
  if(apds.readProximity(val)){
    if (val > 50) {
      digitalWrite(RELAY, ON);
    }
    else  {
      digitalWrite(RELAY, OFF);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY,OUTPUT);
  Serial.println("Stating APDS-9960 Proximity");
  if( apds.init()){
    Serial.println("APDS--9960 initialization complete");
  } else {
    Serial.println("Something went wrong during APDS-9960 init!");
  }

  if( apds.setProximityGain(PGAIN_2X)){
    Serial.println("APDS--9960 initialization complete");
  } else {
    Serial.println("Something went wrong during APDS-9960 init!");
  }

  if( apds.enableProximitySensor(true)){
    Serial.println("Proximity Sensor is now running");
  } else {
    Serial.println("Something went wrong during Proximity Sensor init!");
  }
}

void loop() {
  ProximityReading();
}
