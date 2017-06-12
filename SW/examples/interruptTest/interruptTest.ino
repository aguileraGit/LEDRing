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

int ledNum;
unsigned char pixelValuesR[PIXEL_COUNT];
unsigned char pixelValuesG[PIXEL_COUNT];
unsigned char pixelValuesB[PIXEL_COUNT];

int randLed;
bool ledBlink;
int currentLed;

MMA8452 accelerometer;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB);

bool interrupt;

//Sleep
bool sleepEnabled;

//Shutdown - 1000ms = 1s
#define SHUTDOWN  5000

unsigned long timerCount;

int shutDownCount;

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

  } else {
    digitalWrite(DEBUG_LED, HIGH);
    while (true) {};
  }

  //Timer
  shutDownCount = 0;
  Timer1.initialize(1000); //1000ns = 1ms
  Timer1.attachInterrupt(oneMsTimer);

  //Neopixels
  strip.begin();
  strip.show();
  int i = 0;
  for (i = 0; i < PIXEL_COUNT; i++) {
    pixelValuesR[i] = 0;
    pixelValuesG[i] = 0;
    pixelValuesB[i] = 0;
  }

  interrupt = false;
  sleepEnabled = true;

  ledNum = 0;
  randLed = random(PIXEL_COUNT);
  ledBlink = false;
  currentLed = 0;
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
    delay(100);
    digitalWrite(DEBUG_LED, HIGH);


  } //End interrupt

  //Sleep
  if (sleepEnabled) {
    sleepFn();
  }

}




void accelerometerInterruptHandler() {
  digitalWrite(DEBUG_LED, HIGH);
  interrupt = true;
}


void oneMsTimer() {
  timerCount++;
  
  //Update LEDs every 10ms. 
  if (timerCount % 10 == 0) {
    currentLed++;
    
    ledBlink = !ledBlink;

    if(ledBlink){
      strip.setPixelColor(randLed, strip.Color(64, 0, 0));  
    }
    
  }

  //-------- Shut down Device --------//
  if (timerCount % SHUTDOWN == 0) {

    //Turn off LED
    digitalWrite(DEBUG_LED, LOW);

    //Call sleep function.
    sleepFn();
  }
}


void sleepFn() {
  LowPower.idle(SLEEP_15MS, ADC_OFF, TIMER2_OFF, TIMER1_ON, TIMER0_OFF, SPI_OFF, USART0_ON, TWI_OFF);
}
