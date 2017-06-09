#include <Average.h>
#include <LowPower.h>
#include <Wire.h>
#include <MMA8452.h>
#include <MMA8452Reg.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

/*
Select Arduino Pro or Pro Mini AND
328P 3.3V 8Mhz from Processor
Burn bootloader with 16Mhz

https://github.com/MajenkoLibraries/Average
https://github.com/rocketscream/Low-Power
https://github.com/adafruit/Adafruit_NeoPixel
https://github.com/akupila/Arduino-MMA8452
*/

//Debug LED
#define DEBUG_LED  9

//NeoPixels
#define PIXEL_PIN  6
#define PIXEL_COUNT 12

MMA8452 accelerometer;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB);

//Program specific
int rVal = 1;
int gVal = 1;
int bVal = 1;

enum progStates {
  R_STATE,
  B_STATE,
  G_STATE,
  RESET,
  SLEEP_STATE
};
enum progStates PROGSTATE;

bool fakeSleep;
int restoreR, restoreG, restoreB;

//Low Pass filter - Hopefully cuts down on the random "Sleep"
#define AVG_LENGTH  125
Average<float> ave(AVG_LENGTH);
#define AVG_THRESHOLD  -0.7

void setup() {
  //Enable Debug LED
  pinMode(DEBUG_LED, OUTPUT);

  //Start i2c (Wire) for accelerometer
  Wire.begin();

  bool initialized = accelerometer.init();
  if (initialized) {
    accelerometer.setDataRate(MMA_800hz);  // we need a quick sampling rate
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

  //Wake up
  fakeSleep = false;
  
  //Fill Average Array with zero's. -1 is down and +1 is up. Make it swiss.
  for(int j = 0;j<AVG_LENGTH;j++){
    ave.push(0);
  }
}


void loop() {
  
  //Get taps and double taps
  bool singleTap, doubleTap;
  bool x, y, z;
  bool negX, negY, negZ;
  accelerometer.getTapDetails(&singleTap, &doubleTap, &x, &y, &z, &negX, &negY, &negZ);
  
  //Get X value
  float xVal, yVal, zVal;
  accelerometer.getAcceleration(&xVal, &yVal, &zVal);
  
    //Average zVal and decide to sleep
  float currentAverage = ave.rolling(zVal);
  
  //Decide to sleep or wake
  if(currentAverage > AVG_THRESHOLD){
    fakeSleep = false;
  } else {
    //Update LEDs via function
    updateLEDS(0, 0, 0);
    fakeSleep = true;
    PROGSTATE = SLEEP_STATE;
    LowPower.idle(SLEEP_15MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_ON, USART0_OFF, TWI_OFF);
  }
  
  //Select R/G/B state
  if (doubleTap) {
    //digitalWrite(DEBUG_LED, HIGH);

    switch (PROGSTATE) {
      case R_STATE:
        if(fakeSleep == false){
          PROGSTATE = G_STATE;
        }
        break;
      case G_STATE:
        if(fakeSleep == false){
          PROGSTATE = B_STATE;
        }
        break;
      case B_STATE:
        if(fakeSleep == false){
          PROGSTATE = R_STATE;
        }
        break;
      case RESET:
        PROGSTATE = R_STATE;
        break;
      case SLEEP_STATE:
        //LowPower.idle(SLEEP_15MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
        digitalWrite(DEBUG_LED, HIGH);
        break;
      default:
        digitalWrite(DEBUG_LED, HIGH);
        break;
    }
  }//End Double Tap

  //Increase brightness
  if (singleTap) {
    //digitalWrite(DEBUG_LED, LOW);
    
    //Tap detected while sleeping - Time to wake up
    if(PROGSTATE == SLEEP_STATE && fakeSleep == true){
      //digitalWrite(DEBUG_LED, LOW);
      updateLEDS(1, 1, 1);
      PROGSTATE = R_STATE;
    } else {
      //digitalWrite(DEBUG_LED, HIGH);
      //LowPower.idle(SLEEP_15MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
      PROGSTATE = SLEEP_STATE;
    }
  
    if (PROGSTATE == R_STATE && fakeSleep == false) {
      rVal = rVal * 2;
      if (rVal > 255) {
        rVal = 1;
      }

    } else if (PROGSTATE == G_STATE && fakeSleep == false) {
      gVal = gVal * 2;
      if (gVal > 255) {
        gVal = 1;
      }

    } else if (PROGSTATE == B_STATE && fakeSleep == false) {
      bVal = bVal * 2;
      if (bVal > 255) {
        bVal = 1;
      }
    } else {
      //digitalWrite(DEBUG_LED, HIGH);
    }

    //Update LEDs via function
    updateLEDS(rVal, gVal, bVal);

  }//End Single Tap

}//End Main Loop


void updateLEDS(int rUpdate, int gUpdate, int bUpdate) {
  int j = 0;
  for (j = 0; j < PIXEL_COUNT; j++) {
    strip.setPixelColor(j, strip.Color(rUpdate, gUpdate, bUpdate));
    strip.show();
    delay(20);
  }
}

