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
int rVal = 1;
int gVal = 1;
int bVal = 1;

enum blinkStates {
  R_STATE,
  B_STATE,
  G_STATE,
  RESET
};
enum blinkStates PROGSTATE;

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
  
  //Set State
  PROGSTATE = R_STATE;
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
  
  //Advance state
  if (doubleTap){
    digitalWrite(DEBUG_LED, HIGH);
    delay(20);
    digitalWrite(DEBUG_LED, LOW);
    
    switch(PROGSTATE) {
      case R_STATE:
        PROGSTATE = G_STATE;
        break;
      case G_STATE:
        PROGSTATE = B_STATE;
        break;
      case B_STATE:
        PROGSTATE = R_STATE;
        break;
      case RESET:
        PROGSTATE = R_STATE;
        break;
      default:
        digitalWrite(DEBUG_LED, HIGH);
        break;   
    }
  }
  
  //Increase brightness
  if (singleTap){
    //digitalWrite(DEBUG_LED, LOW);
    
    if (PROGSTATE == R_STATE){
      rVal = rVal * 2;
      if(rVal > 255){
        rVal = 0;
      }
  
    } else if(PROGSTATE == G_STATE){
      gVal = gVal * 2;
      if(gVal > 255){
        gVal = 0;
      }
      
    } else if(PROGSTATE == B_STATE){
      bVal = bVal * 2;
      if(bVal > 255){
        bVal = 0;
      }
    } else {
      digitalWrite(DEBUG_LED, HIGH);
    }
    
    int j = 0;
    for(j = 0; j< PIXEL_COUNT; j++){
      strip.setPixelColor(j, strip.Color(rVal,gVal,bVal));
      strip.show();
      delay(20);
    }
    
 }//End Double Tap
  
  
}//End loop


