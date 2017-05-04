#include <Wire.h>
#include <Adafruit_GFX.h>
#include "MAX30105.h"
#include "Adafruit_LEDBackpack.h"
#include "heartRate.h"

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
int love[4] = {76, 111, 118, 101};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }

  particleSensor.setup(); //Configure sensor. Use 6.4mA for LED drive
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  
  // Initialize display
  alpha4.begin(0x70); // pass in 7-bit Address
  char displaybuffer[12] = {'L', 'o', 'v', 'e', ' ', 'W', 'a', 't', 'c', 'h', ' ', ' '};
  for (int i=0; i < 12; i++) {
    // set every digit to the buffer
    alpha4.writeDigitAscii(0, displaybuffer[i % 12]);
    alpha4.writeDigitAscii(1, displaybuffer[(i+1) % 12]);
    alpha4.writeDigitAscii(2, displaybuffer[(i+2) % 12]);
    alpha4.writeDigitAscii(3, displaybuffer[(i+3) % 12]);
    alpha4.writeDisplay();
    delay(400);
 }

//  
//  alpha4.writeDigitAscii(0, love[0]);
//  alpha4.writeDigitAscii(1, love[1]);
//  alpha4.writeDigitAscii(2, love[2]);
//  alpha4.writeDigitAscii(3, love[3]);
//  alpha4.writeDisplay();
//  delay(5000);
//  alpha4.writeDigitAscii(0, 'W');
//  alpha4.writeDigitAscii(1, 'a');
//  alpha4.writeDigitAscii(2, 't');
//  alpha4.writeDigitAscii(3, 'c');
//  alpha4.writeDisplay();
//  delay(2000);
//  alpha4.writeDigitAscii(0, 'a');
//  alpha4.writeDigitAscii(1, 't');
//  alpha4.writeDigitAscii(2, 'c');
//  alpha4.writeDigitAscii(3, 'h');
//  alpha4.writeDisplay();
  delay(3000);
  alpha4.clear();
  alpha4.writeDisplay();

  
}

void loop() {
  // put your main code here, to run repeatedly:
  long irValue = particleSensor.getIR();
  int hundreds = 0;
  int tens = 0;
  int ones = 0;

  alpha4.writeDigitRaw(0,0); // top value should not generally be on.
  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  int intBPM = (int) beatsPerMinute;
  if (intBPM >= 0) {
    ones = intBPM % 10;
    alpha4.writeDigitAscii(3,48 + ones);
  } else {
    alpha4.writeDigitRaw(3,0);
  }
  
  if (intBPM > 9) {
    tens = (intBPM/10) % 10;
    alpha4.writeDigitAscii(2,48 + tens);
  } else {
    alpha4.writeDigitRaw(2,0);
  }
  if (intBPM > 99) {
    hundreds = (intBPM/100) % 10;
    alpha4.writeDigitAscii(1,48 + hundreds);
  } else {
    alpha4.writeDigitRaw(1,0);
  }

  if (beatsPerMinute > 80) {
    char displaybuffer[8] = {'L', 'o', 'v', 'e', ' ', ' ', ' ', ' '};
     for (int i=0; i < 16; i++) {
        // set every digit to the buffer
        alpha4.writeDigitAscii(0, displaybuffer[i % 8]);
        alpha4.writeDigitAscii(1, displaybuffer[(i+1) % 8]);
        alpha4.writeDigitAscii(2, displaybuffer[(i+2) % 8]);
        alpha4.writeDigitAscii(3, displaybuffer[(i+3) % 8]);
        alpha4.writeDisplay();
        delay(200);
     }
     beatsPerMinute = beatAvg;
     
  }
  
  
  alpha4.writeDisplay();
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  Serial.print(", Tens=");
  Serial.print(tens);
  Serial.print(", Ones=");
  Serial.print(ones);

  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();
  


}
