#include <Arduino.h>
#include <SSD1306.h>
#include <SPI.h>

SSD1306 display(0x3c, 4, 5 , GEOMETRY_128_32);

int x0 = 0;
int alpha = 0.9;
char buf[10];
char buf_rotary[10];

const int           pulseA = 12; 
const int           pulseB = 13; 
const int           pushSW = 2; 
volatile int        lastEncoded = 0; 
volatile long       encoderValue = 0; 

IRAM_ATTR void handleRotary() 
{ 
  // Never put any long instruction
  int MSB = digitalRead(pulseA); //MSB = most significant bit
  int LSB = digitalRead(pulseB); //LSB = least significant bit
  
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
  
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++; 
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;     
  lastEncoded = encoded; //store this value for next time
  
  if (encoderValue > 255) 
  {         
    encoderValue = 255;     
  } 
  else if (encoderValue < 0 ) 
  {         
    encoderValue = 0;     
  }
}

IRAM_ATTR void buttonClicked() 
{     
  Serial.println("pushed"); 
} 

void setup() 
{     
  display.init();
  display.setFont(ArialMT_Plain_16);
  display.display();
  Serial.begin(115200);     
  pinMode(pushSW, INPUT_PULLUP);     
  pinMode(pulseA, INPUT_PULLUP);     
  pinMode(pulseB, INPUT_PULLUP);     
  attachInterrupt(pushSW, buttonClicked, FALLING);     
  attachInterrupt(pulseA, handleRotary, CHANGE);     
  attachInterrupt(pulseB, handleRotary, CHANGE); 
  pinMode(15, OUTPUT); 
} 

void loop() 
{   
  display.init();
  display.flipScreenVertically();
  int x1 = analogRead(A0);
  x0 = x0 * alpha + (1 - alpha) * x1;
  display.drawString(80,0,"Bri: " + String(x0));
  
  if(x0 < encoderValue)
  {
    digitalWrite(15, HIGH);
    display.drawString(10,50,"Relay ON ");
  }
  else
  {
    digitalWrite(15, LOW);
    display.drawString(10,50,"Realy OFF");
  }

  display.drawString(0,0,"Rotary: " + String(encoderValue));
  display.display();
  delay(500);     
  Serial.println(encoderValue); 
} 
