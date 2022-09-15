#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
 
 
void setup()
{
  Wire.begin();
 
  Serial.begin(115200);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nI2C Scanner");
}
 
 
void loop()
{
  byte error, address;
 
  Serial.println("Scanning...");

  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of the Write.endTransmisstion to see if a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      Serial.print(address, HEX);
      Serial.println("  !");
 
    }    
  }
  delay(5000);           // wait 5 seconds for next scan
}

// I2C Scanning 결과 두개의 주소가 확인되어
// APDS9960 을 제거한 후 확인하니 알수없는 장치가 0x3C에 연결되어있음
// APDS9960 의 I2C 주소는 0x39임을 확인하였다.
