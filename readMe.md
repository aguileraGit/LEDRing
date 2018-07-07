
## Update
As best as I can remember, the mold needs to be solid. The silicon-ish material didn't stick well. And apparently I've decided to add a motor driver. I think the PCB is all working (v1.2). I really want to change out the LEDs so they're cheaper.

## READ ME
LED Ring (needs a better name) is an Arduino based platform. It contains a ring of LEDs, accelerometer, Atmel 328p, FTDI, USB (IP68), and debug LED. In the future the device will be encased in silicon-ish type material making the device IP68. Yes, you'll be able to dunk it in water.

## Hardware
A few minor updates need to be done to the HW. Mainly a capacitor to keep the FTDI from resetting and preventing uploads from Arduino IDE. I think there's a capacitor that needs a different footprint.

## Software
Current SW is interruptTest.ino. I really need to create a library to hide a lot of stuff (define DEBUG_LED for example).

Thanks to all who have created libraries and saved me countless hours. Mainly MMA8452 and NeoPixels!

## Mold
Four part mold. Two parts create the mold. The third holds a magnet that interns holds the PCB steady while pouring the silicon. The forth part contains a "nub" to prevent the PCB from wiggling around. This is meant to be printed at 0.1mm resolution. It needs to be glued on to the rest of the mold. The rest of the parts can be printed at 0.2mm.
- Bluesil V-249A/B.
- Hardness: 67A (Fitbit band has a hardness of 60)
- https://www.freemansupply.com/products/mold-making-silicone-rubber/bluestar-addition-cure-silicone-rubber/v-249-mold-making-silicone-rubber-heat-cure
 - Need to upload the 3d mold models.
 - Need to upload print settings.


#### Old references - Might not even be using some of these.
https://github.com/MajenkoLibraries/Average
https://github.com/rocketscream/Low-Power
https://github.com/adafruit/Adafruit_NeoPixel
https://github.com/akupila/Arduino-MMA8452


#### Or these
http://www.engblaze.com/hush-little-microprocessor-avr-and-arduino-sleep-mode-basics/
http://rubenlaguna.com/wp/2008/10/15/arduino-sleep-mode-waking-up-when-receiving-data-on-the-usart/
https://www.arduino.cc/en/Tutorial/SerialEvent
http://donalmorrissey.blogspot.com/2011/11/sleeping-arduino-part-4-wake-up-via.html
