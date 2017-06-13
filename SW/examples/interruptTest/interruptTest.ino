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
bool interrupt;
unsigned long timerCount;
#define SHUTDOWN  5000  //Shutdown - 1000ms = 1s


//App specific Led
volatile bool updateBlinkLed;
volatile bool ledBlink;
volatile bool updateAllLeds;
volatile int updateAllCount;
volatile int currentPixel;


int ledNum;
unsigned char pixelValues[PIXEL_COUNT];


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
  
  //Init LEDs
  pixelValues[0] = 128;
  for(int i = 1; i < PIXEL_COUNT; i++){
    pixelValues[i] = 0;
  }
  updateAllCount = 0;
  currentPixel = 0;
}

void loop() {

  if (interrupt) {
    interrupt = false;

    bool singleTap, doubleTap;
    bool x, y, z;
    bool negX, negY, negZ;

    //Clear it by reading from it. Duh!
    accelerometer.getTapDetails(&singleTap, &doubleTap, &x, &y, &z, &negX, &negY, &negZ);

    //Debug - LED
    //delay(100);
    //digitalWrite(DEBUG_LED, HIGH);
  } //End interrupt


  if(updateBlinkLed){
    updateBlinkLed = false;
    
    if(ledBlink){
      for(int i = 1; i < PIXEL_COUNT; i++){
        if(i == currentPixel){
          strip.setPixelColor(currentPixel, strip.Color(8, 8, 8));
        } else if(i == 6) {
          strip.setPixelColor(6, strip.Color(32, 0, 0));
        } else {
          strip.setPixelColor(pixelValues[i], strip.Color(0, 0, 0));
        }
      }
      
      
  
    } else {
      for(int i = 1; i < PIXEL_COUNT; i++){
        if(i == currentPixel){
          strip.setPixelColor(currentPixel, strip.Color(8, 8, 8));
        } else if(i == 6) {
          strip.setPixelColor(6, strip.Color(0, 0, 0));
        } else {
          strip.setPixelColor(pixelValues[i], strip.Color(0, 0, 0));
        }
      }
    }
    strip.show();
    
    updateAllCount++;
    
    if(updateAllCount == 4){
      updateAllCount = 0;
      
      currentPixel++;
      
      if(currentPixel >= PIXEL_COUNT){
        currentPixel = 0;
      }
    }
  }
  

  //Sleep - power savings
  if (sleepEnabled) {
    sleepFn();
  }

}




void accelerometerInterruptHandler() {
  //digitalWrite(DEBUG_LED, HIGH);
  interrupt = true;
  activity = true;
}


void oneMsTimer() {
  //Update every 1ms
  timerCount++;

  //Update every 100ms
  if (timerCount % 100 == 0) {
    ;
  }
  
  //Update every 50ms
  if (timerCount % 200 == 0) {
    updateBlinkLed = true;
    ledBlink = !ledBlink;
    updateAllCount++;
  }
  
  //-------- Track no activity --------//
  if (activity == false) {
    activityCount++;
  } else {
    activityCount = 0;
    activity = false;
  }

  //-------- Shut Down --------//
  if (activityCount == SHUTDOWN) {
    activityCount = 0;

    //Turn off LED
    digitalWrite(DEBUG_LED, LOW);

    //Call sleep function.
    sleepFn();
  }
}


void sleepFn() {
  LowPower.idle(SLEEP_15MS, ADC_OFF, TIMER2_OFF, TIMER1_ON, TIMER0_OFF, SPI_OFF, USART0_ON, TWI_OFF);
}
