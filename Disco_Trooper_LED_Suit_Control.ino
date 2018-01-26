#include <TimerOne.h>
#include <LPD6803.h>

// Disco Trooper Mirror Suit LED Control
// 2012 Chris Lee
// Original code by Bliptronics.com Ben Moyes 2009 to control LPD6803-based RGB LED Modules in a strand
// Code cleaned up and Object-ified by ladyada, should be a bit easier to use

/*****************************************************************************/

// Choose which 2 pins you will use for output. Can be any valid output pins.
int dataPin = 2;       // 'yellow' wire
int clockPin = 3;      // 'green' wire
int numberOfPixels = 28;// Don't forget to connect 'blue' to ground and 'red' to +5V
int potOnePin = 0;      // pot 1 analog pin
int potTwoPin = 1;      // pot 2 analog pin
int potOneVal = 0;
int potTwoVal = 0;
int potMax = 950;
int potMin = 50;
int potOneZone = 1;
int potTwoZone = 1;
int potTwoColor = 1;

// Timer 1 is also used by the strip to send pixel clocks

// Set the first variable to the NUMBER of pixels. 20 = 20 pixels in a row
LPD6803 strip = LPD6803(numberOfPixels, dataPin, clockPin);


// 23, 0
// 24, 1
// 25, 2
// 26, 3
// 27, 4
// 10, 5
// 11, 6
// 12, 7
// 13, 8
// 14, 9

int chasePatternOne[30] = {
  23, 0, 15, 24, 1, 16, 25, 2, 22, 26, 3, 17, 27, 4, 21, 10, 5, 18, 11, 6, 20, 12, 7, 19, 13, 8, 15, 14, 9, 19};

int chasePatternTwo[28] = {
  0,1,2,3,4,5,6,7,8,9,22,21,20,19,10,11,12,13,14,23,24,25,26,27,15,16,17,18};

int colorSet_USA[56] = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
  1,1,1,255,255,69,69,69,69,69,69,69,69,69,69,1,1,255,69,69,69,255,1,1,1,1,255,255};


void setup() {

  // The Arduino needs to clock out the data to the pixels
  // this happens in interrupt timer 1, we can change how often
  // to call the interrupt. setting CPUmax to 100 will take nearly all all the
  // time to do the pixel updates and a nicer/faster display, 
  // especially with strands of over 100 dots.
  // (Note that the max is 'pessimistic', its probably 10% or 20% less in reality)

  strip.setCPUmax(80);  // start with 50% CPU usage. up this if the strand flickers or is slow

  // Start up the LED counter
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();

  Serial.begin(9600);    // set up Serial library at 9600 bps

}


void loop() {
  potOneVal = analogRead(potOnePin);    // read the value from pot 1
  potTwoVal = analogRead(potTwoPin);    // read the value from pot 2

  potOneZone = map(potOneVal, potMin, potMax, 1, 14);  // set last number = total number of light patterns
  potTwoZone = map(potTwoVal, potMin, potMax, 1, 5);
  potTwoColor = map(potTwoVal, potMin, potMax, 1, 90);

//  Serial.println(potOneZone);

  switch (potOneZone) {
  case 1: 
    allwhite();
    break;
  case 2:
    singleOn(map(potTwoVal, potMin, potMax, 0, 27));
    break;
  case 3:
    rainbow(50);
    break;
  case 4:
    lateralTwoColor(potTwoColor);
    break;
  case 5:
    chase2(255, 255, 255);
    break;
  case 6:
    rainbowCycle(50);
    break;
  case 7:
    pulseUp(potTwoZone * 25);
    break;
  case 8:
    pulseDown(potTwoZone * 25);
    break;
  case 9:
    chaseDown(potTwoColor);
    break;
  case 10:
    chaseUp(potTwoColor);
    break;
  case 11:
    colorRandom();
    break;
  case 12:
    colorSet();
    break;
  case 13:
    twinkle(potTwoZone * 30);
    break;
  case 14:
    blackOut();
    break;
  }
  delay(20);
}

void colorSet() {
  int i, j;
  for (j=0; j < 28; j++) {    
  i = colorSet_USA[j+28];
  if (i == 255)
  {
  strip.setPixelColor(j, Color(i, i, i));
  }
  else
  {
  strip.setPixelColor(j, Wheel(i));
  }
  }
  strip.show();
  delay(25);
}


void colorRandom() {
  int i, j; 
  j=random(strip.numPixels()+1);
  strip.setPixelColor(j, Wheel(random(127)));
  strip.show();
  delay(30);
}


void twinkle(uint8_t wait) {
  int i, j; 
  for (i=0; i < strip.numPixels(); i++) {    
    strip.setPixelColor(i, Color(0, 0, 0));
  }
  j=random(strip.numPixels()+1);
  strip.setPixelColor(j, Color(255, 255, 255));
  strip.show();
  delay(wait);
}


void chaseDown(uint8_t c) {  
  int i, j, k;

  for (i=0; i < 30; i = i + 3) {    
    for (j=0; j < strip.numPixels(); j++) {    
      strip.setPixelColor(j, 0);
    }
    for (k=0; k < 3; k++) {    
      strip.setPixelColor(chasePatternOne[i+k], Wheel(c));
    }
    strip.show();
    delay(30);
  } 
}

void chaseUp(uint8_t c) {  
  int i, j, k;
    Serial.println(c);

  for (i=30; i > 0; i = i - 3) {    
    for (j=0; j < strip.numPixels(); j++) {    
      strip.setPixelColor(j, 0);
    }
    for (k=0; k < 3; k++) {    
      strip.setPixelColor(chasePatternOne[i+k], Wheel(c));
    }
    strip.show();
    delay(30);
  } 
}


void rainbow(uint8_t wait) {
  int i, j;

  for (j=0; j < 96; j++) {     // cycles of all 96 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 96));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}


void pulseDown(uint8_t wait) {   // can't figure out why this isn't working as intended
  for (int j=1; j < 254; j = j + 5) {    
    for (int i=0; i < strip.numPixels(); i++) {    
      strip.setPixelColor(i, Color(j, j, j));
    }
    strip.show();
    delay(wait);
  }
}

void pulseUp(uint8_t wait) {   // can't figure out why this isn't working as intended
  for (int j=1; j < 254;  j = j + 5) {    
    for (int i=0; i < strip.numPixels(); i++) {    
      strip.setPixelColor(i, Color(255-j, 255-j, 255-j));
    }
    strip.show();
    delay(wait);
  }
}


void singleOn(uint8_t pixel) {
  int j;
  for (j=0; j < strip.numPixels(); j++) {    
    strip.setPixelColor(j, 0);
  }
  strip.setPixelColor(pixel, Color(255, 255, 255));
  strip.show();
  delay(30);
}

void lateralTwoColor(uint8_t c) {
  int j;

  for (j=0; j < 14; j++) {    
    strip.setPixelColor(chasePatternTwo[j], Wheel(c));
  }
  for (j=14; j < 28; j++) {    
    strip.setPixelColor(chasePatternTwo[j], Color(127, 127, 127));
  }
  strip.show();
  delay(150);

  for (j=0; j < 14; j++) {    
    strip.setPixelColor(chasePatternTwo[j], Color(127, 127, 127));
  }
  for (j=14; j < 28; j++) {    
    strip.setPixelColor(chasePatternTwo[j], Wheel(c));
  }
  strip.show();
  delay(150);

}

void chase2(uint8_t r, uint8_t g, uint8_t b) {
  int i, j;

  for (i=0; i < strip.numPixels(); i++) {    
    for (j=0; j < strip.numPixels(); j++) {    
      strip.setPixelColor(j, 0);
    }
    strip.setPixelColor(i, Color(r, g, b));
    strip.show();
    delay(100);
  } 
}

void allwhite() {
  int i, j;

  j = map(potTwoVal, potMin, potMax, 0, 30);
  //  Serial.println(j);
  for (i=0; i < strip.numPixels(); i++) {    
    strip.setPixelColor(i, Color(j, j, j));
    strip.show();
    delay(20);
  } 
}

void blackOut() {
  int i;

  for (i=0; i < strip.numPixels(); i++) {    
    strip.setPixelColor(i, Color(0, 0, 0));
    strip.show();
    delay(20);
  } 
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;

  for (j=0; j < 96; j++) {     // cycles of all 96 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 96 / strip.numPixels()) + j) % 96) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint16_t c) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(20);
  }
}

/* Helper functions */

// Create a 15 bit color value from R,G,B
unsigned int Color(byte r, byte g, byte b)
{
  //Take the lowest 5 bits of each value and append them end to end
  return( ((unsigned int)g & 0x1F )<<10 | ((unsigned int)b & 0x1F)<<5 | (unsigned int)r & 0x1F);
}

//Input a value 0 to 127 to get a color value.
//The colours are a transition r - g -b - back to r
unsigned int Wheel(byte WheelPos)
{
  byte r,g,b;
  switch(WheelPos >> 5)
  {
  case 0:
    r=31- WheelPos % 32;   //Red down
    g=WheelPos % 32;      // Green up
    b=0;                  //blue off
    break; 
  case 1:
    g=31- WheelPos % 32;  //green down
    b=WheelPos % 32;      //blue up
    r=0;                  //red off
    break; 
  case 2:
    b=31- WheelPos % 32;  //blue down 
    r=WheelPos % 32;      //red up
    g=0;                  //green off
    break; 
  }
  return(Color(r,g,b));
}
























