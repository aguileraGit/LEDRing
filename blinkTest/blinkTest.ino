#include <Wire.h>

/*
Select Arduino Pro or Pro Mini AND
328P 3.3V 8Mhz from Processor
Burn bootloader with 16Mhz
 */

#include <MMA8452.h>
#include <MMA8452Reg.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define DEBUG_LED  9
#define PIXEL_PIN  6
#define PIXEL_COUNT 12

MMA8452 accelerometer;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB);

//Program specific
int currentLED = 0;

void setup() {
  
  //Enable Debug LED
  pinMode(DEBUG_LED, OUTPUT);
  
  //Start i2c (Wire) for accelerometer
  Wire.begin();

  bool initialized = accelerometer.init(); 
  if (initialized) {
    accelerometer.setDataRate(MMA_800hz);	// we need a quick sampling rate
    accelerometer.setRange(MMA_RANGE_2G);

    accelerometer.enableSingleTapDetector(MMA_X + MMA_Y + MMA_Z);
    accelerometer.enableDoubleTapDetector(MMA_X + MMA_Y + MMA_Z, 0x33, 0xBB);
    accelerometer.setTapThreshold(0x30, 0x30, 0x30);
    
  } else {
    digitalWrite(DEBUG_LED, HIGH);
    while (true) {};
  }
  
  //Neopixels
  strip.begin();
  strip.show();
}


void loop() {
  bool singleTap;
  bool doubleTap;
  bool x;
  bool y;
  bool z;
  bool negX;
  bool negY;
  bool negZ;
  
  accelerometer.getTapDetails(&singleTap, &doubleTap, &x, &y, &z, &negX, &negY, &negZ);
  
  if (singleTap){
    digitalWrite(DEBUG_LED, HIGH);
    
    strip.setPixelColor(currentLED, strip.Color(0,150,0));
    strip.show();
    
    currentLED = currentLED + 1;
    if(currentLED >PIXEL_COUNT){
      currentLED = 0;
    }
  }
  
  if (doubleTap){
    digitalWrite(DEBUG_LED, LOW);
    
    int j = 0;
    for(j = 0; j< PIXEL_COUNT; j++){
      strip.setPixelColor(j, strip.Color(0,0,0));
      strip.show();
      delay(20);
    }
  }
  
  
}


