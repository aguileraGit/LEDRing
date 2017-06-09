#include <Wire.h>
#include <MMA8452.h>
#include <MMA8452Reg.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>


//Debug LED
#define DEBUG_LED  9

//NeoPixels
#define PIXEL_PIN  6
#define PIXEL_COUNT 12

MMA8452 accelerometer;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB);

bool interrupt;

enum progStates {
  R_STATE,
  B_STATE,
  G_STATE,
  RESET,
  SLEEP_STATE
};
enum progStates PROGSTATE;

void setup() {
  //Enable Debug LED
  pinMode(DEBUG_LED, OUTPUT);
  //Start i2c (Wire) for accelerometer
  Wire.begin();

  bool initialized = accelerometer.init();
  if (initialized) {

    accelerometer.setDataRate(MMA_1_56hz);	// 800hz doesn't trigger an interrupt
    accelerometer.setRange(MMA_RANGE_2G);
    accelerometer.setInterruptsEnabled(MMA_DATA_READY);
    accelerometer.configureInterrupts(false, false);
    accelerometer.setInterruptPins(true, true, true, true, true, true);

    attachInterrupt(0, accelerometerInterruptHandler, FALLING);

  } else {
    digitalWrite(DEBUG_LED, HIGH);
    while (true) {};
  }
  
  //Neopixels
  strip.begin();
  strip.show();
  
  interrupt = false;
}

void loop() {
	if (interrupt)
	{
		interrupt = false;
		bool wakeStateChanged;
		bool movementOccurred;
		bool landscapePortrait;
		bool tap;
		bool freefall;
		bool dataReady;
		accelerometer.getInterruptEvent(&wakeStateChanged, &movementOccurred, &landscapePortrait, &tap, &freefall, &dataReady);

		// if (dataReady)
		// {
			// clear flag so new data will be read
			uint16_t x, y, z;
			accelerometer.getRawData(&x, &y, &z);

			cli();
			digitalWrite(6, LOW);
			sei();
		// }
	}
	digitalWrite(DEBUG_LED, HIGH);
	delay(100);
	digitalWrite(DEBUG_LED, LOW);
	delay(100);
}




void accelerometerInterruptHandler() {
  digitalWrite(DEBUG_LED, HIGH);
  interrupt = true;
}
