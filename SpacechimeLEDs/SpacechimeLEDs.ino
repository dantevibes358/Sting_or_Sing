/*********************************************************
This is a library for the MPR121 12-channel Capacitive touch sensor

Designed specifically to work with the MPR121 Breakout in the Adafruit shop 
  ----> https://www.adafruit.com/products/

These sensors use I2C communicate, at least 2 pins are required 
to interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.  
BSD license, all text above must be included in any redistribution
**********************************************************/

#include <Wire.h>
#include <Adafruit_MPR121.h>
#include <Adafruit_NeoPixel.h>

#define PIN  6
#define NUMPIXELS  4

Adafruit_NeoPixel glow = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
String glowSt = "static";
float r[4], g[4], b[4], rt[4], gt[4], bt[4] = {0,0,0,0};

Adafruit_MPR121 cap = Adafruit_MPR121();
// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

void setup() 
{
  //while (!Serial1);        // needed to keep leonardo/micro from starting too fast!
  Serial.begin(9600);
  Serial1.begin(9600);  //Serial1 is the RX/TX data port
  //cap.begin(0x5C);
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) 
  {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial1.println("1");
  
  glow.begin();
  //glow.setBrightness(30);
}

void loop() 
{
  
  // Get the currently touched pads
  currtouched = cap.touched();
  
  for (uint8_t i=0; i<12; i++) 
  {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) 
    {
      if(i<10){Serial1.print(0);}
      Serial1.print(i); Serial1.println(" T");
      glowSt = "touched";
    }
    
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) 
    {
      if(i<10){Serial1.print(0);}
      Serial1.print(i); Serial1.println(" R");
      glowSt = "static";
    }
  }
  // reset our state
  lasttouched = currtouched;


//---------------lighting effects----------
  if(glowSt.equals("touched")) //Turn Pink+Orange!
  { 
    for(int i = 0; i<4; i++)
    {
     if(i%2 == random(2)){r[i] = 255; b[i] = 0; g[i] = 20;}
     else {r[i] = 225; b[i] = 220; g[i] = 0;}
    }
  }
  
  if(millis()%100 == 0) //periodically generate a random target value for each LED's bt & gt
  { 
    for(int i = 0; i<4; i++)
    {
     gt[i] = random(255);
     bt[i] = random(255);
    }
  }
  
  for(int i = 0; i<4; i++) //drift towards that target value
  { 
     r[i] = drift(r[i],rt[i]);
     b[i] = drift(b[i],bt[i]);
     g[i] = drift(g[i],gt[i]);
  }
  
  for(int i = 0; i<4; i++)
  {
     glow.setPixelColor(i, glow.Color(r[i], g[i], b[i]));
  }
   glow.show();

  return;
  
  /*   // raw data:
  Serial.print("\t\t\t\t\t\t\t\t\t\t\t\t\t 0x"); Serial.println(cap.touched(), HEX);
  Serial.print("Filt: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.filteredData(i)); Serial.print("\t");
  }
  Serial.println();
  Serial.print("Base: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.baselineData(i)); Serial.print("\t");
  }
  Serial.println();
  
  // put a delay so it isn't overwhelming
  delay(70); */
}

float drift(float value, float target)
{
  if(value < target){return value + 0.5;}
  else if(value > target){return value - 0.5;}
  else {return value;}
}

