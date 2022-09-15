#include<Arduino.h>

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
float alpha = 0.2;
int ema = 0;
int inputPin = A0;

void setup() {
  // initialize serial communication with computer:
  Serial.begin(115200);
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop() {
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  int reading = analogRead(inputPin);
  readings[readIndex] = reading;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  //exponential movind average
  ema = reading * alpha + (1 - alpha) * ema;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
  Serial.printf("%d %d %d \n",reading,average,ema);

  delay(500);        // delay in between reads for stability
}
