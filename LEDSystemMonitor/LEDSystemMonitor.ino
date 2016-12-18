/*
  LED System Monitor
  2016 Brad Erickson eosrei.net

  Waits for two bytes on the serial port, then outputs to a TLC5947 to control two 10 LED bargraphs.

  Reduce CPU speed of Teensy to save power.
  https://www.pjrc.com/teensy/low_power.html
*/

#include "Adafruit_TLC5947.h"
#define NUM_TLC5974 1

#define data       13
#define clock      22
#define latch      23

#define maxBright  512

#define pulseBPM   10
#define pulseBright 64

#define millisPerMinute 60000

#define pinCount 24

// Pin 13: Arduino has an LED connected on pin 13
// Pin 11: Teensy 2.0 has the LED on pin 11
// Pin  6: Teensy++ 2.0 has the LED on pin 6
// Pin 13: Teensy 3.0 has the LED on pin 13
#define ledPin 11

Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5974, clock, data, latch);

// Init loop index only once.
byte i;

// TLC Pins for CPU display in low to high order.
const byte pinsCPU[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
// TLC Pins for RAM display in low to high order.
const byte pinsRAM[] = {14, 15, 16, 17, 18, 19, 20, 21, 22, 23};

// Frame of bytes for the CPU display.
int frameCPU[10];
// Frame of bytes for the RAM display.
int frameRAM[10];

// A heartbeat from 0 to maxBright running at pulseBPM.
int pulse = 0;

// Store last and current frame times to find frame interval.
unsigned long lastMillis = 0;
unsigned long currentMillis = 0;
unsigned long frameMillis = 0;

// Store the CPU read byte.
byte inCPU = 1;
// Store the RAM read byte.
byte inRAM = 1;

void setup() {
  // Set all pins to ouput to save power.
  for(i=0; i<pinCount; i++) {
    pinMode(i, OUTPUT);
  }
  // Shut off ADC to save power.
  ADCSRA = 0;

  // Start serial port at 9600 bps.
  Serial.begin(9600);

  // Start TLC5947
  tlc.begin();
}

void loop() {
  if (Serial.available()) {
    inCPU = Serial.read();
    inRAM = Serial.read();
  }

  // Calculate time since last frame.
  currentMillis = millis();
  frameMillis = currentMillis - lastMillis;
 
  calcPulse();
  cleanFrames();
  drawCPU(inCPU);
  drawRAM(inRAM);
  //drawPulse();
  writeFrames();

  // Update lastMillis.
  lastMillis = currentMillis;
}

long rangePulse;
// Value padding to keep math out of the floats.
const long mathPadding = 100;
const long paddedPulseBright = mathPadding * pulseBright;
const long pulseLength = millisPerMinute / pulseBPM;

void calcPulse() {
  // If pulseBright = 256.
  // rangePulse goes 0-512.
  // pulse goes 256-0-256.
  long xPulse = paddedPulseBright * 2 * frameMillis / pulseLength;

  rangePulse += xPulse;
  // Limit rangePulse to paddedPulseBright * 2
  rangePulse = rangePulse % (paddedPulseBright * 2);
  pulse = abs(rangePulse - paddedPulseBright) / mathPadding;
}

void cleanFrames() {
  for (i = 0; i < 10; i++) {
    frameCPU[i] = 0;
    frameRAM[i] = 0;
  }
}

void writeFrames() {
  for (i = 0; i < 10; i++) {
    tlc.setPWM(pinsCPU[i], frameCPU[i]);
    tlc.setPWM(pinsRAM[i], frameRAM[i]);
  }
  tlc.write();
}

void drawCPU(uint8_t percent) {
  const byte litLEDs = percent / 10;
  const byte remainder = percent % 10;

  for (i = 0; i < 10; i++) {
    if (i < litLEDs) {
      frameCPU[i] += 1 + pulse;
    }
    else if (i == litLEDs) {
      // Calculate how bright to make the highest LED
      // TODO: Gamma?
      frameCPU[i] += maxBright * remainder / 10;
    }
  }
}

void drawRAM(uint8_t percent) {
  const byte litLEDs = percent / 10;
  const byte remainder = percent % 10;

  for (i = 0; i < 10; i++) {
    if (i < litLEDs) {
      frameRAM[i] += 1 + pulse;
    }
    else if (i == litLEDs) {
      // Calculate how bright to make the highest LED
      // TODO: Gamma?
      frameRAM[i] += maxBright * remainder / 10;
    }
  }
}

void drawPulse() {
  for (i=0; i<10; i++) {
    frameRAM[i] += pulse;
  }
}

