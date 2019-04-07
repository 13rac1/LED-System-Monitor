/*
  LED System Monitor
  2016 Brad Erickson

  Waits for two bytes on the serial port, then outputs to a TLC5947 to control
  two 10 LED bargraphs.

  Reduce CPU speed of Teensy to save power.
  https://www.pjrc.com/teensy/low_power.html
*/

#include "Adafruit_TLC5947.h"
#define NUM_TLC5974 1

#define data 13
#define clock 22
#define latch 23

#define maxBright 512

#define pulseBPM 10
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

// Store last and current frame times to find frame interval.
unsigned long lastMillis = 0;
unsigned long currentMillis = 0;
unsigned long frameMillis = 0;

// Time since event timers.
unsigned long disconnectStartMillis = 0;
unsigned long lastDataMillis = 0;

// State Management
enum states { START, CONNECT, RUN, NODATA, DISCONNECT, SLEEP };
states state = START;

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

// Store the CPU read byte.
byte inCPU = 1;
// Store the RAM read byte.
byte inRAM = 1;

void setup() {
  // Set all pins to ouput to save power.
  for (i = 0; i < pinCount; i++) {
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
  cleanFrames();

  // Calculate time since last frame.
  currentMillis = millis();
  frameMillis = currentMillis - lastMillis;

  calcPulse();

  // If serial is connected.
  if (Serial) {
    if (state == START || state == SLEEP) {
      state = CONNECT;
    }
  } else {
    // Serial not connected.
    if (state == CONNECT || state == RUN || state == NODATA) {
      state = DISCONNECT;
      disconnectStartMillis = currentMillis;
    }
  }

  // If Serial data available.
  if (Serial.available()) {
    // TODO: Care about two reads?
    inCPU = Serial.read();
    inRAM = Serial.read();
    state = RUN;
    lastDataMillis = currentMillis;
  }

  // Show warning after one second of no data.
  if (state == RUN) {
    bool noDataLimit = currentMillis - lastDataMillis > 1000;
    if (noDataLimit) {
      state = NODATA;
    }
  }

  // Only display disconnect for 5 seconds.
  if (state == DISCONNECT) {
    bool disconnectLimit = currentMillis - disconnectStartMillis > 5000;
    if (disconnectLimit) {
      state = SLEEP;
    }
  }

  // Draw frames depending on system state.
  switch (state) {
    case START:
      drawPulse();
      break;
    case CONNECT:
      drawConnect();
      break;
    case RUN:
      drawCPU(inCPU);
      drawRAM(inRAM);
      break;
    case NODATA:
      drawWarning();
      break;
    case DISCONNECT:
      drawDisconnect();
      break;
    case SLEEP:
      drawSleep();
      break;
  }

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

// Draw Functions

void drawCPU(byte percent) {
  const byte litLEDs = percent / 10;
  const byte remainder = percent % 10;

  for (i = 0; i < 10; i++) {
    if (i < litLEDs) {
      frameCPU[i] += 1 + pulse;
    } else if (i == litLEDs) {
      // Calculate how bright to make the highest LED
      // TODO: Gamma?
      frameCPU[i] += maxBright * remainder / 10;
    }
  }
}

void drawRAM(byte percent) {
  const byte litLEDs = percent / 10;
  const byte remainder = percent % 10;

  for (i = 0; i < 10; i++) {
    if (i < litLEDs) {
      frameRAM[i] += 1 + pulse;
    } else if (i == litLEDs) {
      // Calculate how bright to make the highest LED
      // TODO: Gamma?
      frameRAM[i] += maxBright * remainder / 10;
    }
  }
}

// Draw "sleep" by pulsing the blue LEDs.
void drawSleep() {
  frameCPU[0] += pulse;
  frameRAM[0] += pulse;
}

// Draw "connect" by pulsing two green LEDs.
void drawConnect() {
  frameCPU[1] += pulse;
  frameCPU[2] += pulse;
  frameRAM[1] += pulse;
  frameRAM[2] += pulse;
}

// Draw "disconnect" by pulsing two red LEDs.
void drawDisconnect() {
  frameCPU[8] += pulse;
  frameCPU[9] += pulse;
  frameRAM[8] += pulse;
  frameRAM[9] += pulse;
}

// Draw "warning" by pulsing two yellow LEDs.
void drawWarning() {
  frameCPU[6] += pulse;
  frameCPU[7] += pulse;
  frameRAM[6] += pulse;
  frameRAM[7] += pulse;
}

void drawPulse() {
  for (i = 0; i < 10; i++) {
    frameCPU[i] += pulse;
    frameRAM[i] += pulse;
  }
}
