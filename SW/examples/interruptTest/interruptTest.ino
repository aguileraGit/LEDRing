#include <TimerOne.h>
#include <Wire.h>
#include <MMA8452.h>
#include <MMA8452Reg.h>
#include <Adafruit_NeoPixel.h>
#include <LowPower.h>
#include <avr/power.h>

//Debug LED
#define DEBUG_LED  9

//NeoPixels
#define PIXEL_PIN  6
#define PIXEL_COUNT 12

//System
MMA8452 accelerometer;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB);
volatile bool interrupt;
unsigned long timerCount;
#define SHUTDOWN  30000  //Shutdown - 1000ms = 1s
volatile bool shutDown;

//App specific Led
volatile bool updateBlinkLed;
volatile bool ledBlink;
volatile bool updateAllLeds;
volatile int updateAllCount;
volatile int currentPixel;

volatile int ledNumAtTap;
volatile int ledNum;
volatile int randLed;

#define GAME_SPEED_SLOW  80 
#define GAME_SPEED_FAST  20
#define GAME_SPEED       GAME_SPEED_SLOW

//Sleep
bool sleepEnabled;
bool activity;
unsigned long activityCount;


void setup() {
  
  //Enable Debug LED
  pinMode(DEBUG_LED, OUTPUT);

  //Start i2c (Wire) for accelerometer
  Wire.begin();

  bool initialized = accelerometer.init();

  if (initialized) {
    accelerometer.setDataRate(MMA_400hz);
    accelerometer.setRange(MMA_RANGE_2G);

    accelerometer.enableSingleTapDetector(MMA_X + MMA_Y + MMA_Z);
    accelerometer.enableDoubleTapDetector(MMA_X + MMA_Y + MMA_Z, 0x33, 0xBB);
    accelerometer.setTapThreshold(0x20, 0x20, 0x20);

    accelerometer.setInterruptsEnabled(MMA_TAP);
    accelerometer.configureInterrupts(false, false);
    accelerometer.setInterruptPins(false, false, false, true, false, false);

    attachInterrupt(0, accelerometerInterruptHandler, FALLING);
  }

  //Timer
  Timer1.initialize(1000); //1000ns = 1ms
  Timer1.attachInterrupt(oneMsTimer);

  //Neopixels
  strip.begin();
  strip.show();

  //App specific
  interrupt = false;
  sleepEnabled = true;
  ledBlink = false;
  activity = false;
  activityCount = 0;
  shutDown = false;

  updateAllCount = 0;
  currentPixel = 0;

  randomSeed(analogRead(0));
  randLed = random(PIXEL_COUNT);
}

void loop() {
  
  /********************************************************/
  // Deal with taps
  /********************************************************/
  if (interrupt) {
    interrupt = false;

    Timer1.stop();

    bool singleTap, doubleTap;
    bool x, y, z;
    bool negX, negY, negZ;

    //Clear it by reading from it. Duh!
    accelerometer.getTapDetails(&singleTap, &doubleTap, &x, &y, &z, &negX, &negY, &negZ);

    if (singleTap) {
      //Clear all
      for (int j = 0; j < PIXEL_COUNT; j++) {
        strip.setPixelColor(j, strip.Color(0, 0, 0));
        strip.show();
      }

      if (ledNumAtTap == randLed) {
        strip.setPixelColor(ledNumAtTap, strip.Color(0, 32, 0));
        strip.show();
      } else {
        strip.setPixelColor(ledNumAtTap, strip.Color(32, 0, 0));
        strip.show();
      }

      delay(800);
    }

    //Get new random LED
    randLed = random(PIXEL_COUNT);

    //Resume interrupt and timer operations
    Timer1.resume();

  } //End interrupt

  /********************************************************/
  // Set LEDs
  /********************************************************/
  if (updateBlinkLed) {
    Timer1.stop();
    ledBlink = !ledBlink;

    //Clear all
    for (int j = 0; j < PIXEL_COUNT; j++) {
      strip.setPixelColor(j, strip.Color(0, 0, 0));
      strip.show();
    }

    for (int i = 0; i < PIXEL_COUNT; i++) {
      if (i == currentPixel) {
        strip.setPixelColor(currentPixel, strip.Color(16, 16, 16));
      } else if (i == randLed) {
        if (ledBlink) {
          strip.setPixelColor(randLed, strip.Color(0, 0, 32));
        } else {
          strip.setPixelColor(randLed, strip.Color(0, 0, 0));
        }
      } else {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();
    }

    //Blinking on the target LED is faster than the other LEDs - Control the rate of all the LEDs here.
    updateAllCount++;
    if (updateAllCount >= 2) {
      updateAllCount = 0;

      currentPixel++;

      if (currentPixel >= PIXEL_COUNT) {
        currentPixel = 0;
      }

    }

    updateBlinkLed = false;
    Timer1.resume();
  }


  /********************************************************/
  // Sleep for a few milliseconds
  /********************************************************/
  if (sleepEnabled) {
    sleepFn();
  }

  /********************************************************/
  // Shut device down until a tap occurrs
  /********************************************************/
  if (shutDown) {

    digitalWrite(DEBUG_LED, LOW);

    for (int j = 0; j < PIXEL_COUNT; j++) {
      strip.setPixelColor(j, strip.Color(0, 0, 0));
      strip.show();
    }
    
    //Wait for tap
    while (!activity) {
      sleepFn();
    }

    activityCount = 0;
    shutDown = false;
  }

}




void accelerometerInterruptHandler() {
  interrupt = true;
  activity = true;
  ledNumAtTap = currentPixel;
}



void oneMsTimer() {
  /********************************************************/
  // Update every 1ms
  /********************************************************/
  timerCount++;

  /********************************************************/
  // Update every 50ms
  /********************************************************/
  if (timerCount % GAME_SPEED == 0) {
    updateBlinkLed = true;
  }

  /********************************************************/
  // Track activity
  /********************************************************/
  if (!activity) {
    activityCount++;
  } else {
    activityCount = 0;
    activity = false;
  }

  /********************************************************/
  // Shut down
  /********************************************************/
  if (activityCount == SHUTDOWN) {
    shutDown = true;
  }
}


void sleepFn() {
  LowPower.idle(SLEEP_15MS, ADC_OFF, TIMER2_OFF, TIMER1_ON, TIMER0_OFF, SPI_OFF, USART0_ON, TWI_ON);
}
